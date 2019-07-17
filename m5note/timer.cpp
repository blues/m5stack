// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// This file contains general purpose time and timer-related methods, used because suppression timers
// are such a fundamental building block of asynchronous data sensing and communications devices.

#include <time.h>
#include "m5note.h"

// A simple suppression timer based on a millisecond system clock.  This clock is reset to 0
// after boot and every wake.  This returns true if the specified interval has elapsed, in milliseconds,
// and it updates the timer if it expires so that we will go another period.
bool timerExpiredMs(uint32_t *timer, uint32_t periodMs) {
    bool expired = false;
    
    // If the timer went backward, we've expired regardless of the interval
    uint32_t prev = *timer;
    uint32_t now = millis();
    if (now < prev)
        prev = 0;
    
    // If never initialized, it's expired
    if (prev == 0 || now >= prev+periodMs) {
        expired = true;
        *timer = now;
    }

    return expired;

}

// A simple suppression timer based on a millisecond system clock.  This clock is reset to 0
// after boot and every wake.  This returns true if the specified interval has elapsed, in seconds,
// and it updates the timer if it expires so that we will go another period.
bool timerExpiredSecs(uint32_t *timer, uint32_t periodSecs) {
	return timerExpiredMs(timer, periodSecs*1000);
}

// Convert the specified Unix epoch time to an ISO 8601 formatted string in UTC
bool timeString(uint32_t secs, char *buf, int bufLen) {
    time_t timeNowSecs = secs;
    struct tm *t = gmtime(&timeNowSecs);
    if (t == NULL) {
        *buf = '\0';
        return false;
    }
    int year = t->tm_year+1900;
    int mon1 = t->tm_mon+1;
    int day1 = t->tm_mday;
    int hour0 = t->tm_hour;
    int min0 = t->tm_min;
    int sec0 = t->tm_sec;
    snprintf(buf, bufLen-1, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, mon1, day1, hour0, min0, sec0);
    return true;
}

// Convert the specified Unix epoch time to a local string, in an abbreviated manner
bool timeStringLocal(epoch secs, int offsetMins, char *zone, char *buf, int bufLen) {
    time_t timeNowSecs = secs + (offsetMins*60);;
    struct tm *t = gmtime(&timeNowSecs);
    if (t == NULL) {
        *buf = '\0';
        return false;
    }
	const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    snprintf(buf, bufLen-1, "%s %02d:%02d %s", days[t->tm_wday], t->tm_hour, t->tm_min, zone);
    return true;
}
