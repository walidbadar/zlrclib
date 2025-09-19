# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
find_package(Python REQUIRED COMPONENTS Interpreter)

project(requests)

option(INCLUDE_HTML_CONTENT "Include the HTML content" ON)

target_sources(app PRIVATE src/main.c)
target_sources_ifdef(CONFIG_NET_SAMPLE_HTTP_SERVICE app PRIVATE src/http_server.c)
target_sources_ifdef(CONFIG_NET_SAMPLE_WEBSOCKET_SERVICE app PRIVATE src/ws.c)

target_link_libraries(app PRIVATE zephyr_interface zephyr)

zephyr_linker_sources(SECTIONS sections-rom.ld)
zephyr_linker_section_ifdef(CONFIG_NET_SAMPLE_HTTP_SERVICE NAME
				http_resource_desc_test_http_service
				KVMA RAM_REGION GROUP RODATA_REGION)
        
set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

# Generate the HTML content
foreach(web_resource
  index.html
  main.js
    )
  generate_inc_file_for_target(
    app
    ${CMAKE_SOURCE_DIR}/../web_resources/${web_resource}
    ${gen_dir}/${web_resource}.gz.inc
    --gzip
  )
endforeach()

# Generate the certificates
foreach(cert
	lrclib.der
    )
  generate_inc_file_for_target(
    app
    ${CMAKE_SOURCE_DIR}/../certs/${cert}
    ${gen_dir}/${cert}.inc
    )
endforeach()
