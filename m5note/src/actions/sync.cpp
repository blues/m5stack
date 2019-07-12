// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// GPS menu actions & methods

#include "../../m5note.h"
#include "../ui/ui.h"

int actionSetSyncPeriod(int buttonState) {
    static char nbuf[64];

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    default:
        int action = editControlButton(buttonState, nbuf, sizeof(nbuf));
        if (action == MENU_ACTION_COMPLETED) {
            J *req = NoteNewRequest("service.set");
            if (req != NULL) {
                JAddNumberToObject(req, "minutes", atoi(nbuf));
                NoteRequest(req);
            }
        }
        return (action);
    }

    // Bring up the edit control
    nbuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("service.get"));
    if (rsp != NULL) {
        snprintf(nbuf, sizeof(nbuf), "%d", JGetInt(rsp, "minutes"));
        NoteDeleteResponse(rsp);
    }
    editControlBegin(nbuf, EDIT_CHARSET_NUMERIC);

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
