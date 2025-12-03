/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zlrclib_display.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_display_mgr);

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

static int current_song = -1;
static int song_indices[8];

/* Event handler for song selection */
static void song_list_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_CLICKED) {
        int *idx_ptr = (int *)lv_event_get_user_data(e);
        int idx = *idx_ptr;
        current_song = idx;
      
        LOG_INF("Selected: %s - %s", songs[idx].title, songs[idx].artist);
    }
}

void zlrclib_display_mgr(void)
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    
    /* Song list container */
    lv_obj_t *list_container = lv_obj_create(screen);
    lv_obj_set_size(list_container, LV_PCT(100), LV_PCT(100));
    lv_obj_align(list_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(list_container, lv_color_white(), 0);
    lv_obj_set_style_pad_all(list_container, 5, 0);
    lv_obj_set_style_border_width(list_container, 0, 0);
    lv_obj_set_style_pad_row(list_container, 2, 0);
    lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollbar_mode(list_container, LV_SCROLLBAR_MODE_AUTO);
    
    /* Create song list items */
    for(int i = 0; i < sizeof(songs) / sizeof(songs[0]); i++) {
        song_indices[i] = i;
        
        lv_obj_t *btn = lv_btn_create(list_container);
        lv_obj_set_size(btn, LV_PCT(100), LV_PCT(30));
        lv_obj_set_style_radius(btn, 0, 0);
        lv_obj_set_style_bg_color(btn, lv_color_white(), 0);
        lv_obj_set_style_bg_color(btn, lv_color_make(230, 230, 230), LV_STATE_PRESSED);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, lv_color_black(), 0);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_event_cb(btn, song_list_event_handler, LV_EVENT_CLICKED, &song_indices[i]);
        
        /* Song info as single line */
        lv_obj_t *label = lv_label_create(btn);
        char text[64];
        snprintf(text, sizeof(text), "%s - %s", songs[i].title, songs[i].artist);
        lv_obj_center(label);
        lv_obj_set_width(label, DISPLAY_WIDTH);
        lv_obj_set_style_text_color(label, lv_color_black(), 0);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_text(label, text);
    }
}
