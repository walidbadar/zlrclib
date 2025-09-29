#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <app/zlrclib.h>
#include <zephyr/data/json.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zlrclib_parser);

static const struct json_obj_descr track_descr[] = {
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, id, JSON_TOK_NUMBER),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, name, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, trackName, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, artistName, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, albumName, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, instrumental, JSON_TOK_TRUE),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, plainLyrics, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM(struct zlrclib_track, syncedLyrics, JSON_TOK_STRING),
};

int zlrclib_track_parser(struct zlrclib_track *track, uint8_t *buf, size_t len)
{
	int ret, ret_json;

	if (!track && !buf) {
		LOG_ERR("Invalid arguments");
		return -EINVAL;
	}

	ret = json_obj_parse(buf, len, track_descr, ARRAY_SIZE(track_descr), track);
	ret_json = (1 << ARRAY_SIZE(track_descr)) - 1;

	if (ret < 0) {
		LOG_ERR("JSON Parse Error: %d", ret);
		return ret;
	} else if (ret != ret_json) {
		LOG_ERR("Not all values decoded; Expected return code %d but got %d", ret_json,
			ret);
		return ret;
<<<<<<< HEAD
	} else {
		track->id;
		track->name;
		track->trackName;
		track->artistName;
		track->albumName;
		track->instrumental;
		track->plainLyrics;
		track->syncedLyrics;
=======
>>>>>>> b138496 (Added zlrclib track parser)
	}

	return ret;
}
