#
# Copyright (c) 2025 Muhammad Waleed Badar
#
# SPDX-License-Identifier: Apache-2.0
#

target_sources(app PRIVATE zlrclib_http_server.c)

zephyr_linker_sources(SECTIONS sections-rom.ld)

zephyr_linker_section_ifdef(CONFIG_HTTP_SERVER NAME
  http_resource_desc_test_http_service
  KVMA RAM_REGION GROUP RODATA_REGION
)

foreach(web_resource
  index.html
  main.js
)
  generate_inc_file_for_target(
    app
    ${CMAKE_SOURCE_DIR}/web_resources/${web_resource}
    ${ZEPHYR_BINARY_DIR}/include/generated/${web_resource}.gz.inc
    --gzip
  )
endforeach()
