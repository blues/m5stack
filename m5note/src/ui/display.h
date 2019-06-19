// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Definitions for the lowest-level e-ink display driver, which are largely wrappers around Adafruit's graphics
// library, ZinggJM's e-ink libraries, and U8G2 font libraries.

#pragma once

// Font definitions
enum fontIDs {
    FONT_TINY,
    FONT_TINY_HIGHLIGHTED,
    FONT_SMALL_MONO,
    FONT_SMALL_MONO_BOLD,
    FONT_SMALL,
    FONT_SMALL_HIGHLIGHTED,
    FONT_SMALL_DISABLED,
    FONT_SMALL_SELECTED,
    FONT_MEDIUM,
    FONT_LARGE,
};

// General rectangle
typedef struct {
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} rect;

// Maximum chars for static text buffers being displayed on the screen
#define MAXTEXT 48

// Public
#define     PRINT_TRANSPARENT   0x00000000  // Print over what's already there
#define     PRINT_OPAQUE        0x00000001  // Erase text area before printing
#define     PRINT_LINE          0x00000002  // Erase entire line before printing
#define     PRINT_UPDATE        0x00000004  // Update rect after printing

void displayPrint(const char *text, int eraseMode);
void displayPrintReplace(int fromFont, const char *fromText, int toFont, const char *toText, rect *bounds);
void displayUnionRect(rect *a, rect *b, rect *out);
void displayUpdateRect(rect *bound);
void displaySetFont(int fontid);
void displaySetCursor(int x, int y);
void displayGetCursor(int *x, int *y);
void displaySetTextCursor(int col, int row, bool includeLineSpacing);
void displayClear(void);
void displayClearRect(rect *bound);
void displayFillRect(rect *bound);
void displayBitmap(rect *bound, uint8_t *bitmap);
void displayUpdate();
void displayCompleted();
void displayGetTextBounds(const char *text, int x0, int y0, int16_t *x1, int16_t *y1, uint16_t *optWidth, uint16_t *optHeight);
void displayGetTextExtent(const char *text, int *optWidth, int *optHeight);
void displayGetFontBounds(int *optWidth, int *optHeight);
void displayGetScreenBounds(int *optWidth, int *optHeight);
void displayInit();

