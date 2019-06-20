// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// The product's home screen

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the home screen
int showHome(int buttonState) {

	// Exit if the UI is not yet active
	if (!uiIsActive()) {
	    displayClear(false);
		displayCenteredBegin(FONT_SMALL);
		displayCentered("one moment, please...");
		displayCenteredEnd();
		return MENU_ACTION_COMPLETED;
	}

    // Display the home screen
    displayClear(false);
	displayCenteredBegin(FONT_SMALL);
	displayCentered("home");
	displayCenteredEnd();

	// Done
    return MENU_ACTION_COMPLETED;
}
