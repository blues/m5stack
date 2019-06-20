// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Display the About dialog, along with the QR code.  Note that the QR code URL may be formed by using
// an administrative environment variable, so that when the Airnote data is ultimately routed to a Route
// on the Notehub, this QR code can be updated to point to that same website where the data can be displayed.

// Status and Settings-related Menus

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the About screen
int showAbout(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_RELEASED:
        return MENU_ACTION_CAPTURE;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Display the about screen
    displayClear(false);
	displayCenteredBegin(FONT_TINY);
	displayCentered("about");
	displayCenteredEnd();

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
