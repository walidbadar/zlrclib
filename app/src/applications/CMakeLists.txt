#
# Copyright (c) 2025 Muhammad Waleed Badar
#
# SPDX-License-Identifier: Apache-2.0
#

target_sources_ifdef(CONFIG_RTC app PRIVATE zlrclib_rtc.c)
