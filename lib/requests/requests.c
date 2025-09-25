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

static int requests_init(struct requests_ctx *ctx, const uint8_t *url)
{
	int ret;

	memset(ctx, 0, sizeof(struct requests_ctx));

	ret = requests_url_parser(ctx, url);
	if (ret < 0) {
		LOG_ERR("Failed to parse URL: %d", ret);
		return ret;
	}

	ret = requests_dns_lookup(ctx);
	if (ret < 0) {
		LOG_ERR("DNS Failed to resolve: %d", ret);
		return ret;
	}

	return ret;
}

int requests_get(struct requests_ctx *ctx, http_response_cb_t cb, const uint8_t *url)
{
	int ret;

	if (!ctx && !url) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	ret = requests_init(ctx, url);
	if (ret < 0) {
		LOG_ERR("Requests initialization failed: %d", ret);
		return ret;
	}

	ret = requests_connect(ctx);
	if (ret >= 0) {
		ctx->req.method = HTTP_GET;
		ctx->req.url = ctx->url_fields.uri;
		ctx->req.host = ctx->url_fields.hostname;
		ctx->req.protocol = "HTTP/1.1";
		ctx->req.response = (http_response_cb_t)cb;
		ctx->req.recv_buf = ctx->recv_buf;
		ctx->req.recv_buf_len = sizeof(ctx->recv_buf);

		ret = http_client_req(ctx->sockfd, &ctx->req, CONFIG_NET_SOCKETS_DNS_TIMEOUT, ctx);
		close(ctx->sockfd);
	} else {
		LOG_ERR("Cannot connect to remote (%d)", -errno);
		ret = -ECONNABORTED;
	}

	return ret;
}

int requests_post(struct requests_ctx *ctx, http_response_cb_t cb, const uint8_t *url,
		  const uint8_t *payload, http_payload_cb_t payload_cb)
{
	int ret;

	if (!ctx || !url) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	ret = requests_init(ctx, url);
	if (ret < 0) {
		LOG_ERR("Requests initialization failed: %d", ret);
		return ret;
	}

	const char *headers[] = {
		"Transfer-Encoding: chunked\r\n",
		NULL
	};

	ret = requests_connect(ctx);
	if (ret >= 0) {
		ctx->req.method = HTTP_POST;
		ctx->req.url = ctx->url_fields.uri;
		ctx->req.host = ctx->url_fields.hostname;
		ctx->req.protocol = "HTTP/1.1";
		ctx->req.response = cb;
		ctx->req.recv_buf = ctx->recv_buf;
		ctx->req.recv_buf_len = sizeof(ctx->recv_buf);
		ctx->req.header_fields = headers;
		ctx->req.payload = payload;
		ctx->req.payload_len = strlen(ctx->req.payload);
		ctx->req.payload_cb = payload_cb;

		ret = http_client_req(ctx->sockfd, &ctx->req, CONFIG_NET_SOCKETS_DNS_TIMEOUT, ctx);
		close(ctx->sockfd);
	} else {
		LOG_ERR("Cannot connect to remote (%d)", -errno);
		ret = -ECONNABORTED;
	}

	return ret;
}
