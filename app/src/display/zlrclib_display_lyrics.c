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
#define LYRICS_END " \"}"

#define LYRICS "syncedLyrics"
#define LYRICS_OFFSET 15
#define VERSE_OFFSET 10
#define VERSE_DELAY 5

#define ARTIST "David+Kushner"
#define TRACK  "Mr+Forgettable"
const uint8_t *url = "https://lrclib.net/api/get?artist_name=" ARTIST "&track_name=" TRACK;

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

void zlrclib_display_lyrics_work(struct k_work *item)
{
	int ret;
	zlrclib_rm(LYRICS);

	static lv_obj_t *label;

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
			len = strlen(pos_start) - strlen(pos_end);
		}

		ret = zlrclib_fread(LYRICS, ctx.recv_buf, len - VERSE_OFFSET, pos + VERSE_OFFSET);
		if (ret < 0) {
			break;
		}

		LOG_INF("%s", ctx.recv_buf);

		ret = memcmp(ctx.recv_buf, LYRICS_END, sizeof(LYRICS_END));
		if(!ret) {
			break;
		}

		if(label == NULL) {
			label = lv_label_create(lv_scr_act());
			lv_obj_center(label);
			lv_obj_set_width(label, DISPLAY_WIDTH);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
			lv_label_set_text(label, "");
		}

		lv_obj_fade_out(label, 100, 0);
		lv_obj_fade_in(label, 100, 100);

		lv_label_set_text(label, ctx.recv_buf);

		pos += len + 2;
		LOG_INF("Read postion: %d", pos);

		ret = zlrclib_fread(LYRICS, ctx.recv_buf, CONFIG_NET_IPV4_MTU, pos);
		if (ret < 0) {
			break;
		}

		k_msleep(VERSE_DELAY * MSEC_PER_SEC);
	}
}
