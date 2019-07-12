// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Display the sequence of dialogs for setting the Product ID

#include "../../m5note.h"
#include "../ui/ui.h"

int actionSetSerialNumber(int buttonState) {
    static char snbuf[64];

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    default:
        int action = editControlButton(buttonState, snbuf, sizeof(snbuf));
        if (action == MENU_ACTION_COMPLETED)
            NoteSetSerialNumber(snbuf);
        return (action);
    }

    // Bring up the edit control
    if (!NoteGetServiceConfigST(NULL, 0, NULL, 0, NULL, 0, snbuf, sizeof(snbuf)))
        snbuf[0] = '\0';
    editControlBegin(snbuf, EDIT_CHARSET_ASCII);

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
