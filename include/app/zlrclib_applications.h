#ifndef ZLRCLIB_APPLICATIONS_H_
#define ZLRCLIB_APPLICATIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

int zlrclib_rtc_get_time(uint8_t *buf, size_t len);

#endif /* ZLRCLIB_APPLICATIONS_H_ */
