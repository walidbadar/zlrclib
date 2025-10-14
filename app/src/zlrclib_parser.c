#include <zephyr/data/json.h>
#include <app/zlrclib.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_parser);

static const struct json_obj_descr track_descr[] = {
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "id", id, JSON_TOK_NUMBER),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "name", name, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "trackName", track_name, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "artistName", artist_name, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "albumName", album_name, JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "instrumental", instrumental,
				  JSON_TOK_TRUE),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "plainLyrics", plain_lyrics,
				  JSON_TOK_STRING),
	JSON_OBJ_DESCR_PRIM_NAMED(struct zlrclib_track, "syncedLyrics", synced_lyrics,
				  JSON_TOK_STRING),
};

int zlrclib_track_parser(struct zlrclib_track *track, uint8_t *buf, size_t len)
{
	int ret, ret_json;

	if (!track && !buf) {
		LOG_ERR("Invalid arguments");
		return -EINVAL;
	}

	memset(track, 0, sizeof(struct zlrclib_track));

	ret = json_obj_parse(buf, len, track_descr, ARRAY_SIZE(track_descr), track);
	ret_json = (1 << ARRAY_SIZE(track_descr)) - 1;

	if (ret < 0) {
		LOG_ERR("JSON Parse Error: %d", ret);
		return ret;
	}

	if (ret != ret_json) {
		LOG_ERR("Not all values decoded; Expected return code %d but got %d", ret_json,
			ret);
		return -EINVAL;
	}

	return ret;
}
