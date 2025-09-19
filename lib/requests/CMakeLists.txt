#
# Copyright (c) 2025 Muhammad Waleed Badar
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_library_named(requests)

zephyr_library_sources(requests_connect.c)
zephyr_library_sources(requests_parser.c)
zephyr_library_sources(requests.c)
