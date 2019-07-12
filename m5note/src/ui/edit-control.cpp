// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// An implementation of a full-screen edit control that uses a 5-position joystick for selection.
// Note that this has been tuned for the 2.9" e-Ink display with specific font sizes, and thus
// there is significant technical debt as it pertains to generalizing this to other display sizes.

#include "../../m5note.h"
#include "ui.h"

// Fonts
#define FONT_EC                 FONT_SMALL_MONO
#define FONT_EC_BOLD            FONT_SMALL_MONO_BOLD

// Context data
#define ECROWS      5
#define ECCOLS      23

static const char *ecSelectorASCII[] {
        " @.()[]{}<>|'\"`/\\^~?:! ",
        " 0123456789#$%&*_=+-,; ",
		" aAbBcCdDeEfFgGhHiIjJ ",
		" KklLmMnNoOpPqQrRsStT ",
		" uUvVwWxXyYzZ         ",
        };
static int ecSelectorASCIIStartRow = 0;
static int ecSelectorASCIIStartCol = 1;
static bool ecSelectorASCIIDisallowSpace = false;

static const char *ecSelectorPRODUCTID[] {
        "                      ",
        "      0123456789      ",
        "      abcdefghij      ",
        "      klmnopqrst      ",
        "      uvwxyz+-_.@     ",
        };
static int ecSelectorPRODUCTIDStartRow = 1;
static int ecSelectorPRODUCTIDStartCol = 6;
static bool ecSelectorPRODUCTIDDisallowSpace = true;

static const char *ecSelectorNUMERIC[] {
        "                      ",
        "                      ",
        "                      ",
        "                      ",
        "      0123456789      ",
        };
static int ecSelectorNUMERICStartRow = 4;
static int ecSelectorNUMERICStartCol = 6;
static bool ecSelectorNUMERICDisallowSpace = true;

enum cmdIDs {
    CMD_CANCEL,
    CMD_BACKSPC,
    CMD_CLEAR,
    CMD_OK,
};
typedef struct {
    int id;
    const char *name;
    int length;
    int offset;
    int prev;
    int next;
} cmdDef;
#define CMDS 4
static cmdDef command[CMDS] {
    {CMD_CANCEL, "CANCEL", 6, 0, 3, 1},
    {CMD_BACKSPC, "BKSPC", 5, 8, 0, 2},
    {CMD_CLEAR, "CLR", 3, 15, 1, 3},
    {CMD_OK, "OK", 2, 21, 2, 0}
};
static char ecSelector[ECROWS][ECCOLS];
static bool ecDisallowSpace;
static int ecY;
static int cmdY;
static int ecXCenter;
static int ecCharWidth;
static int ecCharHeight;
static int ecSelectedCol;
static int ecLastBottomCol;
static int ecSelectedRow;
static char editedText[64] = {0};

// Map space character
char mapSpace(char ch) {
	if (ch == ' ')
		ch = '_';
	return ch;
}

// Deselect the currently-selected cell
void ecRedrawSel(int fontidFrom, int fontidTo, rect *update) {
    if (ecSelectedRow < ECROWS) {
        char charbufFrom[2], charbufTo[2];
        charbufFrom[0] = ecSelector[ecSelectedRow][ecSelectedCol];
        charbufFrom[1] = '\0';
		charbufTo[0] = charbufFrom[0];
        charbufTo[1] = charbufFrom[1];;
		if (fontidTo == FONT_EC_BOLD)
			charbufTo[0] = mapSpace(charbufTo[0]);
		else
			charbufFrom[0] = mapSpace(charbufFrom[0]);
        displaySetCursor(ecSelectedCol * ecCharWidth, (ecSelectedRow+1) * ecCharHeight);
        displayPrintReplace(fontidFrom, charbufFrom, fontidTo, charbufTo, update);
    } else {
        displaySetCursor(command[ecSelectedCol].offset * ecCharWidth, (ecSelectedRow+1) * ecCharHeight);
        displayPrintReplace(fontidFrom, command[ecSelectedCol].name, fontidTo, command[ecSelectedCol].name, update);
    }
}

// Start the password control
void editControlBegin(char *password, int charset) {

    // Select the appropriate charset
    const char **ecSelectorCharset;
    int ecSelectorStartRow, ecSelectorStartCol;
    switch (charset) {
    case EDIT_CHARSET_PRODUCTID:
        ecSelectorCharset = ecSelectorPRODUCTID;
        ecSelectorStartRow = ecSelectorPRODUCTIDStartRow;
        ecSelectorStartCol = ecSelectorPRODUCTIDStartCol;
        ecDisallowSpace = ecSelectorPRODUCTIDDisallowSpace;
        break;
    case EDIT_CHARSET_URL:
    case EDIT_CHARSET_ASCII:
    default:
        ecSelectorCharset = ecSelectorASCII;
        ecSelectorStartRow = ecSelectorASCIIStartRow;
        ecSelectorStartCol = ecSelectorASCIIStartCol;
        ecDisallowSpace = ecSelectorASCIIDisallowSpace;
        break;
	case EDIT_CHARSET_NUMERIC:
        ecSelectorCharset = ecSelectorNUMERIC;
        ecSelectorStartRow = ecSelectorNUMERICStartRow;
        ecSelectorStartCol = ecSelectorNUMERICStartCol;
        ecDisallowSpace = ecSelectorNUMERICDisallowSpace;
		break;
    }

    // Initialize the new password buffer
    strlcpy(editedText, password, sizeof(editedText)-1);

    // Initialize the selector array
    for (int row=0; row<ECROWS; row++)
        for (int col=0; col<ECCOLS; col++)
            ecSelector[row][col] = ecSelectorCharset[row][col];
    ecSelectedCol = ecSelectorStartCol;
    ecSelectedRow = ecSelectorStartRow;

    // Start fresh
    displayClear();

    // Compute metrics
    int wScreen, hScreen;
    displaySetFont(FONT_EC);
    displayGetFontBounds(&ecCharWidth, &ecCharHeight);
    displayGetScreenBounds(&wScreen, &hScreen);
    cmdY = (ECROWS+1) * ecCharHeight;
    ecY = hScreen - (ecCharHeight/3);
    ecXCenter = wScreen/2;

    // Display the selector
    for (int row=0; row<ECROWS; row++) {
        for (int col=0; col<ECCOLS; col++) {
            displaySetCursor(col * ecCharWidth, (row+1) * ecCharHeight);
            displaySetFont((ecSelectedCol == col && ecSelectedRow == row) ? FONT_EC_BOLD : FONT_EC);
            char charbuf[2];
            charbuf[0] = ecSelector[row][col];
            charbuf[1] = '\0';
            displayPrint(charbuf, PRINT_TRANSPARENT);
        }
    }

    // Display the commands
    for (int cmd=0; cmd<CMDS; cmd++) {
        displaySetCursor(command[cmd].offset * ecCharWidth, cmdY);
        displaySetFont(FONT_EC);
        displayPrint(command[cmd].name, PRINT_TRANSPARENT);
    }

    // Display the existing text
    int len = strlen(editedText);
    displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
    displayPrintReplace(FONT_EC_BOLD, "", FONT_EC_BOLD, editedText, NULL);

    // Display help text
    int wText, hText;
	const char *text = "Press-and-Hold for UP/DOWN";
    displayGetTextExtent(text, &wText, &hText);
    displaySetCursor(ecXCenter-wText/2, ecY-(hText*2));
	displaySetFont(FONT_SMALL_HIGHLIGHTED);
	displayPrint(text, PRINT_OPAQUE);

}

// Process a password dialog, returning one of
// MENU_ACTION_CAPTURE to continue the operation
// MENU_ACTION_COMPLETED if an item is selected (right button)
// MENU_ACTION_CANCEL if the operation is to abort (left button)
int editControlButton(int buttonState, char *buf, int bufLen) {
    int targetCol, targetRow;

    // If we've previously captured input, release it
    switch (buttonState) {

    case BUTTON_RELEASED:
    case BUTTON_REFRESH:
        return MENU_ACTION_CAPTURE;

    case BUTTON_PRESSED_U: {
        rect to, update;
        targetCol = ecSelectedCol;
        if (ecSelectedRow == 0)
            targetRow = ECROWS-1;
        else
            targetRow = ecSelectedRow - 1;
        if (ecDisallowSpace && targetRow < ECROWS-1 && ecSelector[targetRow][targetCol] == ' ')
            return MENU_ACTION_CAPTURE;
        ecRedrawSel(FONT_EC_BOLD, FONT_EC, &update);
        if (ecSelectedRow == 0)
            ecSelectedRow = ECROWS-1;
        else {
            if (ecSelectedRow >= ECROWS)
                ecSelectedCol = ecLastBottomCol;
            ecSelectedRow--;
        }
        ecRedrawSel(FONT_EC, FONT_EC_BOLD, &to);
        return MENU_ACTION_CAPTURE;
    }

    case BUTTON_PRESSED_D: {
        rect from, to;
        if (ecSelectedRow >= ECROWS)
            return MENU_ACTION_CAPTURE;
        targetRow = ecSelectedRow + 1;
        targetCol = ecSelectedCol;
        if (ecDisallowSpace && targetRow != ECROWS &&  ecSelector[targetRow][targetCol] == ' ')
            return MENU_ACTION_CAPTURE;
        ecRedrawSel(FONT_EC_BOLD, FONT_EC, &from);
        ecSelectedRow++;
        if (ecSelectedRow == ECROWS) {
            int selectedCmd = 0;
            for (int cmd=0; cmd<CMDS; cmd++) {
                if (ecSelectedCol >= (command[cmd].offset)) {
                    if (ecSelectedCol <= (command[cmd].offset + command[cmd].length))
                        selectedCmd = cmd;
                }
            }
            ecLastBottomCol = ecSelectedCol;
            ecSelectedCol = selectedCmd;
        }
        ecRedrawSel(FONT_EC, FONT_EC_BOLD, &to);
        return MENU_ACTION_CAPTURE;
    }

    case BUTTON_PRESSED_R: {
        rect from, to;
        targetCol = ecSelectedCol;
        targetRow = ecSelectedRow;
        if (ecSelectedCol == ECCOLS-1)
            targetCol = 0;
        else
            targetCol = ecSelectedCol + 1;
        if (ecDisallowSpace && ecSelectedRow != ECROWS &&  ecSelector[targetRow][targetCol] == ' ')
            return MENU_ACTION_CAPTURE;
        ecRedrawSel(FONT_EC_BOLD, FONT_EC, &from);
        if (ecSelectedRow == ECROWS) {
            ecSelectedCol = command[ecSelectedCol].next;
        } else {
            if (ecSelectedCol == ECCOLS-1)
                ecSelectedCol = 0;
            else
                ecSelectedCol++;
        }
        ecRedrawSel(FONT_EC, FONT_EC_BOLD, &to);
        return MENU_ACTION_CAPTURE;
    }

    case BUTTON_PRESSED_L: {
        rect from, to;
        targetCol = ecSelectedCol;
        targetRow = ecSelectedRow;
        if (ecSelectedCol == ECCOLS-0)
            targetCol = ECCOLS-1;
        else
            targetCol = ecSelectedCol - 1;
        if (ecDisallowSpace && ecSelectedRow != ECROWS &&  ecSelector[targetRow][targetCol] == ' ')
            return MENU_ACTION_CAPTURE;
        ecRedrawSel(FONT_EC_BOLD, FONT_EC, &from);
        if (ecSelectedRow == ECROWS) {
            ecSelectedCol = command[ecSelectedCol].prev;
        } else {
            if (ecSelectedCol == 0)
                ecSelectedCol = ECCOLS-1;
            else
                ecSelectedCol--;
        }
        ecRedrawSel(FONT_EC, FONT_EC_BOLD, &to);
        return MENU_ACTION_CAPTURE;
    }

    case BUTTON_PRESSED_S: {
        rect update1, update2;
        if (ecSelectedRow < ECROWS) {

            int len = strlen(editedText);
            displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
            displayPrintReplace(FONT_EC_BOLD, editedText, FONT_EC_BOLD, "", &update1);
            if (len < sizeof(editedText)-1) {
                editedText[len++] = ecSelector[ecSelectedRow][ecSelectedCol];
                editedText[len] = '\0';
            }
            displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
            displayPrintReplace(FONT_EC_BOLD, "", FONT_EC_BOLD, editedText, &update2);
            return MENU_ACTION_CAPTURE;

        } else {
            switch (command[ecSelectedCol].id) {

            case CMD_CANCEL:
                return MENU_ACTION_CANCEL;

            case CMD_BACKSPC: {
                rect update1, update2;
                int len = strlen(editedText);
                displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
                displayPrintReplace(FONT_EC_BOLD, editedText, FONT_EC_BOLD, "", &update1);
                editedText[--len] = '\0';
                displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
                displayPrintReplace(FONT_EC_BOLD, "", FONT_EC_BOLD, editedText, &update2);
                return MENU_ACTION_CAPTURE;
            }

            case CMD_CLEAR: {
                rect update1, update2;
                int len = strlen(editedText);
                displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
                displayPrintReplace(FONT_EC_BOLD, editedText, FONT_EC_BOLD, "", &update1);
                editedText[0] = '\0';
                displaySetCursor(ecXCenter - ((ecCharWidth * len) / 2), ecY);
                displayPrintReplace(FONT_EC_BOLD, "", FONT_EC_BOLD, editedText, &update2);
                return MENU_ACTION_CAPTURE;
            }

            case CMD_OK: {
                strlcpy(buf, editedText, bufLen-1);
                return MENU_ACTION_COMPLETED;
            }

            }

        }
    }

    }

    return MENU_ACTION_CANCEL;
}
