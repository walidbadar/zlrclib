/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app/zlrclib.h>
#include <app/zlrclib_filesystem.h>
#include <app/lib/wifi_conn_mgr.h>
#include <app/lib/requests.h>
#include <zephyr/net/http/server.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib);

#define ARTIST "Taylor+Swift"
#define TRACK  "New+Romantics"

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
const uint8_t *url = "https://lrclib.net/api/get?artist_name=" ARTIST "&track_name=" TRACK;
#else
const uint8_t *url = "https://google.com";
#endif

static int resp_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
	struct requests_ctx *ctx = (struct requests_ctx *)user_data;

	zlrclib_fwrite("track", rsp->body_frag_start, rsp->body_frag_len, ctx->recv_buf_len);
	ctx->recv_buf_len += rsp->body_frag_len;

	return 0;
}

int main(void)
{
	int ret;
	LOG_INF("Starting zlrclib app");

	if (IS_ENABLED(CONFIG_HTTP_SERVER)) {
		http_server_start();
	}

	struct requests_ctx ctx;

	while (1) {
		ret = requests_get(&ctx, resp_cb, url);
		if (ret < 0) {
			LOG_ERR("Requests GET failed: %d", ret);
		}

		for (int i = 0; i <= ctx.recv_buf_len / CONFIG_NET_IPV4_MTU; i++) {
			zlrclib_fread("track", ctx.recv_buf, CONFIG_NET_IPV4_MTU,
				      i * CONFIG_NET_IPV4_MTU);
			printk("%.*s\n", CONFIG_NET_IPV4_MTU, ctx.recv_buf);
		}

		zlrclib_rm("track");

		k_msleep(60 * MSEC_PER_SEC);
	}

	return 0;
}
