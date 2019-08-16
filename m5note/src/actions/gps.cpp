// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// GPS menu actions & methods

#include "../../m5note.h"
#include "../ui/ui.h"

// Get a pointer to current cached location settings
const char *settingsLocation() {
    const char *loc = LOCATION_UNKNOWN;
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location.mode"));
    if (rsp != NULL) {
        loc = JGetString(rsp, "mode");
        if (strcmp(loc, "periodic") == 0) {
            if (JGetBool(rsp, "start"))
                loc = LOCATION_TRACK;
            else
                loc = LOCATION_PERIODIC;
        }
        NoteDeleteResponse(rsp);
    }
    return loc;
}

// Set Location Settings
bool setLocationSettings(const char *settings) {
    J *req = NoteNewRequest("card.location.mode");
    if (req == NULL)
        return false;
    if (strcmp(settings, LOCATION_TRACK) == 0) {
        JAddBoolToObject(req, "start", true);
        settings = LOCATION_PERIODIC;
    } else {
        JAddBoolToObject(req, "stop", true);
    }
    JAddStringToObject(req, "mode", settings);
    return NoteRequest(req);
}

// Return an appropriate menu flag
int flagIsLocIndoor(char *buf, int buflen) {
    if (strcmp(settingsLocation(), LOCATION_INDOOR) == 0)
        return MENU_FLAG_SELECTED;
    return 0;
}

// Set the status appropriately
int actionSetLocIndoor(int arg) {
    setLocationSettings(LOCATION_INDOOR);
    return MENU_ACTION_COMPLETED;
}

// Return an appropriate menu flag
int flagIsLocMobile(char *buf, int buflen) {
    if (strcmp(settingsLocation(), LOCATION_MOBILE) == 0)
        return MENU_FLAG_SELECTED;
    return 0;
}

// Set the status appropriately
int actionSetLocMobile(int arg) {
    setLocationSettings(LOCATION_MOBILE);
    return MENU_ACTION_COMPLETED;
}

// Return an appropriate menu flag
int flagIsLocOutdoor(char *buf, int buflen) {
    if (strcmp(settingsLocation(), LOCATION_OUTDOOR) == 0)
        return MENU_FLAG_SELECTED;
    return 0;
}

// Set the status appropriately
int actionSetLocOutdoor(int arg) {
    setLocationSettings(LOCATION_OUTDOOR);
    return MENU_ACTION_COMPLETED;
}

// Return an appropriate menu flag
int flagIsLocTrack(char *buf, int buflen) {
    if (strcmp(settingsLocation(), LOCATION_TRACK) == 0)
        return MENU_FLAG_SELECTED;
    return 0;
}

// Set the status appropriately
int actionSetLocTrack(int arg) {
    setLocationSettings(LOCATION_TRACK);
    return MENU_ACTION_COMPLETED;
}

// Return an appropriate menu flag
int flagIsLocPeriodic(char *buf, int buflen) {
    if (strcmp(settingsLocation(), LOCATION_PERIODIC) == 0)
        return MENU_FLAG_SELECTED;
    return 0;
}

// Set the status appropriately
int actionSetLocPeriodic(int arg) {
    setLocationSettings(LOCATION_PERIODIC);
    return MENU_ACTION_COMPLETED;
}

int actionSetLocPeriod(int buttonState) {
    static char nbuf[64];

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    default:
        int action = editControlButton(buttonState, nbuf, sizeof(nbuf));
        if (action == MENU_ACTION_COMPLETED) {
            J *req = NoteNewRequest("card.location.mode");
            if (req != NULL) {
                JAddNumberToObject(req, "minutes", atoi(nbuf));
                NoteRequest(req);
            }
        }
        return (action);
    }

    // Bring up the edit control
    nbuf[0] = '\0';
    J *rsp = NoteRequestResponse(NoteNewRequest("card.location.mode"));
    if (rsp != NULL) {
        snprintf(nbuf, sizeof(nbuf), "%d", JGetInt(rsp, "minutes"));
        NoteDeleteResponse(rsp);
    }
    editControlBegin(nbuf, EDIT_CHARSET_NUMERIC);

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
