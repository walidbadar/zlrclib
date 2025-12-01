/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZLRCLIB_H_
#define ZLRCLIB_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

struct zlrclib_track_info {
	uint32_t id;
	uint8_t name[32];
	uint8_t track_name[32];
	uint8_t artist_name[32];
	uint8_t album_name[32];
	bool instrumental;
};

#endif /* ZLRCLIB_H_ */
