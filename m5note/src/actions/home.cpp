// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// The product's home screen

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the home screen
int actionHome(int buttonState) {

    // Suppress the frequency of UI display
    static bool fast = true;
    static uint32_t refreshTimer = 0;
    static uint32_t lastPoll = 0;
	if (!timerExpiredMs(&lastPoll, 250)) {
		lastPoll = millis();
	    if (!timerExpiredSecs(&refreshTimer, fast ? 1 : 5))
	        return MENU_ACTION_CAPTURE;
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

    // Remember the last time we were refreshed, and schedule a refresh
    refreshTimer = millis();

    // Display the status screen
    displayCenteredBegin(FONT_TINY);

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

    char status[128] = {0};
    if (NoteGetNetStatus(status, sizeof(status))) {
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
        fast = memcmp(status, "idle", 4) != 0;
    } else {
        displayCentered("");
    }

    if (NoteGetStatus(status, sizeof(status), NULL, NULL, NULL)) {
        NoteErrorClean(status);
        displayCentered(status);
    } else {
        displayCentered("");
    }

    double lat, lon;
    char locStatus[MAXTEXT];
    epoch time;
    bool locValid = NoteGetLocation(&lat, &lon, &time, locStatus, sizeof(locStatus));
    if (!locValid)
        displayCentered(locStatus);
    else {
        char locstr[64];
        snprintf(locstr, sizeof(locstr), "%.5f %.5f", lat, lon);
        displayCentered(locstr);
    }

    if (!NoteTimeValid())
        displayCentered("(time not yet known)");
    else {
        char timestr[64];
        timeString(NoteTimeST(), timestr, sizeof(timestr));
        displayCentered(timestr);
    }

    displayCenteredEnd();

    // Capture button input
    return MENU_ACTION_CAPTURE;
}
