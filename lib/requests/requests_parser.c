/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/net/http/parser_url.h>
#include <app/lib/requests.h>
#include "requests_private.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(requests_parser, CONFIG_REQUESTS_LOG_LEVEL);

/* TODO: Replace with http_parser_url.c*/
static int requests_url_fields_get(const uint8_t *url, const struct http_parser_url *purl,
				   enum http_parser_url_fields url_field, uint8_t *field,
				   uint16_t field_len)
{
	if (!url || !purl || !field) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	uint16_t off = purl->field_data[url_field].off;
	uint16_t len = purl->field_data[url_field].len;

	if (len == 0 || len >= field_len) {
		return -EINVAL;
	}

	if (url_field == UF_PATH) {
		memcpy(field, url + off, strlen(url));
		field[strlen(url)] = '\0';
	} else {
		memcpy(field, url + off, len);
		field[len] = '\0';
	}

	return 0;
}

int requests_url_parser(struct requests_ctx *ctx, const uint8_t *url)
{
	int ret;
	struct http_parser_url purl;
	uint8_t *hostname = ctx->url_fields.hostname;
	uint8_t *port = ctx->url_fields.port;
	uint8_t *uri = ctx->url_fields.uri;

	http_parser_url_init(&purl);

	ret = http_parser_parse_url(url, strlen(url), 0, &purl);
	if (ret < 0) {
		LOG_ERR("Error parsing URL: %d\n", ret);
		return ret;
	}

	ret = requests_url_fields_get(url, &purl, UF_HOST, hostname,
				      sizeof(ctx->url_fields.hostname));
	if (ret < 0) {
		LOG_ERR("Error parsing hostname: %d\n", ret);
		return ret;
	}

	ret = requests_url_fields_get(url, &purl, UF_PORT, port, sizeof(ctx->url_fields.port));
	if (ret < 0) {
		if (IS_ENABLED(CONFIG_NET_SOCKETS_SOCKOPT_TLS)) {
			memcpy(port, CONFIG_REQUESTS_HTTPS_PORT,
			       sizeof(CONFIG_REQUESTS_HTTPS_PORT));
		} else {
			memcpy(port, CONFIG_REQUESTS_HTTP_PORT, sizeof(CONFIG_REQUESTS_HTTP_PORT));
		}
	}

	ret = requests_url_fields_get(url, &purl, UF_PATH, uri, sizeof(ctx->url_fields.uri));
	if (ret < 0) {
		LOG_ERR("Error parsing URI: %d\n", ret);
		return ret;
	}

	LOG_INF("Hostname: %s, Port: %s, URI: %s", hostname, port, uri);

	return ret;
}
