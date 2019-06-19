// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Definitions for the menu control

#pragma once

#include "display.h"

// The selection array is statically allocated, so keep this as small as you can
// based on precisely the maximum number of entries in any menu or submenu.
#define MAX_SELECTIONS  8
#define MAX_LABEL       40

// Function to return flags for a given menu selection
#define MENU_FLAG_DISABLED      0x00000001
#define MENU_FLAG_SELECTED      0x00000002
typedef int (*menuFlagsFunc) (char *buf, int bufLen);

// Function to perform an action and return one of these 
enum menuActionIDs {
    MENU_ACTION_COMPLETED,
    MENU_ACTION_CANCEL,
    MENU_ACTION_CAPTURE,
    MENU_ACTION_DISMISS,
};
typedef int (*menuSelectFunc) (int menuSelectArg);

// Menu data structures
struct SELECTION {
    char label[MAX_LABEL];
    menuFlagsFunc menuFlagsFn;
    menuSelectFunc menuSelectFn;
    int menuSelectArg;
};
struct MENU {
    int id;
    int parentid;
    char label[MAX_LABEL];
    SELECTION selection[MAX_SELECTIONS];
} ;
#define ENDMENU {"", NULL, NULL, 0}
#define ENDMENUS {0, 0, "", {ENDMENU}}

// We will get polled when the menu is active with a BUTTON_RELEASED, so we can make the menu disappear
#define MENU_POLL_MS        1000
#define MENU_CANCEL_MS      30000

// Public
int menuActivate(int newmenu);
int menuBack(int newmenu);
void menuDeactivate(void);
void menuSet(MENU *pMenu, menuSelectFunc fnHome, menuSelectFunc fnPress);
void menuHomeChanged(void);
void menuButton(int buttonState);
unsigned menuPoll(void);
void menuActivity(void);
bool menuStart(menuSelectFunc fn);

