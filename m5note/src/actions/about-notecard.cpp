// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the About screen for the Notecard
int actionAboutNotecard(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_RELEASED:
        return MENU_ACTION_CAPTURE;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Display the notecard config and version
    displayClear();
    displayCenteredBegin(FONT_TINY);
	char iccid[64] = {0};
    J *rsp = NoteRequestResponse(NoteNewRequest("card.version"));
    if (rsp != NULL) {
		displayCentered(JGetString(rsp, "name"));
		displayCentered(JGetString(rsp, "version"));
		char *wireless = JGetString(rsp, "wireless");
		char *comma = strchr(wireless, ',');
		if (comma != NULL) {
			const char *prefix = "iccid:";
			int prefixLen = strlen(prefix);
			int len = comma - wireless;
			if (len > sizeof(iccid)-prefixLen-1)
				len = sizeof(iccid)-prefixLen-1;
			strlcpy(iccid, prefix, sizeof(iccid));
			memcpy(&iccid[prefixLen], wireless, len);
			iccid[prefixLen+len] = '\0';
		}
        NoteDeleteResponse(rsp);
    }
    char prod[64], svc[64], device[64], sn[64];
    if (NoteGetServiceConfigST(prod, sizeof(prod), svc, sizeof(svc), device, sizeof(device), sn, sizeof(sn))) {
        if (prod[0] != '\0')
            displayCentered(prod);
        if (svc[0] != '\0')
            displayCentered(svc);
        if (device[0] != '\0')
            displayCentered(device);
        if (sn[0] != '\0')
            displayCentered(sn);
    } else {
        displayCentered("(Notecard is busy)");
    }
	if (iccid[0] != '\0')
		displayCentered(iccid);
    displayCenteredEnd();

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
