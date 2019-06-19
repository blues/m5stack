// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "m5note.h"
#include "src/ui/menu.h"

// The main menu MUST be the first entry, but the order of all others is arbitrary
// so long as these IDs exactly match the index of the entries in the table below.
#define MAIN_MENU       0
#define SETUP_MENU      1
#define NOTECARD_MENU   2
#define GPS_MENU        3
#define HIDDEN_MENU     4

static MENU menu[] = {
    { MAIN_MENU, MAIN_MENU, "Options",
      {
          {"Add a Note", NULL, actionTestAdd, BUTTON_START},
          {"Sync With Service Now", NULL, actionTestSync, BUTTON_START},
          {"Stay Connected", flagTestIsContinuous, actionTestToggleContinuous, BUTTON_START},
          {"Setup >", NULL, menuActivate, SETUP_MENU},
          {"Demo", NULL, actionDemo, BUTTON_START},
          {"< Back", NULL, menuBack, 0},
          ENDMENU,
      }
    },
    { SETUP_MENU, MAIN_MENU, "Setup",
      {
          {"Notecard >", NULL, menuActivate, NOTECARD_MENU},
          {"Notehub >", NULL, actionSetProductID, BUTTON_START},
          {"GPS Mode >", NULL, menuActivate, GPS_MENU},
          {"< Back", NULL, menuBack, 0},
          ENDMENU,
      }
    },
    { NOTECARD_MENU, SETUP_MENU, "Notecard",
      {
          {"About", NULL, actionAboutNotecard, BUTTON_START},
          {"Sync Period (mins) >", 0, actionSetSyncPeriod, BUTTON_START},
          {"Environment", NULL, actionEnv, BUTTON_START},
          {"Factory Restore", NULL, actionFactoryRestore, BUTTON_START},
          {"< Back", NULL, menuBack, 0},
          ENDMENU
      }
    },
    { GPS_MENU, SETUP_MENU, "GPS Mode",
      {
          {"Indoor (off)", flagIsLocIndoor, actionSetLocIndoor, BUTTON_START},
          {"Mobile (on)", flagIsLocMobile, actionSetLocMobile, BUTTON_START},
          {"Outdoor (when moved)", flagIsLocOutdoor, actionSetLocOutdoor, BUTTON_START},
          {"Periodic (timed)", flagIsLocPeriodic, actionSetLocPeriodic, BUTTON_START},
          {"Tracker (timed/logged)", flagIsLocTrack, actionSetLocTrack, BUTTON_START},
          {"GPS Period (mins) >", 0, actionSetLocPeriod, BUTTON_START},
          {"< Back", NULL, menuBack, 0},
          ENDMENU
      }
    },
    { HIDDEN_MENU, HIDDEN_MENU, "-",
      {
          {"Product ID", NULL, actionProductID, BUTTON_START},
          ENDMENU,
      }
    },
    ENDMENUS
};

// Process a menu button when a menu is not set
static int menuPress(int buttonState) {
        switch (buttonState) {
        case BUTTON_PRESSED_R:
            displayClear();
            menuActivate(MAIN_MENU);
            return 0;
        case BUTTON_PRESSED_U:
        case BUTTON_PRESSED_D:
        case BUTTON_PRESSED_S:
            break;
        case BUTTON_PRESSED_L:
            menuDeactivate();
            break;
        }
    return 0;
}

// Initialize the menu subsystem
void menuInit(menuSelectFunc home) {
    menuSet(menu, home, menuPress);
}
