// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Show the environment.  This control only really works for small environments.  Because the e-ink display
// updates so slowly, this may actually be good enough.  However, if we need the ability to scroll around
// perhaps we should use the list control to do something scrollable.

#include "../../m5note.h"
#include "../ui/ui.h"

// Show the service environment vars
int actionEnv(int buttonState) {

    // If we've previously captured input, release it
    switch (buttonState) {
    case BUTTON_START:
        break;
    case BUTTON_REFRESH:
    case BUTTON_RELEASED:
        return MENU_ACTION_CAPTURE;
    default:
        return MENU_ACTION_COMPLETED;
    }

    // Clear the display
    int hScreen, y0, y1, lvScreenWidth;
    displaySetFont(FONT_SMALL);
    displayGetScreenBounds(&lvScreenWidth, &hScreen);
    displaySetTextCursor(0, 0, true);
    displayGetCursor(NULL, &y0);
    displaySetTextCursor(0, 1, true);
    displayGetCursor(NULL, &y1);
    int lvRowHeight = y1 - y0;
    int lvRows = hScreen / lvRowHeight;
    int lvCurrentRow = 0;
    displayClear();

    // Get the environment
    char envbuf[512];
    if (!NoteGetEnvAll(envbuf, sizeof(envbuf)))
        strlcpy(envbuf, "can't get env", sizeof(envbuf));

    // Display it
    char *env = envbuf;
    while (*env != '\0') {

        // Extract the next line into linebuf
        char linebuf[MAXTEXT];
        char *eol = strchr(env, '\n');
        char *next = eol+1;
        if (eol == NULL) {
            eol = strchr(env, 0);
            next = eol;
        }
        int len = eol - env;
        if (len > sizeof(linebuf)-1)
            len = sizeof(linebuf)-1;
        memcpy(linebuf, env, len);
        linebuf[len] = '\0';
        env = next;

        // Print it
        displaySetCursor(0, lvRowHeight * (lvCurrentRow+1));
        displayPrint(linebuf, PRINT_LINE|PRINT_UPDATE);
        lvCurrentRow++;
        if (lvCurrentRow >= lvRows)
            lvCurrentRow = 0;

        // Poll
        uiRefreshStatus(false);
    }

    // Capture button input
    return MENU_ACTION_CAPTURE;

}
