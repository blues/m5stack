// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Requires Arduino libraries:
//  https://github.com/adafruit/Adafruit-GFX-Library

#include "../../m5note.h"

// These are the various fonts available to us
#include "free_fonts.h"
#define FONT_TINY_FONT FSB9
#define FONT_SMALL_FONT FSB12
#define FONT_MEDIUM_FONT FSB18
#define FONT_LARGE_FONT FSB24

// Text size parameters set by displaySetFont
static int fontTextHeight = 1;
static int fontTextAverageWidth = 1;

// Fudge parameters
#define DESCENDER (fontTextHeight/4)
#define HSPACING  (fontTextAverageWidth/4)

// Set the font
void displaySetFont(int fontid) {
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    switch (fontid) {
    case FONT_TINY:
		fontTextHeight = 8;
		fontTextAverageWidth = 6;
		M5.Lcd.setTextSize(1);
        M5.Lcd.setFreeFont(FONT_TINY_FONT);
        break;
    case FONT_SMALL:
		fontTextHeight = 16;
		fontTextAverageWidth = 8;
		M5.Lcd.setTextSize(1);
        M5.Lcd.setFreeFont(FONT_SMALL_FONT);
        break;
    case FONT_SMALL_SELECTED:
		fontTextHeight = 16;
		fontTextAverageWidth = 8;
		M5.Lcd.setTextSize(1);
	    M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5.Lcd.setFreeFont(FONT_SMALL_FONT);
        break;
    case FONT_SMALL_HIGHLIGHTED:
		fontTextHeight = 16;
		fontTextAverageWidth = 8;
		M5.Lcd.setTextSize(1);
	    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
        M5.Lcd.setFreeFont(FONT_SMALL_FONT);
		break;
    case FONT_SMALL_DISABLED:
		fontTextHeight = 16;
		fontTextAverageWidth = 8;
		M5.Lcd.setTextSize(1);
	    M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK);
        M5.Lcd.setFreeFont(FONT_SMALL_FONT);
        break;
    case FONT_MEDIUM:
		fontTextHeight = 24;
		fontTextAverageWidth = 16;
		M5.Lcd.setTextSize(1);
        M5.Lcd.setFreeFont(FONT_MEDIUM_FONT);
        break;
    case FONT_LARGE:
		fontTextHeight = 32;
		fontTextAverageWidth = 28;
		M5.Lcd.setTextSize(1);
        M5.Lcd.setFreeFont(FONT_LARGE_FONT);
        break;
    }
}

// Mark that we're completed with display operations, so we
// can safely power down
void displayCompleted() {
}

// Get the screen params
void displayGetScreenBounds(int *optWidth, int *optHeight) {
    if (optWidth != NULL)
        *optWidth = M5.lcd.width();
    if (optHeight != NULL)
        *optHeight = M5.lcd.height();
}

// Get the current font params (width is of the char "M" in a variable font)
void displayGetFontBounds(int *optWidth, int *optHeight) {
    displayGetTextExtent("M", optWidth, optHeight);
}

// Get the position and width and height of a given string
void displayGetTextBounds(const char *text, int x0, int y0, int16_t *x1, int16_t *y1, uint16_t *optWidth, uint16_t *optHeight) {
    int w, h;
    displayGetTextExtent(text, &w, &h);
    if (x1 != NULL)
        *x1 = x0;
    if (y1 != NULL)
        *y1 = y0 - h;
    if (optWidth != NULL)
        *optWidth = w;
    if (optHeight != NULL)
        *optHeight = h;
}

// Get the width and height of a given string
void displayGetTextExtent(const char *text, int *optWidth, int *optHeight) {
    int16_t x1, y1;
    uint16_t w, h;
	w = strlen(text)*(fontTextAverageWidth+HSPACING);
	h = fontTextHeight+DESCENDER;
    if (optWidth != NULL)
        *optWidth = (int) w;
    if (optHeight != NULL)
        *optHeight = (int) h;
}

// Set the cursor in terms of pixels, with the position being the lower-left corner of text to be painted there.
// That is, text will be painted up and to the right of this position.
void displaySetCursor(int x, int y) {
    M5.Lcd.setCursor(x, y);
}

// Get the current pixel cursor
void displayGetCursor(int *x, int *y) {
    if (x != NULL)
        *x = M5.Lcd.getCursorX();
    if (y != NULL)
        *y = M5.Lcd.getCursorY();
}

// Set the cursor in terms of text rows/cols using the current font
void displaySetTextCursor(int col, int row, bool tightLineSpacing) {
    int charWidth, charHeight;
    if (!tightLineSpacing) {
        displayGetFontBounds(&charWidth, &charHeight);
    } else {
        displayGetFontBounds(&charWidth, &charHeight);
        int16_t x1, y1;
        uint16_t w0, h0, w1, h1;
        displayGetTextBounds("M", 0, 0, &x1, &y1, &w0, &h0);
        charWidth = w0;
        charHeight = h0 + 5;
    }
    M5.Lcd.setCursor(col * charWidth, (row+1) * charHeight);
}

// Print the text transparently on the background
void displayPrint(const char *text, int eraseMode) {
    int16_t x0, y0;
    int16_t x1, y1;
    uint16_t w, h;
    x0 = M5.Lcd.getCursorX();
    y0 = M5.Lcd.getCursorY();
    displayGetTextBounds(text, x0, y0, &x1, &y1, &w, &h);
    x1 -= HSPACING+1;
    w += HSPACING+1;
    h += DESCENDER+1;
    if ((eraseMode & PRINT_LINE) != 0)
        w = (M5.lcd.width() - x1) - 1;
    if ((eraseMode & (PRINT_OPAQUE|PRINT_LINE)) != 0)
        M5.lcd.fillRect(x1, y1, w, h, TFT_BLACK);
    M5.Lcd.print(text);
}

// Optimally replace text at the current pos in a way that doesn't even require an update
void displayPrintReplace(int fromFont, const char *fromText, int toFont, const char *toText, rect *bounds) {
    rect from, to, bound;
    int x = M5.Lcd.getCursorX();
    int y = M5.Lcd.getCursorY();
    displaySetFont(fromFont);
    displayGetTextBounds(fromText, x, y, &from.x, &from.y, &from.w, &from.h);
    displaySetFont(toFont);
    displayGetTextBounds(toText, x, y, &to.x, &to.y, &to.w, &to.h);
    bound.x = from.x;
    bound.w = from.w;
    if (to.w > from.w)
        bound.w = to.w;
    bound.y = from.y;
    bound.h = from.h;
    if (to.h > from.h)
        bound.h = to.h;
    bound.h += DESCENDER*2;
    M5.lcd.fillRect(bound.x, bound.y, bound.w, bound.h, TFT_BLACK);
    M5.Lcd.print(toText);
    if (bounds != NULL)
        *bounds = bound;
}

// Maximize the bounds of a rect.  Note that the output could be the same as one of
// the inputs, so it needs to be done on a copy.
void displayUnionRect(rect *a, rect *b, rect *out) {
    rect o;
    o.x = min(a->x, b->x);
    o.y = min(a->y, b->y);
    o.w = max(a->x+a->w, b->x+b->w) - o.x;
    o.h = max(a->y+a->h, b->y+b->h) - o.y;
    *out = o;
}

// Clear just the specified rect
void displayClearRect(rect *bound) {
    M5.lcd.fillRect(bound->x, bound->y, bound->w, bound->h, TFT_BLACK);
}

// Fill just the specified rect
void displayFillRect(rect *bound) {
    M5.lcd.fillRect(bound->x, bound->y, bound->w, bound->h, TFT_WHITE);
}

// Display a bitmap
void displayBitmap(rect *bound, uint8_t *bitmap) {
    M5.lcd.drawBitmap(bound->x, bound->y, bound->w, bound->h, (uint16_t *) bitmap);
}

// Initialize the M5 libraries and also the display
void displayInit() {
	M5.begin();
    M5.lcd.init();
}

// Clear the display buffer, and set everything to default parameters
void displayClear(bool force) {
    M5.lcd.fillScreen(TFT_BLACK);
    M5.lcd.setTextColor(TFT_WHITE);
}
