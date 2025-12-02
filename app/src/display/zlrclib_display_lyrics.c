/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>
#include <zlrclib_filesystem.h>
#include <zlrclib/net/lib/requests.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_display_lyrics);

#define LYRICS_POS_START "["
#define LYRICS_POS_END "\\n"

#define LYRICS "syncedLyrics"
#define LYRICS_OFFSET 15
#define VERSE_OFFSET 10

#define ARTIST "Seafret"
#define TRACK  "Atlantis"

const uint8_t *url = "https://lrclib.net/api/get?artist_name=" ARTIST "&track_name=" TRACK;

static lv_obj_t *lyrics;

static int zlrclib_display_lyrics_cb(struct http_response *rsp, enum http_final_call final_data, void *user_data)
{
	static bool is_synced_lyrics = false;

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

void zlrclib_display_lyrics(struct k_work *item)
{
	int ret;
	zlrclib_rm(LYRICS);

	struct requests_ctx ctx;
	while (requests_get(&ctx, zlrclib_display_lyrics_cb, url) < 0) {
		LOG_ERR("Requests GET failed");
	}

	uint16_t len = 0;
	uint16_t pos = 0;
	uint8_t *pos_start;
	uint8_t *pos_end;

	zlrclib_fread(LYRICS, ctx.recv_buf, CONFIG_NET_IPV4_MTU, 0);

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

		LOG_INF("%s", ctx.recv_buf);

		if(lyrics == NULL) {
			lyrics = lv_label_create(lv_scr_act());
			lv_obj_align(lyrics, LV_ALIGN_TOP_LEFT, 0, 24);
			lv_obj_set_width(lyrics, 128);
			lv_label_set_text(lyrics, "");
		}

		lv_obj_fade_out(lyrics, 100, 0);
		lv_obj_fade_in(lyrics, 100, 100);

		lv_label_set_long_mode(lyrics, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_label_set_text(lyrics, ctx.recv_buf);

		pos += len;
		LOG_INF("Read postion: %d", pos);

		ret = zlrclib_fread(LYRICS, ctx.recv_buf, CONFIG_NET_IPV4_MTU, pos);
		if (ret < 0) {
			break;
		}

		k_msleep(5 * MSEC_PER_SEC);
	}
}
