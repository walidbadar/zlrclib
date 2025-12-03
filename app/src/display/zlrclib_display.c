/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_display);

int zlrclib_display_init(void)
{
	const struct device *ddev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(ddev)) {
		LOG_ERR("Display device not ready");
		return -ENODEV;
	}

	display_blanking_off(ddev);

	lv_obj_t *display_title = lv_label_create(lv_scr_act());
	lv_obj_align(display_title, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_width(display_title, DISPLAY_WIDTH);
	lv_obj_set_style_text_align(display_title, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text(display_title, "ZLRCLIB APP: Powered by Zephyr RTOS");
	lv_obj_fade_out(display_title, 500, 3 * MSEC_PER_SEC);

	LOG_INF("ZLRCLIB APP Started");

	return 0;
}
