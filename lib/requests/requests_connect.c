/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app/lib/requests.h>
#include "requests_private.h"
#include "requests_certs.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(requests_connect, CONFIG_REQUESTS_LOG_LEVEL);

#define NET_EVENT_L4_MASK (NET_EVENT_L4_CONNECTED | NET_EVENT_L4_DISCONNECTED)

static struct net_mgmt_event_callback l4_cb;
static K_SEM_DEFINE(network_connected, 0, 1);

static void net_event_l4_handler(struct net_mgmt_event_callback *cb, uint64_t status,
				 struct net_if *iface)
{
	switch (status) {
	case NET_EVENT_L4_CONNECTED:
		LOG_INF("Network connection established (IPv4)");
		k_sem_give(&network_connected);
		break;
	case NET_EVENT_L4_DISCONNECTED:
		break;
	default:
		break;
	}
}

int requests_dns_lookup(struct requests_ctx *ctx)
{
	int ret;
	struct addrinfo hints;

	if (!ctx) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	if (!strlen(ctx->url_fields.hostname) || !strlen(ctx->url_fields.port)) {
		LOG_ERR("Invalid hostname or port");
		return -EINVAL;
	}

	net_mgmt_init_event_callback(&l4_cb, net_event_l4_handler, NET_EVENT_L4_MASK);
	net_mgmt_add_event_callback(&l4_cb);
	conn_mgr_mon_resend_status();

	LOG_INF("Waiting for IPv4 address assignment (DHCP/Static)");

	k_sem_take(&network_connected, K_FOREVER);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* TODO: Replace with dns_get_addr_info*/
	ret = getaddrinfo(ctx->url_fields.hostname, ctx->url_fields.port, &hints, &ctx->ai);
	if (ret < 0) {
		LOG_ERR("DNS lookup failed: %d", ret);
		return ret;
	}

	// freeaddrinfo(ctx->ai);
	return ret;
}

static int requests_connect_setup(struct requests_ctx *ctx)
{
	int ret = 0;

	if (!ctx) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_NET_SOCKETS_SOCKOPT_TLS)) {
		sec_tag_t sec_tag_list[] = {
			CA_CERTIFICATE_TAG,
		};

		ctx->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TLS_1_2);
		if (ctx->sockfd >= 0) {
			ret = setsockopt(ctx->sockfd, SOL_TLS, TLS_SEC_TAG_LIST, sec_tag_list,
					 sizeof(sec_tag_list));
			if (ret < 0) {
				LOG_ERR("Failed to set secure option (%d)", -errno);
			}

			ret = setsockopt(ctx->sockfd, SOL_TLS, TLS_HOSTNAME,
					 ctx->url_fields.hostname,
					 sizeof(ctx->url_fields.hostname));
			if (ret < 0) {
				LOG_ERR("Failed to set TLS_HOSTNAME: %s option (%d)",
					ctx->url_fields.hostname, -errno);
			}
		}
	} else {
		ctx->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	if (ctx->sockfd < 0) {
		LOG_ERR("Failed to create socket (%d)", -errno);
		ret = -errno;
	}

	return ret;
}

int requests_connect(struct requests_ctx *ctx)
{
	int ret;

	if (!ctx) {
		LOG_ERR("Invalid argument");
		return -EINVAL;
	}

	ret = requests_connect_setup(ctx);
	if (ret < 0) {
		LOG_ERR("Cannot create socket (%d)", -errno);
		return -ECONNABORTED;
	}

	ret = connect(ctx->sockfd, ctx->ai->ai_addr, ctx->ai->ai_addrlen);
	if (ret < 0) {
		LOG_ERR("Cannot connect to remote (%d)", -errno);
		close(ctx->sockfd);
		freeaddrinfo(ctx->ai);
		ctx->sockfd = -1;
		ret = -ECONNABORTED;
	}

	return ret;
}

static int requests_certs(void)
{
	int ret;

	if (IS_ENABLED(CONFIG_NET_SOCKETS_SOCKOPT_TLS)) {
		ret = tls_credential_add(CA_CERTIFICATE_TAG, TLS_CREDENTIAL_CA_CERTIFICATE,
					 ca_certificate, sizeof(ca_certificate));
		if (ret < 0) {
			LOG_ERR("Failed to register public certificate: %d", ret);
		}
	}

	return ret;
}

SYS_INIT(requests_certs, APPLICATION, 95);
