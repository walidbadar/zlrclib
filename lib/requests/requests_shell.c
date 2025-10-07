/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <app/lib/requests.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(requests_shell, CONFIG_REQUESTS_LOG_LEVEL);

static struct requests_ctx req_ctx;

static int http_response_handler(struct http_response *rsp, enum http_final_call final_data,
				 void *user_data)
{
	if (rsp->body_frag_len) {
		printk("%.*s", rsp->body_frag_len, rsp->body_frag_start);
	}

	return 0;
}

static int cmd_requests_get(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	if (argc < 2) {
		shell_error(sh, "Usage: requests get <url>");
		return -EINVAL;
	}

	ret = requests_get(&req_ctx, http_response_handler, argv[1]);
	if (ret < 0) {
		shell_error(sh, "GET request failed (%d)", ret);
		return ret;
	}

	return ret;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_requests,
			       SHELL_CMD_ARG(get, NULL,
					     "Perform HTTP GET request: requests get <url>",
					     cmd_requests_get, 2, 0),
			       SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(requests, &sub_requests, "HTTP requests commands", NULL);
