// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "m5note.h"
#include "src/ui/menu.h"

// The main menu MUST be the first entry, but the order of all others is arbitrary
// so long as these IDs exactly match the index of the entries in the table below.
#define MAIN_MENU       0
#define AUX_MENU		1

MENU menu[] = {
    { MAIN_MENU, MAIN_MENU, "Options",
      {
          {"This Here", NULL, showAbout, BUTTON_START},
          {"Is Really", NULL, showAbout, BUTTON_START},
          {"A Test >", NULL, menuActivate, AUX_MENU},
          {"Of The Menu", NULL, showAbout, BUTTON_START},
          {"About", NULL, showAbout, BUTTON_START},
          ENDMENU,
      }
    },
    { AUX_MENU, MAIN_MENU, "Test Menu",
      {
          {"Submenu One", NULL, showAbout, BUTTON_START},
          {"Sub Two", NULL, showAbout, BUTTON_START},
          {"Third", NULL, showAbout, BUTTON_START},
          ENDMENU,
      }
    },
    ENDMENUS
};
