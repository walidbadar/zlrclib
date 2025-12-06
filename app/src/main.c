/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>
#include <zlrclib/net/lib/wifi_conn_mgr.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib);

#define ZLRCLIB_DISPLAY_STACK_SIZE 4096
#define ZLRCLIB_DISPLAY_PRIORITY 6

static void zlrclib_display_thread(void)
{
	int ret;

	ret = zlrclib_display_init();
	if (ret < 0) {
		return;
	}

	zlrclib_display_mgr_init();

	while (1) {
		lv_task_handler();
		k_sleep(K_MSEC(10));
	}
}

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

	return ret;
}

K_THREAD_DEFINE(zlrclib_display_tid, ZLRCLIB_DISPLAY_STACK_SIZE, zlrclib_display_thread, NULL, NULL, NULL, ZLRCLIB_DISPLAY_PRIORITY, 0, 200);
