/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zlrclib_display.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zlrclib_display_mgr);

/* Song data structure */
typedef struct {
    const char *title;
    const char *artist;
} song_t;

/* Sample songs */
static const song_t songs[] = {
    {"Bohemian Rhapsody", "Queen"},
    {"Imagine", "John Lennon"},
    {"Hotel California", "Eagles"},
    {"Stairway to Heaven", "Led Zeppelin"},
    {"Sweet Child O' Mine", "Guns N' Roses"},
    {"Billie Jean", "Michael Jackson"},
    {"Smells Like Teen Spirit", "Nirvana"},
    {"Yesterday", "The Beatles"}
};

#define SONG_COUNT (sizeof(songs) / sizeof(songs[0]))

/* Global variables */
static lv_obj_t *song_list_container;
static lv_obj_t *song_screen;
static int current_song_sel = 0;
static lv_group_t *song_group;

/* Get the LVGL keypad input device */
static const struct device *lvgl_keypad =
    DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_keypad_input));

/* Update visual selection */
static void update_song_selection(void)
{
    for (int i = 0; i < SONG_COUNT; i++) {
        lv_obj_t *btn = lv_obj_get_child(song_list_container, i);
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        
        if (i == current_song_sel) {
            /* Selected: black background, white text */
            lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
            lv_obj_set_style_text_color(label, lv_color_white(), 0);
        } else {
            /* Unselected: white background, black text */
            lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
            lv_obj_set_style_text_color(label, lv_color_black(), 0);
        }
    }
}

/* Event handler for keypad navigation */
static void song_keypad_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    uint32_t key = lv_event_get_key(e);
    
    if (code == LV_EVENT_KEY) {
        switch (key) {
            case LV_KEY_UP:
                LOG_INF("UP pressed");
                if (current_song_sel > 0) {
                    current_song_sel--;
                    update_song_selection();
                }
                break;
                
            case LV_KEY_DOWN:
                LOG_INF("DOWN pressed");
                if (current_song_sel < SONG_COUNT - 1) {
                    current_song_sel++;
                    update_song_selection();
                }
                break;
                
            case LV_KEY_ENTER:
                LOG_INF("Selected: %s - %s", 
                        songs[current_song_sel].title, 
                        songs[current_song_sel].artist);
                /* Add your song playback logic here */
                break;
        }
    }
}

void zlrclib_display_songs(void)
{
    /* Create song screen */
    song_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(song_screen, lv_color_white(), 0);
    
    /* Song list container */
    song_list_container = lv_obj_create(song_screen);
    lv_obj_set_size(song_list_container, LV_PCT(100), LV_PCT(100));
    lv_obj_align(song_list_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(song_list_container, lv_color_white(), 0);
    lv_obj_set_style_pad_all(song_list_container, 5, 0);
    lv_obj_set_style_border_width(song_list_container, 0, 0);
    lv_obj_set_style_pad_row(song_list_container, 2, 0);
    lv_obj_set_flex_flow(song_list_container, LV_FLEX_FLOW_COLUMN);
    
    /* Disable scrolling - we'll use keypad navigation instead */
    lv_obj_clear_flag(song_list_container, LV_OBJ_FLAG_SCROLLABLE);
    
    /* Create song list items */
    for (int i = 0; i < SONG_COUNT; i++) {
        lv_obj_t *btn = lv_obj_create(song_list_container);
        lv_obj_set_size(btn, LV_PCT(100), LV_PCT(35));
        lv_obj_set_style_radius(btn, 0, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, lv_color_black(), 0);
        
        /* Disable scrolling on buttons */
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        
        /* Song info as single line */
        lv_obj_t *label = lv_label_create(btn);
        char text[64];
        snprintf(text, sizeof(text), "%s - %s", songs[i].title, songs[i].artist);
        lv_label_set_text(label, text);
        lv_obj_center(label);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    }
    
    /* Create input group and add the container */
    song_group = lv_group_create();
    lv_group_add_obj(song_group, song_list_container);
    lv_indev_set_group(lvgl_input_get_indev(lvgl_keypad), song_group);
    
    /* Add event handler for keypad navigation */
    lv_obj_add_event_cb(song_list_container, song_keypad_event_handler, LV_EVENT_KEY, NULL);
    
    /* Set initial selection */
    current_song_sel = 0;
    update_song_selection();
    
    /* Load the song screen */
    lv_scr_load(song_screen);
}
