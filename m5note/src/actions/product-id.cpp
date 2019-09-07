// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Display the sequence of dialogs for setting the Product ID

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the Product ID settings screen
int actionSetProductID(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_RELEASED:
        return MENU_ACTION_CAPTURE;
    case BUTTON_PRESSED_S:
    case BUTTON_PRESSED_R:
        menuStart(actionProductID);
        return MENU_ACTION_CAPTURE;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Display static text
    displayClear();
    displayCenteredBegin(FONT_TINY);
    if (NOTEHUB_WEBSITE[0] != '\0') {
        displayCentered("Please enter a Product ID from");
        displayCentered("your " NOTEHUB_WEBSITE " Project's Settings." );
        displayCentered("");
        displayCentered("continue >");
    } else {
        displayCentered("Please enter a Product ID from");
        displayCentered("your Notehub Project's Settings." );
        displayCentered("");
        displayCentered("continue >");
    }
    displayCenteredEnd();

    // Capture button input
    return MENU_ACTION_CAPTURE;

}

// Set the product ID
int actionProductID(int buttonState) {
    static char pidbuf[64];

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    default:
        int action = editControlButton(buttonState, pidbuf, sizeof(pidbuf));
        if (action == MENU_ACTION_COMPLETED) {

            // Set the product ID in the notecard
            if (J *req = NoteNewRequest("service.set")) {
                JAddStringToObject(req, "product", pidbuf);
                NoteRequest(req);
            }

        } else if (action == MENU_ACTION_CANCEL) {
            action = MENU_ACTION_DISMISS;
        }
        return (action);
    }

    // Bring up the password view control
    pidbuf[0] = '\0';
    if (J *rsp = NoteRequestResponse(NoteNewRequest("service.get"))) {
        strlcpy(pidbuf, JGetString(rsp, "product"), sizeof(pidbuf));
        NoteDeleteResponse(rsp);
    }
    editControlBegin(pidbuf, EDIT_CHARSET_PRODUCTID);

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
