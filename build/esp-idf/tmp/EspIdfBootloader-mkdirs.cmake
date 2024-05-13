# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/sjv/zephyrproject/modules/hal/espressif/components/bootloader/subproject"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/build/bootloader"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/tmp"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/src/EspIdfBootloader-stamp"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/src"
  "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/src/EspIdfBootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/src/EspIdfBootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/sjv/zephyrproject/zephyr/samples/bluetooth/peripheral_dis/build/esp-idf/src/EspIdfBootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
