# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(Python REQUIRED COMPONENTS Interpreter)

project(zlrclib)

add_subdirectory(src/display)
add_subdirectory(src/filesystem)
add_subdirectory(src/http_server)

set(APP_SOURCES 
  src/main.c
  src/zlrclib_parser.c
  src/zlrclib_tracks.c
)

target_sources(app PRIVATE ${APP_SOURCES})
target_link_libraries(app PRIVATE zephyr_interface zephyr)

if(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
  foreach(cert
    lrclib.der
  )
    generate_inc_file_for_target(
      app
      ${CMAKE_SOURCE_DIR}/certs/${cert}
      ${ZEPHYR_BINARY_DIR}/include/generated/${cert}.inc
    )
  endforeach()
endif()
