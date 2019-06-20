// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Handy utilities built upon the display primitives that handle the most common methods of presentation
// on the Airnote, which are full-centered or bottom-line centered.

#include "../../m5note.h"

// For centered display
#define MAXROWS 25
#define MAXCHARSPERLINE 64
static char dispRow[MAXROWS][MAXCHARSPERLINE];
static int dispRows;
static int dispMaxRows;
static int dispFont;

// Begin adding rows to center on the display.  Font is used to determine
// the maximum number of rows to append.
void displayCenteredBegin(int font) {
    dispRows = 0;
    displaySetFont(font);
	dispFont = font;
    int wScreen, hScreen, wText, hText;
    displayGetScreenBounds(&wScreen, &hScreen);
    displayGetFontBounds(&wText, &hText);
    dispMaxRows = hScreen/hText;
    if (dispMaxRows > MAXROWS)
        dispMaxRows = dispMaxRows;
}

// APpend a row to be centered on the display
void displayCentered(const char *text) {
    if (dispRows < dispMaxRows)
        strlcpy(dispRow[dispRows++], text, sizeof(dispRow[0]));
}

// Display the rows and update the screen
void displayCenteredEnd() {
    int hText;
    displaySetFont(dispFont);
    displayClear(false);
    displayGetFontBounds(NULL, &hText);
    for (int i=0; i<dispRows; i++) {
        if (i >= dispMaxRows)
            break;
        int wText, wScreen, hScreen;
        char *text = dispRow[i];
        displayGetScreenBounds(&wScreen, &hScreen);
        displayGetTextExtent(text, &wText, NULL);
        int cursorBaseY = ((hScreen - hText*dispRows)/2) + (hText*2)/3;
        displaySetCursor(wScreen/2-wText/2, cursorBaseY + i*hText);
        displayPrint(text, PRINT_TRANSPARENT);
    }
}

// Display the rows and update the screen
void displayCenteredBottomEnd() {
    int hText;
    displaySetFont(dispFont);
    displayGetFontBounds(NULL, &hText);
    for (int i=0; i<dispRows; i++) {
        if (i >= dispMaxRows)
            break;
        int wText, wScreen, hScreen;
        char *text = dispRow[i];
        displayGetScreenBounds(&wScreen, &hScreen);
        displayGetTextExtent(text, &wText, NULL);
		int cursorBaseY = hScreen - hText/4 - (dispRows-i-1)*hText;
	    displaySetCursor(wScreen/2-wText/2, cursorBaseY - (hText/2));
        displayPrint(text, PRINT_TRANSPARENT);
    }
}

// Display a single row, centered
void displayCenteredLine(int font, const char *text) {
    displayCenteredBegin(font);
    displayCentered(text);
    displayCenteredEnd();
}

// Display a single row, centered on the bottom of the display
void displayBottomLine(int font, const char *text) {
    displaySetFont(font);
    int wText, wScreen, hScreen, hText;
    displayGetFontBounds(NULL, &hText);
    displayGetScreenBounds(&wScreen, &hScreen);
    displayGetTextExtent(text, &wText, NULL);
    int cursorBaseY = hScreen - hText/4;
    displaySetCursor(wScreen/2-wText/2, cursorBaseY);
    displayPrint(text, PRINT_TRANSPARENT);
}
