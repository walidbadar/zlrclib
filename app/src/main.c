/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app/lib/wifi_conn_mgr.h>
#include <app/lib/requests.h>
#include <zephyr/net/http/server.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(requests_sample);

#define ARTIST	"benson+boone"
#define TRACK	"mystical+magical"

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
const char url[] = {
    "https://lrclib.net/api/get?artist_name=" ARTIST "&track_name=" TRACK
};
#else
const uint8_t url[] ={
	"http://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&hourly=temperature"
};
#endif

static int resp_cb(struct http_response *rsp, enum http_final_call final_data,
			   void *user_data)
{
	static uint32_t total_pkt_size;
	total_pkt_size += rsp->data_len;

	printk("%.*s", rsp->data_len, rsp->body_frag_start);

	if (final_data == HTTP_DATA_FINAL) {
		printk("\nTotal packet size: %d\n", total_pkt_size);
		total_pkt_size = 0;
	}

	return 0;
}

int main(void)
{
	LOG_INF("Starting request sample");

	http_server_start();

	int ret;
	struct requests_ctx ctx;

	while (1) {
		ret = requests_get(&ctx, url, resp_cb);
		if (ret < 0) {
			LOG_ERR("Requests GET failed: %d", ret);
		}
		k_msleep(60 * MSEC_PER_SEC);
	}

	return 0;
}
