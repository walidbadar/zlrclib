/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zlrclib_display_mgr);

static struct zlrclib_track_info tracks[] = {{.id = 1,
					      .track_name = "Mr Forgettable",
					      .artist_name = "David Kushner",
					      .album_name = "Footprints",
					      .instrumental = false},
					     {.id = 2,
					      .track_name = "Atlantic",
					      .artist_name = "Seafret",
					      .album_name = "Most of Us Are Strangers",
					      .instrumental = false},
					     {.id = 3,
					      .track_name = "Wi$h Li$t",
					      .artist_name = "Taylor Swift",
					      .album_name = "The Fate of Ophelia",
					      .instrumental = false}};

/* Global variables */
static int current_track = 0;
static lv_obj_t *track_list_container;

/* Work item for displaying lyrics */
K_WORK_DEFINE(zlrclib_work, zlrclib_display_lyrics_work);

/* Get the LVGL keypad input device */
static const struct device *lvgl_keypad =
	DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_keypad_input));

static void zlrclib_display_tracks_update(void)
{
	for (int i = 0; i < ARRAY_SIZE(tracks); i++) {
		lv_obj_t *btn = lv_obj_get_child(track_list_container, i);

		if (i == current_track) {
			/* Selected: black background, white text */
			lv_obj_set_style_border_color(btn, lv_color_black(), 0);
		} else {
			/* Unselected: white background, black text */
			lv_obj_set_style_border_color(btn, lv_color_white(), 0);
		}
	}
}

static void zlrclib_display_tracks_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t key = lv_event_get_key(e);

	if (code == LV_EVENT_KEY) {
		switch (key) {
		case LV_KEY_UP:
			LOG_INF("UP pressed");
			if (current_track > 0) {
				current_track--;
				zlrclib_display_tracks_update();
			}
			break;

		case LV_KEY_DOWN:
			LOG_INF("DOWN pressed");
			if (current_track < ARRAY_SIZE(tracks) - 1) {
				current_track++;
				zlrclib_display_tracks_update();
			}
			break;

		case LV_KEY_ENTER:
			LOG_INF("Selected: %s - %s", tracks[current_track].artist_name,
				tracks[current_track].track_name);
			lv_obj_fade_out(track_list_container, MSEC_PER_SEC, 0);
			k_work_submit(&zlrclib_work);
			break;

		case LV_KEY_BACKSPACE:
			LOG_INF("BACKSPACE pressed");
			zlrclib_display_mgr_init();
			break;
		}
	}
}

void zlrclib_display_tracks(void)
{
	/* Create track screen */
	lv_obj_t *track_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(track_screen, lv_color_white(), 0);

	/* Song list container */
	track_list_container = lv_obj_create(track_screen);
	lv_obj_set_size(track_list_container, LV_PCT(100), LV_PCT(100));
	lv_obj_align(track_list_container, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_style_bg_color(track_list_container, lv_color_white(), 0);
	lv_obj_set_style_pad_all(track_list_container, 5, 0);
	lv_obj_set_style_pad_row(track_list_container, 2, 0);
    lv_obj_set_style_border_width(track_list_container, 0, 0);
	lv_obj_set_flex_flow(track_list_container, LV_FLEX_FLOW_COLUMN);

	/* Disable scrolling - we'll use keypad navigation instead */
	lv_obj_clear_flag(track_list_container, LV_OBJ_FLAG_SCROLLABLE);

	/* Create track list items */
	for (int i = 0; i < ARRAY_SIZE(tracks); i++) {
		lv_obj_t *btn = lv_obj_create(track_list_container);
		lv_obj_set_size(btn, LV_PCT(100), 18);
		lv_obj_set_style_radius(btn, 0, 0);
		lv_obj_set_style_border_width(btn, 1, 0);
		lv_obj_set_style_border_color(btn, lv_color_black(), 0);

		/* Disable scrolling on buttons */
		lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

		/* Song info as single line */
		lv_obj_t *label = lv_label_create(btn);
		char text[64];
		snprintf(text, sizeof(text), "%s - %s", tracks[i].artist_name,
			 tracks[i].track_name);
		lv_obj_center(label);
		lv_obj_set_width(label, DISPLAY_WIDTH - 20);
		lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
		lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_label_set_text(label, text);
	}

	/* Create input group and add the container */
	lv_group_t *track_group = lv_group_create();
	lv_group_add_obj(track_group, track_list_container);
	lv_indev_set_group(lvgl_input_get_indev(lvgl_keypad), track_group);

	/* Add event handler for keypad navigation */
	lv_obj_add_event_cb(track_list_container, zlrclib_display_tracks_event_handler,
			    LV_EVENT_KEY, NULL);

	/* Set initial selection */
	current_track = 0;
	zlrclib_display_tracks_update();

	/* Load the track screen */
	lv_scr_load(track_screen);
}
