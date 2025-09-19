/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include <app/lib/requests.h>
#include "requests_private.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(requests, CONFIG_REQUESTS_LOG_LEVEL);

int requests_get(struct requests_ctx *ctx, http_response_cb_t cb, const uint8_t *url)
{
	int ret;

	if (!ctx && !url) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	memset(ctx, 0, sizeof(struct requests_ctx));

	ret = requests_url_parser(ctx, url);
	if (ret < 0) {
		LOG_ERR("Failed to parse URL: %d", ret);
		return ret;
	}

	ret = requests_dns_lookup(ctx);
	if (ret < 0) {
		return ret;
	}

	ret = requests_connect(ctx);
	if (ret >= 0) {
		ctx->req.method = HTTP_GET;
		ctx->req.url = ctx->url_fields.uri;
		ctx->req.host = ctx->url_fields.hostname;
		ctx->req.protocol = "HTTP/1.1";
		ctx->req.response = cb;
		ctx->req.recv_buf = ctx->recv_buf;
		ctx->req.recv_buf_len = sizeof(ctx->recv_buf);

		ret = http_client_req(ctx->sockfd, &ctx->req, CONFIG_NET_SOCKETS_DNS_TIMEOUT, ctx);
		close(ctx->sockfd);
		freeaddrinfo(ctx->ai);
	} else {
		LOG_ERR("Cannot connect to remote (%d)", -errno);
		ret = -ECONNABORTED;
	}

	return ret;
}
