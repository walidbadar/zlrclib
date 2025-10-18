/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <app/zlrclib.h>
#include <app/zlrclib_applications.h>
#include <app/zlrclib_display.h>
#include <app/zlrclib_filesystem.h>
#include <app/lib/requests.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib);

#define ARTIST "Seafret"
#define TRACK  "Atlantis"

#if defined(CONFIG_NET_SOCKETS_SOCKOPT_TLS)
const uint8_t *url = "https://lrclib.net/api/get?artist_name=" ARTIST "&track_name=" TRACK;
#else
const uint8_t *url = "https://google.com";
#endif

#define LYRICS "syncedLyrics"
#define LYRICS_OFFSET 15
#define VERSE_OFFSET 10

#define LYRICS_POS_START "["
#define LYRICS_POS_END "\\n"

static bool is_synced_lyrics = false;

static int resp_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
	struct requests_ctx *ctx = (struct requests_ctx *)user_data;
	char *pos = NULL;

	if (!is_synced_lyrics) {
		pos = strstr(rsp->body_frag_start, LYRICS);
	}

	if (pos) {
		rsp->body_frag_start = pos + LYRICS_OFFSET;
		rsp->body_frag_len = strlen(pos) - LYRICS_OFFSET; 
		is_synced_lyrics = true;
	}

	if (is_synced_lyrics) {
		zlrclib_fwrite(LYRICS, rsp->body_frag_start, rsp->body_frag_len,
			       ctx->recv_buf_len);
		ctx->recv_buf_len += rsp->body_frag_len;
	}

	memset(rsp->recv_buf, 0, rsp->recv_buf_len);

	return 0;
}

int main(void)
{
	int ret;
	LOG_INF("ZLRCLIB APP");

	zlrclib_rm(LYRICS);

	struct requests_ctx ctx;
	ret = requests_get(&ctx, resp_cb, url);
	if (ret < 0) {
		LOG_ERR("Requests GET failed: %d", ret);
	}

	uint16_t len = 0;
	uint16_t pos = 0;
	uint8_t *pos_start;
	uint8_t *pos_end;

	ret = zlrclib_fread(LYRICS, ctx.recv_buf, CONFIG_NET_IPV4_MTU, 0);
	if (ret < 0) {
		return ret;
	}

	while(1) {		
		pos_start = strstr(ctx.recv_buf, LYRICS_POS_START);
		pos_end = strstr(ctx.recv_buf, LYRICS_POS_END);

		if(pos_start && pos_end){			
			len = strlen(pos_start) - strlen(pos_end) + 2;
		}

		ret = zlrclib_fread(LYRICS, ctx.recv_buf, len - VERSE_OFFSET, pos + VERSE_OFFSET);
		if (ret < 0) {
			break;
		}

		zlrclib_display_lyrics(ctx.recv_buf);
		pos += len;
		
		LOG_INF("%s", ctx.recv_buf);
		LOG_INF("Read postion: %d", pos);

		ret = zlrclib_fread(LYRICS, ctx.recv_buf, CONFIG_NET_IPV4_MTU, pos);
		if (ret < 0) {
			break;
		}

		k_msleep(3 * MSEC_PER_SEC);
	}

	return 0;
}
