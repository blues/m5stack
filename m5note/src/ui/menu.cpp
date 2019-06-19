// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// A full-screen menu control, adapted in part from the style of what we did in the 80's in VisiCalc/1-2-3/Symphony
// in a highly-constrained operating environment.  Some complexity herein results from the fact that e-ink
// technology requires performance hacks related to how we redraw, and from the handling of the full-screen
// edit and list controls.

#include "../../m5note.h"
#include "ui.h"

// The hard-coded menu options (see config-menu.cpp)
static MENU *menu = NULL;

// Params for where we place the menu
#define MENU_BASE_X 0
#define MENU_BASE_Y 0
#define MENU_HEADER_SPACING 0

#define FONT_ACTION             FONT_SMALL
#define FONT_HEADER             FONT_SMALL_HIGHLIGHTED
#define FONT_MENU_SELECTED      FONT_SMALL_SELECTED
#define FONT_MENU_DESELECTED    FONT_SMALL
#define FONT_MENU_DISABLED      FONT_SMALL_DISABLED

// Current menu state
static menuSelectFunc homeFn = NULL;
static menuSelectFunc pressFn = NULL;
static bool menuEnabled = false;
static long menuActivityMs;
static MENU *menuActive = menu;
static int menuActiveSelectionIndex = 0;

// This is true if joystick button input is temporarily captured by a select function
static bool buttonInputCaptured = false;

// Forwards
void menuRedraw();
void menuRedrawSelection(int fromIndex, int toIndex);
void menuActivateParent();
void menuDeactivate();
void menuActionCompleted();
void menuActivateSelection(int newmenu, int selectionIndex);

// Initialize the menu subsystem 
void menuSet(MENU *pMenu, menuSelectFunc fnHome, menuSelectFunc fnPress) {
    menu = pMenu;
    homeFn = fnHome;
    pressFn = fnPress;
    menuHomeChanged();
}

// Activate a captured selection
void menuStartSelection(int newmenu, int selectionIndex) {
    menuActivateSelection(newmenu, selectionIndex);
    displayClear();
    SELECTION *pSel = &menuActive->selection[menuActiveSelectionIndex];
    int action = pSel->menuSelectFn(pSel->menuSelectArg);
    buttonInputCaptured = (action == MENU_ACTION_CAPTURE);
}

// Find the specified menu, by method name
bool menuStart(menuSelectFunc fn) {
    for (int m=0; menu[m].label[0] != '\0'; m++) {   // ENDMENUS
        for (int s=0; menu[m].selection[s].label[0] != '\0'; s++) {
            if (menu[m].selection[s].menuSelectFn == fn) {
                menuStartSelection(m, s);
                return true;
            }
        }
    }
    return false;
}

// Count the number of selections in a menu item
int menuSelections(int menuid) {
    int count;
    for (count=0; menu[menuid].selection[count].label[0] != '\0'; count++) ;
    return count;
}

// Indicate that there is menu activity
void menuActivity() {
    menuActivityMs = millis();
}

// Activate a menu while also selecting a specific selection
void menuActivateSelection(int newmenu, int selectionIndex) {
    menuEnabled = true;
    menuActivityMs = millis();
    menuActive = &menu[newmenu];
    menuActiveSelectionIndex = selectionIndex;
    debugf("%s / %s\n", menuActive->label, menuActive->selection[menuActiveSelectionIndex].label);
}

// Go back
int menuBack(int unused) {
    menuActivateParent();
    return MENU_ACTION_COMPLETED;
}

// Activate the specified menu and draw it
int menuActivate(int newmenu) {
    menuActivateSelection(newmenu, 0);
    menuRedraw();
    return MENU_ACTION_COMPLETED;
}

// Call the selection refresh method if we're captured
bool pollIfCaptured() {
    if (!buttonInputCaptured)
        return false;
    static int inPoll = 0;
    if (inPoll++ != 0) {
        inPoll--;
        return false;
    }
    SELECTION *pSel = &menuActive->selection[menuActiveSelectionIndex];
    pSel->menuSelectFn(BUTTON_REFRESH);
    menuActivityMs = millis();
    inPoll--;
    return true;
}

// Process a poll request that happens when menu is up, and return a value
// of the max number of milliseconds when we'd like to next be polled back
unsigned menuPoll() {
    int neverPollMs = 300000;

    // Exit if we're no longer enabled
    if (!menuEnabled)
        return neverPollMs;

    // Poll if we're captured
    pollIfCaptured();

    // If the menu has been up for too long since last activity, deactivate
    if (millis() > menuActivityMs + MENU_CANCEL_MS) {
        menuDeactivate();
        return neverPollMs;
    }

    // We'd like to be polled at least this often
    return MENU_POLL_MS;
}

// Process a button state change
void menuButton(int buttonState) {

    // If any button is pressed, bring the menu up
    if (!menuEnabled) {
        if (pressFn != NULL)
            pressFn(buttonState);
        return;
    }

    // If button input is temporarily captured, reroute it to the current selection
    if (buttonInputCaptured) {
        SELECTION *pSel = &menuActive->selection[menuActiveSelectionIndex];
        int action = pSel->menuSelectFn(buttonState);
        if (action == MENU_ACTION_CAPTURE)
            menuActivityMs = millis();
        else {
            buttonInputCaptured = false;
            if (action == MENU_ACTION_DISMISS) {
                menuDeactivate();
            } else {
                displayClear();
                menuRedraw();
            }
        }
        return;
    }

    // Dispatch on the button pressed
    switch (buttonState) {

    case BUTTON_PRESSED_R: {
        bool enabled;
        int previousIndex = menuActiveSelectionIndex;
        do {
            if (menuActiveSelectionIndex < menuSelections(menuActive->id)-1)
                menuActiveSelectionIndex++;
            else
                menuActiveSelectionIndex = 0;
            enabled = true;
            if (menuActive->selection[menuActiveSelectionIndex].menuFlagsFn != NULL) {
                if ((menuActive->selection[menuActiveSelectionIndex].menuFlagsFn(NULL, 0) & MENU_FLAG_DISABLED) != 0)
                    enabled = false;
            }
        } while (!enabled);
        menuRedrawSelection(previousIndex, menuActiveSelectionIndex);
        menuActivityMs = millis();
        break;
    }

    case BUTTON_PRESSED_U: {
        displayClear();
        menuActivateParent();
        menuActivityMs = millis();
        break;
    }

    case BUTTON_PRESSED_L: {
        bool enabled;
        int previousIndex = menuActiveSelectionIndex;
        do {
            if (menuActiveSelectionIndex > 0)
                menuActiveSelectionIndex--;
            else
                menuActiveSelectionIndex = menuSelections(menuActive->id)-1;
            enabled = true;
            if (menuActive->selection[menuActiveSelectionIndex].menuFlagsFn != NULL) {
                if ((menuActive->selection[menuActiveSelectionIndex].menuFlagsFn(NULL, 0) & MENU_FLAG_DISABLED) != 0)
                    enabled = false;
            }
        } while (!enabled);
        menuRedrawSelection(previousIndex, menuActiveSelectionIndex);
        menuActivityMs = millis();
        break;
    }

    case BUTTON_PRESSED_S: {
        displayClear();
        SELECTION *pSel = &menuActive->selection[menuActiveSelectionIndex];
        if (pSel->menuSelectFn != menuActivate)
            uiFlushInput();
        int action = pSel->menuSelectFn(pSel->menuSelectArg);
        buttonInputCaptured = (action == MENU_ACTION_CAPTURE);
        if (pSel->menuSelectFn != menuActivate && pSel->menuSelectFn != menuBack && action == MENU_ACTION_COMPLETED)
            menuActionCompleted();
        else if (action == MENU_ACTION_DISMISS)
            menuDeactivate();
        menuActivityMs = millis();
        break;
    }

    }

}

// Find the parent of current menu and activate it
void menuActivateParent() {
    if (menuEnabled) {
        if (menuActive->id == menuActive->parentid) {
            menuDeactivate();
        } else {
            int parentEntry = 0;
            int parentSelections = menuSelections(menuActive->parentid);
            for (int i=0; i<parentSelections; i++) {
                if (menu[menuActive->parentid].selection[i].menuSelectFn == menuActivate &&
                    menu[menuActive->parentid].selection[i].menuSelectArg == menuActive->id)
                    parentEntry = i;
            }
            menuActivateSelection(menuActive->parentid, parentEntry);
            menuRedraw();
        }
    }
}

// Refresh the home screen if the menu is NOT being actively displayed
void menuHomeChanged() {
    if (!menuEnabled) {
        if (homeFn != NULL) {
            homeFn(BUTTON_REFRESH);
        } else {
            displayClear();
        }
    } else {
        pollIfCaptured();
    }
}

// Deactivate the menu, and re-enable the home screen
void menuDeactivate() {
    menuEnabled = false;
    buttonInputCaptured = false;
    displayClear();
    if (homeFn != NULL) {
        homeFn(BUTTON_START);
    }
}

// Return to menu mode, clearing the screen
void menuActionCompleted() {

    // If the menu isn't active, do nothing
    if (!menuEnabled)
        return;

    // Return to the currently-selected menu
    displayClear();
    menuRedraw();

}

// Apply the flags to a selection label, into a MAXTEXT buffer
void menuLabelWithFlags(char *buf, char *label, menuFlagsFunc flagsFn) {
    int flags = 0;
    char annotation[20];
    annotation[0] = '\0';
    if (flagsFn != NULL) {
        flags = flagsFn(annotation, sizeof(annotation));
        annotation[sizeof(annotation)-1] = '\0';
        annotation[sizeof(annotation)-2] = '.';
        annotation[sizeof(annotation)-3] = '.';
        annotation[sizeof(annotation)-4] = '.';
    }
    if (annotation[0] == '\0')
        snprintf(buf, MAXTEXT-1, "%s%s", label, ((flags & MENU_FLAG_SELECTED) != 0) ? " *" : "");
    else
        snprintf(buf, MAXTEXT-1, "%s%s (%s)", label, ((flags & MENU_FLAG_SELECTED) != 0) ? " *" : "", annotation);
}


// Redraw the menu on a background that we assume to be blank
void menuRedraw() {

    // Don't ever redraw the hidden menu
    if (menuActive->label[0] == '-' && menuActive->label[1] == '\0') {
        menuDeactivate();
        return;
    }

    // Display the menu header
    displaySetFont(FONT_HEADER);
    displaySetTextCursor(MENU_BASE_X, MENU_BASE_Y, true);
    displayPrint(menuActive->label, PRINT_LINE);

    // Navigate through the menu, displaying all entries
    for(int selection=0; menuActive->selection[selection].label[0] != '\0'; selection++) {

        // Position it always in terms of the deselected font positioning
        displaySetFont(FONT_MENU_DESELECTED);
        displaySetTextCursor(MENU_BASE_X + 2, MENU_BASE_Y + 1 + MENU_HEADER_SPACING + selection, true);

        // Select font based upon whether or not it's selected
        int fontid = selection == menuActiveSelectionIndex ? FONT_MENU_SELECTED : FONT_MENU_DESELECTED;
        if (menuActive->selection[selection].menuFlagsFn != NULL) {
            if ((menuActive->selection[selection].menuFlagsFn(NULL, 0) & MENU_FLAG_DISABLED) != 0)
                fontid = FONT_MENU_DISABLED;
        }
        displaySetFont(fontid);

        // Display it
        char label[MAXTEXT];
        menuLabelWithFlags(label, menuActive->selection[selection].label, menuActive->selection[selection].menuFlagsFn);
        displayPrint(label, PRINT_LINE);

    }

}

// Optimally redraw two lines by moving the selection from one place to another.  Note that we
// always set the font to deselected before setting the text cursor, so that we always have a
// uniform placement of the text regardless of whether we're painting selected or deselected font.
void menuRedrawSelection(int fromIndex, int toIndex) {
    rect from, to;
    char fromText[MAXTEXT];
    menuLabelWithFlags(fromText, menuActive->selection[fromIndex].label, menuActive->selection[fromIndex].menuFlagsFn);
    displaySetFont(FONT_MENU_DESELECTED);
    displaySetTextCursor(MENU_BASE_X + 2, MENU_BASE_Y + 1 + MENU_HEADER_SPACING + fromIndex, true);
    displayPrintReplace(FONT_MENU_SELECTED, fromText, FONT_MENU_DESELECTED, fromText, &from);
    char toText[MAXTEXT];
    menuLabelWithFlags(toText, menuActive->selection[toIndex].label, menuActive->selection[toIndex].menuFlagsFn);
    displaySetFont(FONT_MENU_DESELECTED);
    displaySetTextCursor(MENU_BASE_X + 2, MENU_BASE_Y + 1 + MENU_HEADER_SPACING + toIndex, true);
    displayPrintReplace(FONT_MENU_DESELECTED, toText, FONT_MENU_SELECTED, toText, &to);
}
