// Copyright 2019 Blues Inc.  All rights reserved. 
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <Arduino.h>
#include <Notecard.h>
#include <M5Stack.h>
#include "src/ui/ui.h"
#include "src/actions/actions.h"

// Default website for registration of notehub user IDs
#define NOTEHUB_WEBSITE         "Notehub.IO"        // Set to "" if not using a Blues Wireless notehub

#define debugf Serial.printf

// config-menu.cpp

void menuInit(void);

// timer.cpp

bool timerExpiredMs(uint32_t *timer, uint32_t periodMs);
bool timerExpiredSecs(uint32_t *timer, uint32_t periodSecs);
bool timeString(uint32_t secs, char *buf, int bufLen);
