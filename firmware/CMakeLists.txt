# EITWatch firmware (ESP-IDF)
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS middleware drivers)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(eitwatch)
