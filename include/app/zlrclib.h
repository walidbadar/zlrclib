/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZLRCLIB_H_
#define ZLRCLIB_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern uint8_t track1[];
extern uint8_t track2[];

struct zlrclib_track {
	uint32_t id;
	const uint8_t *name;
	const uint8_t *track_name;
	const uint8_t *artist_name;
	const uint8_t *album_name;
	bool instrumental;
	const uint8_t *plain_lyrics;
	const uint8_t *synced_lyrics;
};

void zlrclib_track_info(const struct zlrclib_track *track);
int zlrclib_track_parser(struct zlrclib_track *track, uint8_t *buf, size_t len);

#endif /* ZLRCLIB_H_ */
