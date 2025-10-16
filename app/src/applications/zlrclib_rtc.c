/*
 * Copyright (c) 2025 Muhammad Waleed Badar
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/util.h>
#include <time.h>

#include <app/zlrclib_applications.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(zlrclib_rtc, LOG_LEVEL_DBG);

#define RTC_BUF_LEN 32

static const struct device *rtc;

static uint8_t *zlrclib_format_time(const struct rtc_time *rtc_tm)
{
    static uint8_t buffer[RTC_BUF_LEN];
    uint8_t *buf = buffer;
	uint8_t *end = buf + RTC_BUF_LEN;

	struct tm tm = {0};
	tm.tm_sec = rtc_tm->tm_sec;
	tm.tm_min = rtc_tm->tm_min;
	tm.tm_hour = rtc_tm->tm_hour;
	tm.tm_mday = rtc_tm->tm_mday;
	tm.tm_mon = rtc_tm->tm_mon;
	tm.tm_year = rtc_tm->tm_year;
	tm.tm_wday = rtc_tm->tm_wday;
	tm.tm_yday = rtc_tm->tm_yday;
	tm.tm_isdst = rtc_tm->tm_isdst;

	size_t used = strftime((char *)buf, end - buf, "%Y-%m-%d %H:%M:%S", &tm);
	buf += used;

	if (rtc_tm->tm_nsec > 0 && buf < end) {
		int written = snprintf((char *)buf, end - buf, ".%09d", rtc_tm->tm_nsec);
		buf += (written > 0) ? written : 0;
	}

	if (tm.tm_wday >= 0 && tm.tm_yday >= 0 && buf < end) {
		used = strftime((char *)buf, end - buf, " %a %j", &tm);
		buf += used;
	} else if (tm.tm_wday >= 0 && buf < end) {
		used = strftime((char *)buf, end - buf, " %a", &tm);
		buf += used;
	}

    return buffer;
}

int zlrclib_rtc_set_time(struct rtc_time *tm, bool boot)
{
	int ret;

    if(boot){
        tm->tm_year = 2025 - 1900;
        tm->tm_mon = 9 - 1;
        tm->tm_mday = 21;
        tm->tm_hour = 4;
        tm->tm_min = 19;
        tm->tm_sec = 0;
    }

	ret = rtc_set_time(rtc, tm);
	if (ret < 0) {
		LOG_ERR("Failed to set date time (%d)", ret);
		return ret;
	}
	return ret;
}

int zlrclib_rtc_get_time(uint8_t *buf, size_t len)
{
	int ret;
	struct rtc_time tm;

	ret = rtc_get_time(rtc, &tm);
	if (ret < 0) {
		LOG_ERR("Failed to read date time (%d)", ret);
		return ret;
	}

	uint8_t *buffer = zlrclib_format_time(&tm);
    memcpy(buf, buffer, len);

	LOG_INF("RTC date and time: %s", buffer);

	return ret;
}

static int zlrclib_rtc(void)
{
    rtc = DEVICE_DT_GET(DT_ALIAS(rtc));
	if (!device_is_ready(rtc)) {
		LOG_ERR("RTC device is not ready");
		return -ENODEV;
	}

    struct rtc_time tm;
	zlrclib_rtc_set_time(&tm, true);

	return 0;
}

SYS_INIT(zlrclib_rtc, APPLICATION, 95);
