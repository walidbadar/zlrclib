/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_INCLUDE_REQUESTS_H_
#define APP_INCLUDE_REQUESTS_H_

#include <stdbool.h>
#include <zephyr/net/conn_mgr_monitor.h>
#include <zephyr/net/dns_resolve.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/client.h>

#define NI_MAXPORT 6

typedef int socket_t;

struct requests_url_fields {
	uint8_t hostname[NI_MAXHOST];
	uint8_t port[NI_MAXPORT];
	uint8_t uri[256];
	bool is_ssl;
};

struct requests_ctx {
	socket_t sockfd;
	struct addrinfo *ai;
	struct requests_url_fields url_fields;
	struct http_request req;
	struct http_response resp;
	uint8_t recv_buf[NET_IPV4_MTU];
	uint16_t status;
};

int requests_get(struct requests_ctx *ctx, http_response_cb_t cb, const uint8_t *url);

#endif /* APP_INCLUDE_REQUESTS_H_ */
