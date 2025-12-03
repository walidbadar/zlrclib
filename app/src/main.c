/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib.h>
#include <zlrclib_display.h>
#include <zlrclib/net/lib/wifi_conn_mgr.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib);

K_WORK_DEFINE(zlrclib_work, zlrclib_display_lyrics);

int main(void)
{
	int ret = 0;

#if defined(CONFIG_WIFI)
	struct net_if *iface = net_if_get_wifi_sta();
	if (iface == NULL) {
		LOG_ERR("Wifi interface not found");
		return -ENODEV;
	}

	ret = wifi_connect(iface, CONFIG_WIFI_STATIC_SSID, CONFIG_WIFI_STATIC_PSK);
	if (ret < 0) {
		LOG_WRN("WiFi connect failed: %d", ret);
	}
#endif

	ret = zlrclib_display_init();
	if(ret < 0) {
		return ret;
	}

	k_work_submit(&zlrclib_work);
	
	while(1) {
		lv_timer_handler();
		k_sleep(K_MSEC(10));
	}

	return 0;
}
