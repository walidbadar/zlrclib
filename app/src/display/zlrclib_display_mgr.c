/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_display_mgr);

/* Menu structure */
typedef enum {
	MENU_HOME,
	MENU_TRACKS,
	MENU_SYSTEM,
	MENU_COUNT
} menu_item_t;

static const char *menu_items[] = {"HOME", "Tracks", "System"};

/* Screen objects */
static lv_obj_t *menu_screen;
static lv_obj_t *home_screen;
static lv_obj_t *system_screen;

/* Global variables */
static int current_menu = 0;
static lv_obj_t *menu_list;
static lv_group_t *input_group;

/* Get the LVGL keypad input device */
static const struct device *lvgl_keypad =
	DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_keypad_input));

static void zlrclib_display_menu_item(lv_obj_t *label, uint8_t *item)
{
	lv_label_set_text(label, item);
	lv_obj_center(label);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(label, lv_color_black(), 0);
}

static void zlrclib_display_menu_home(void)
{
	home_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(home_screen, lv_color_white(), 0);

	lv_obj_t *label = lv_label_create(home_screen);
	zlrclib_display_menu_item(label, "HOME SCREEN\n\nPress ENTER to return");
}

static void zlrclib_display_menu_system(void)
{
	system_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(system_screen, lv_color_white(), 0);

	lv_obj_t *label = lv_label_create(system_screen);
	zlrclib_display_menu_item(label, "SYSTEM SETTINGS\n\nPress ENTER to return");
}

static void zlrclib_display_menu_load_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t key = lv_event_get_key(e);

	if (code == LV_EVENT_KEY && key == LV_KEY_ENTER) {
		LOG_INF("Returning to menu");
		lv_scr_load(menu_screen);
	}
}

static void zlrclib_display_menu_load(lv_obj_t *screen)
{
	if (screen) {
		/* Add event handler to screen for navigation back */
		lv_group_add_obj(input_group, screen);
		lv_obj_add_event_cb(screen, zlrclib_display_menu_load_handler, LV_EVENT_KEY, NULL);

		/* Load the screen with animation */
		lv_scr_load_anim(screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
	}
}

static void zlrclib_display_menu_update(void)
{
	uint32_t i;
	for (i = 0; i < MENU_COUNT; i++) {
		lv_obj_t *btn = lv_obj_get_child(menu_list, i);
		lv_obj_t *label = lv_obj_get_child(btn, 0);

		if (i == current_menu) {
			/* Selected item: black background, white text */
			lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
			lv_obj_set_style_text_color(label, lv_color_white(), 0);
		} else {
			/* Unselected item: white background, black text */
			lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
			lv_obj_set_style_text_color(label, lv_color_black(), 0);
		}
	}
}

static void zlrclib_display_menu_select(menu_item_t item)
{
	switch (item) {
	case MENU_HOME:
		LOG_INF("Navigating to HOME");
		if (!home_screen) {
			zlrclib_display_menu_home();
		}
		zlrclib_display_menu_load(home_screen);
		break;

	case MENU_TRACKS:
		LOG_INF("Navigating to Display Tracks");
		zlrclib_display_tracks();
		break;

	case MENU_SYSTEM:
		LOG_INF("Navigating to System");
		if (!system_screen) {
			zlrclib_display_menu_system();
		}
		zlrclib_display_menu_load(system_screen);
		break;

	default:
		break;
	}
}

static void zlrclib_display_menu_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t key = lv_event_get_key(e);

	if (code == LV_EVENT_KEY) {
		switch (key) {
		case LV_KEY_UP:
			LOG_INF("UP pressed");
			if (current_menu > 0) {
				current_menu--;
				zlrclib_display_menu_update();
			}
			break;

		case LV_KEY_DOWN:
			LOG_INF("DOWN pressed");
			if (current_menu < MENU_COUNT - 1) {
				current_menu++;
				zlrclib_display_menu_update();
			}
			break;

		case LV_KEY_ENTER:
			LOG_INF("ENTER pressed - Selected: %s", menu_items[current_menu]);
			zlrclib_display_menu_select(current_menu);
			break;

		case LV_KEY_BACKSPACE:
			LOG_INF("Backspace pressed");
			break;
		}
	}
}

void zlrclib_display_mgr_init(void)
{
	/* Create menu screen */
	menu_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(menu_screen, lv_color_white(), 0);

	/* Create a container for the menu */
	menu_list = lv_obj_create(menu_screen);
	lv_obj_set_size(menu_list, LV_PCT(100), LV_PCT(100));
	lv_obj_align(menu_list, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_set_style_bg_color(menu_list, lv_color_white(), 0);
	lv_obj_set_style_pad_all(menu_list, 5, 0);
	lv_obj_set_style_border_width(menu_list, 0, 0);
	lv_obj_set_style_pad_row(menu_list, 2, 0);
	lv_obj_set_flex_flow(menu_list, LV_FLEX_FLOW_COLUMN);

	/* Disable scrolling on the container */
	lv_obj_clear_flag(menu_list, LV_OBJ_FLAG_SCROLLABLE);

	/* Create menu items */
	for (int i = 0; i < MENU_COUNT; i++) {
		lv_obj_t *btn = lv_obj_create(menu_list);
		lv_obj_set_size(btn, LV_PCT(100), 18);
		lv_obj_set_style_border_width(btn, 1, 0);
		lv_obj_set_style_border_color(btn, lv_color_black(), 0);
		lv_obj_set_style_radius(btn, 0, 0);
		lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

		lv_obj_t *label = lv_label_create(btn);
		lv_obj_center(label);
		lv_obj_fade_in(label, 500, 0);
		lv_label_set_text(label, menu_items[i]);
	}

	/* Create input group and add the menu container */
	input_group = lv_group_create();
	lv_group_add_obj(input_group, menu_list);
	lv_indev_set_group(lvgl_input_get_indev(lvgl_keypad), input_group);

	/* Add event handler to the menu container */
	lv_obj_add_event_cb(menu_list, zlrclib_display_menu_handler, LV_EVENT_KEY, NULL);

	/* Set initial selection */
	zlrclib_display_menu_update();

	/* Load the menu screen */
	lv_scr_load(menu_screen);
}
