/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZLRCLIB_DISPLAY_H_
#define ZLRCLIB_DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <lvgl.h>

void zlrclib_display_lyrics(uint8_t *synced_lyrics);

#endif /* ZLRCLIB_DISPLAY_H_ */
