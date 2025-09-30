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
	const char *name;
	const char *track_name;
	const char *artist_name;
	const char *album_name;
	bool instrumental;
	const char *plain_lyrics;
	const char *synced_lyrics;
};

#endif /* ZLRCLIB_H_ */
