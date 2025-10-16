/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZLRCLIB_APPLICATIONS_H_
#define ZLRCLIB_APPLICATIONS_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

int zlrclib_rtc_set_time(struct rtc_time *tm, bool boot);
int zlrclib_rtc_get_time(uint8_t *buf, size_t len);

#endif /* ZLRCLIB_APPLICATIONS_H_ */
