// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "m5note.h"

// Show the home screen.
// This method is called initially with BUTTON_START, and then is polled quickly with BUTTON_REFRESH.
int homeScreen(int buttonState) {
    static bool autoRefresh = true;
    static uint32_t refreshTimer = 0;

    // Determine whether or not we should display the home screen, or refresh
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_REFRESH:
        if (!autoRefresh || !timerExpiredSecs(&refreshTimer, 1))
            return MENU_ACTION_CAPTURE;
        refreshTimer = millis();
        break;
    }

    // Exit if the UI is not yet active
    static bool firstActive = true;
    if (!uiIsActive()) {
        displayClear();
        displayCenteredBegin(FONT_SMALL);
        displayCentered("one moment, please...");
        displayCenteredEnd();
        return MENU_ACTION_COMPLETED;
    }
    if (firstActive) {
        firstActive = false;
        displayClear();
    }

    // Display the status screen as a series of centered lines
    displayCenteredBegin(FONT_TINY);

    // Status of pending outbound changes
    J *rsp = NoteRequestResponse(NoteNewRequest("files.stats"));
    if (rsp == NULL) {
        displayCentered("");
    } else {
        int changes = JGetNumber(rsp, "changes");
        if (changes == 0) {
            displayCentered("");
        } else if (changes == 1) {
            displayCentered("1 change pending");
        } else {
            char message[64];
            snprintf(message, sizeof(message), "%d changes pending", changes);
            displayCentered(message);
        }
        NoteDeleteResponse(rsp);
    }

    // Status of our connection to the service, displayed on two lines
    char status[128];
    status[0] = '\0';
    rsp = NoteRequestResponse(NoteNewRequest("service.status"));
    if (rsp != NULL) {
        strlcpy(status, JGetString(rsp, "status"), sizeof(status));
        NoteDeleteResponse(rsp);
    }
    autoRefresh = strstr(status, "{idle}") == NULL;
    NoteErrorClean(status);
    int maxLine1Len = 34;
    int statusLen = strlen(status);
    if (statusLen <= maxLine1Len) {
        displayCentered(status);
        displayCentered("");
    } else {
        statusLen = maxLine1Len;
        while (statusLen > 0 && status[statusLen] != ' ')
            statusLen--;
        status[statusLen] = '\0';
        displayCentered(status);
        displayCentered(&status[statusLen+1]);
    }

    // Status of service synchronization, displayed on two lines
    status[0] = '\0';
    rsp = NoteRequestResponse(NoteNewRequest("service.sync.status"));
    if (rsp != NULL) {
        strlcpy(status, JGetString(rsp, "status"), sizeof(status));
        NoteDeleteResponse(rsp);
    }
    NoteErrorClean(status);
    maxLine1Len = 34;
    statusLen = strlen(status);
    if (statusLen <= maxLine1Len) {
        displayCentered(status);
        displayCentered("");
    } else {
        statusLen = maxLine1Len;
        while (statusLen > 0 && status[statusLen] != ' ')
            statusLen--;
        status[statusLen] = '\0';
        displayCentered(status);
        displayCentered(&status[statusLen+1]);
    }

    // Status of the Notecard
    if (NoteGetStatus(status, sizeof(status), NULL, NULL, NULL)) {
        NoteErrorClean(status);
        displayCentered(status);
    } else {
        displayCentered("");
    }

    // Status of the GPS
    double lat, lon;
    char locStatus[MAXTEXT];
    JTIME time;
    bool locValid = NoteGetLocation(&lat, &lon, &time, locStatus, sizeof(locStatus));
    if (!locValid)
        displayCentered(locStatus);
    else {
        char locstr[64];
        snprintf(locstr, sizeof(locstr), "%.5f %.5f", lat, lon);
        displayCentered(locstr);
    }

    // The current time, displayed in the local timezone as determined by the service
    if (!NoteTimeValid())
        displayCentered("(time not yet known)");
    else {
        char timestr[64];
        char *Country, *Area, *Zone;
        int ZoneOffset;
        NoteRegion(&Country, &Area, &Zone, &ZoneOffset);
        timeStringLocal(NoteTimeST(), ZoneOffset, Zone, timestr, sizeof(timestr));
        displayCentered(timestr);
    }

    // Display it
    displayCenteredEnd();

    // Display the COMMANDS on the bottom line if we're not in auto-refresh mode
    if (!autoRefresh)
        displayBottomLine(FONT_TINY_HIGHLIGHTED, "REFRESH", "", "MENU");

    // Continue to capture button input and call us back to display again
    return MENU_ACTION_CAPTURE;

}
