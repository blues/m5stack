// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Test methods

#include "../../m5note.h"
#include "../ui/ui.h"

// Return an appropriate menu flag as to whether or not we're in "continuously-connected" mode
int flagTestIsContinuous(char *buf, int buflen) {
    J *rsp = NoteRequestResponse(NoteNewRequest("service.get"));
    if (rsp != NULL) {
        if (strcmp(JGetString(rsp, "mode"), "continuous") == 0)
            return MENU_FLAG_SELECTED;
    }
    return 0;
}

// Toggle the mode between periodic and continuous
int actionTestToggleContinuous(int buttonState) {
    bool isContinuous = flagTestIsContinuous(NULL, 0) == MENU_FLAG_SELECTED;
    J *req = NoteNewRequest("service.set");
    JAddStringToObject(req, "mode", isContinuous ? "periodic" : "continuous");
    NoteRequest(req);
    return MENU_ACTION_DISMISS;
}

// Add a random test note to a test file, simulating a temperature from -5.00 to 85.00 degrees
int actionTestAdd(int buttonState) {
    J *body = JCreateObject();
    if (body != NULL) {
        JAddNumberToObject(body, "temperature", (double) random(-500, 8500) / 100);
        NoteSend("test.qo", body, false);
    }
    return MENU_ACTION_DISMISS;
}

// Force a synchronization manually, as opposed to waiting for the notecard to sync periodically on its own
int actionTestSync(int buttonState) {
    NoteRequest(NoteNewRequest("service.sync"));
    return MENU_ACTION_DISMISS;
}
