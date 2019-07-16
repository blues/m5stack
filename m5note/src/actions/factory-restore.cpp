// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "../../m5note.h"
#include "../ui/ui.h"

// Do a factory restore
int actionFactoryRestore(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_PRESSED_U:
    case BUTTON_PRESSED_D:
    case BUTTON_PRESSED_S:
    case BUTTON_RELEASED:
        return MENU_ACTION_CAPTURE;
    case BUTTON_PRESSED_R:
        if (!NoteFactoryReset(true))
            break;
        return MENU_ACTION_DISMISS;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Prompt for validation
    displayClear();
	displayCenteredBegin(FONT_SMALL);
	char version[64];
    if (!NoteGetVersion(version, sizeof(version)))
		displayCentered("(sensor is busy)");
	else {
        displayCentered("Are you sure?");
        displayCentered("(press RIGHT to proceed)");
    }
	displayCenteredEnd();

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
