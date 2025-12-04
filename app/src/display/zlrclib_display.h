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
#include <lvgl_input_device.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)
#define DISPLAY_WIDTH DT_PROP(DISPLAY_NODE, width)
#define DISPLAY_HEIGHT DT_PROP(DISPLAY_NODE, height)

int zlrclib_display_init(void);
void zlrclib_display_mgr_init(void);
void zlrclib_display_tracks(void);
void zlrclib_display_lyrics_work(struct k_work *item);

#endif /* ZLRCLIB_DISPLAY_H_ */
