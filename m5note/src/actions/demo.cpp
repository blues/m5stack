// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Fill this in with your own code

#include "../../m5note.h"

int actionDemo(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_REFRESH:
        return MENU_ACTION_CAPTURE;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Bring up the edit control
    J *rsp = NoteRequestResponse(NoteNewRequest("card.version"));
    displayCenteredLine(FONT_TINY, JGetString(rsp, "name"));
    NoteDeleteResponse(rsp);

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
