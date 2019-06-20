// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "display.h"
#include "menu.h"

//
// ui.cpp
//

enum buttonEventIDs {
    BUTTON_NONE,
    BUTTON_START,
    BUTTON_REFRESH,
    BUTTON_RELEASED,
    BUTTON_PRESSED_R,
    BUTTON_PRESSED_L,
    BUTTON_PRESSED_S,
};

void uiInit(void);
bool uiIsActive(void);
void uiPoll(void);
void uiClear(void);
void uiTask(void *pvParameters);
void uiRefreshStatus(bool);
void uiFlushInput();
void uiSetAutoRepeat(bool enabled);

//
// display-util.cpp
//

void displayCenteredBegin(int font);
void displayCentered(const char *text);
void displayCenteredEnd(void);
void displayCenteredBottomEnd(void);
void displayCenteredLine(int font, const char *text);
void displayBottomLine(int font, const char *text);
