/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/settings/settings.h>

static uint8_t _BatteryLevel = 20;
static bool _IsTxEnabled = false;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_DIS_VAL)),
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xc1, 0x03), // #APPEARENCE
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static int settings_runtime_load(void)
{
#if defined(CONFIG_BT_DIS_SETTINGS)
	settings_runtime_set("bt/dis/model", "Zephyr Model", sizeof("Zephyr Model"));
	settings_runtime_set("bt/dis/manuf", "Zephyr Manufacturer", sizeof("Zephyr Manufacturer"));
#if defined(CONFIG_BT_DIS_SERIAL_NUMBER)
	settings_runtime_set("bt/dis/serial", CONFIG_BT_DIS_SERIAL_NUMBER_STR,
			     sizeof(CONFIG_BT_DIS_SERIAL_NUMBER_STR));
#endif
#if defined(CONFIG_BT_DIS_SW_REV)
	settings_runtime_set("bt/dis/sw", CONFIG_BT_DIS_SW_REV_STR,
			     sizeof(CONFIG_BT_DIS_SW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_FW_REV)
	settings_runtime_set("bt/dis/fw", CONFIG_BT_DIS_FW_REV_STR,
			     sizeof(CONFIG_BT_DIS_FW_REV_STR));
#endif
#if defined(CONFIG_BT_DIS_HW_REV)
	settings_runtime_set("bt/dis/hw", CONFIG_BT_DIS_HW_REV_STR,
			     sizeof(CONFIG_BT_DIS_HW_REV_STR));
#endif
#endif
	return 0;
}

int main(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		settings_load();
	}

	settings_runtime_load();

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	// err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));

	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return 0;
	}

	printk("Advertising successfully started\n");
	while (1) {
		_BatteryLevel = _BatteryLevel + 3;
		BtSetBattery(_BatteryLevel);
		if (_BatteryLevel > 100) {
			_BatteryLevel = 0;
		}
	}
	return 0;
}

/*IMPLEMENTATION NUS SERVICE*/

static const struct bt_uuid_128 nus_svc_uuid =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x6E400001, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E));

static const struct bt_uuid_128 Nus_service_WriteChar =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x6E400002, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E));

static const struct bt_uuid_128 Nus_service_ReadChar =
	BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x6E400003, 0xB5A3, 0xF393, 0xE0A9, 0xE50E24DCCA9E));

static char test_char1_value[5];

// persistant characteristic data

static ssize_t write_nux_char(struct bt_conn *conn,
			      const struct bt_gatt_attr *attr, // write char value
			      const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
	for (int i = 0; i < len; i++) {
		printk("%x ", *(uint8_t *)&buf[i]);
	}

	printk("\n");
	return len;
}

static void NotifyEnabledCb(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);
	_IsTxEnabled = (value == BT_GATT_CCC_NOTIFY);
	printk("Battery level notification %s.", _IsTxEnabled ? "enabled" : "disabled");
}

static ssize_t read_nux_char(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr, // #read char value
			     void *buf, uint16_t len, uint16_t offset)
{

	return bt_gatt_attr_read(conn, attr, buf, len, offset, test_char1_value, 4);
}

BT_GATT_SERVICE_DEFINE(
	nus_svc, BT_GATT_PRIMARY_SERVICE(&nus_svc_uuid),
	BT_GATT_CHARACTERISTIC(&Nus_service_WriteChar.uuid, BT_GATT_CHRC_WRITE, BT_GATT_PERM_WRITE,
			       NULL, write_nux_char, NULL),
	BT_GATT_CHARACTERISTIC(&Nus_service_ReadChar.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, read_nux_char, NULL, test_char1_value),
	BT_GATT_CCC(NotifyEnabledCb, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), // FOR NOTIFICATION
);

int BtSetBattery(int level)
{
	if (_IsTxEnabled == true) {

		printk("Notifications  enabled.\n");

		snprintf(test_char1_value, sizeof(test_char1_value), "%d", level);

		// Notify the battery level characteristic
		int rc = bt_gatt_notify(NULL, &nus_svc.attrs[3], test_char1_value,
					strlen(test_char1_value));

		k_sleep(K_SECONDS(1));

		return rc == -ENOTCONN ? 0 : rc;

		return -EINVAL;
	}
}