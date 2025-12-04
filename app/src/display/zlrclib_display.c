/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib/zephyr_logo.h>
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

	LV_IMG_DECLARE(zephyr_logo);
	lv_obj_t *logo = lv_image_create(lv_scr_act());
	lv_obj_set_size(logo, DISPLAY_WIDTH, DISPLAY_HEIGHT);
	lv_obj_fade_out(logo, MSEC_PER_SEC, MSEC_PER_SEC);
	lv_image_set_src(logo, &zephyr_logo);

	uint32_t elapsed = 0;
	while (elapsed < MSEC_PER_SEC) {
		lv_timer_handler();
		k_msleep(10);
		elapsed += 10;
	}

	LOG_INF("ZLRCLIB APP Started");

	return 0;
}
