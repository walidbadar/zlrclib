/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/drivers/display.h>

#include <zlrclib_display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_display);

static const struct device *ddev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display)); 

void zlrclib_display_lyrics(uint8_t *synced_lyrics)
{
	static lv_obj_t *lyrics;

    if(lyrics == NULL) {
        lyrics = lv_label_create(lv_scr_act());
        lv_obj_align(lyrics, LV_ALIGN_TOP_LEFT, 0, 24);
		lv_obj_set_width(lyrics, 128);
		lv_label_set_long_mode(lyrics, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }

    lv_label_set_text(lyrics, synced_lyrics);
}

static int zlrclib_display(void)
{
	if (!device_is_ready(ddev)) {
		LOG_ERR("Display device not ready");
		return -ENODEV;
	}

	display_blanking_off(ddev);

	lv_obj_t *display_title = lv_label_create(lv_scr_act());
	lv_obj_align(display_title, LV_ALIGN_TOP_MID, 0, 0);
	lv_label_set_text(display_title, "ZLRCLIB APP");
	lv_task_handler();

	return 0;
}

SYS_INIT(zlrclib_display, APPLICATION, 95);
