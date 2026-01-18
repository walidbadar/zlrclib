# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(Python REQUIRED COMPONENTS Interpreter)

project(zlrclib)

include_directories(src/applications)
include_directories(src/display)
include_directories(src/filesystem)
include_directories(src/http_server)
include_directories(src/wifi)

add_subdirectory(src/applications)
add_subdirectory_ifdef(CONFIG_DISPLAY src/display)
add_subdirectory_ifdef(CONFIG_FILE_SYSTEM src/filesystem)
add_subdirectory_ifdef(CONFIG_HTTP_SERVER src/http_server)
add_subdirectory_ifdef(CONFIG_WIFI_CONNECTION_MANAGER src/wifi)

target_sources(app PRIVATE src/main.c)
