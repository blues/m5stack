// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#pragma once

#include "display.h"
#include "menu.h"

//
// ui.cpp
//

#define BUTTON_NONE             'o'
#define BUTTON_START            's'
#define BUTTON_REFRESH          'r'
#define BUTTON_RELEASED         'x'
#define BUTTON_PRESSED_R        'R'
#define BUTTON_PRESSED_L        'L'
#define BUTTON_PRESSED_U        'U'
#define BUTTON_PRESSED_D        'D'
#define BUTTON_PRESSED_S        'S'

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
void displayBottomLine(int font, const char *textL, const char *textM, const char *textR);

//
// edit-control.cpp
//

//
// edit-control.cpp
//

#define EDIT_CHARSET_ASCII      1
#define EDIT_CHARSET_URL        2
#define EDIT_CHARSET_PRODUCTID  3
#define EDIT_CHARSET_NUMERIC    4
void editControlBegin(char *edit, int charset);
int editControlButton(int buttonState, char *buf, int bufLen);
