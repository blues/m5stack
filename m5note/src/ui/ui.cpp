// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// This module contains all processing of UI events from the joystick, as well as asynchronous display events.
// Note that like all of Airnote, this file can be compiled with or without FreeRTOS.  When operating under
// FreeRTOS, the input and display tasks are just that - tasks that operate independently and which use
// events/semaphores to wake up and do their work.  However, when operating in the absence of a multitasking
// kernel, the surrounding program must call uiPoll() on a VERY frequent basis, else the joystick will feel
// extremely sluggish to the user.

#include "../../m5note.h"
#include "ui.h"

// Initialization
static bool uiInitialized = false;
static bool uiActive = false;

// Triggers that influence the home screen painting
static bool dataChanged = false;
static bool clearDisplay = false;

// Async queue of button presses
struct entry_s {
    struct entry_s *next;
    int buttonState;
};
static struct entry_s *first = NULL;
static struct entry_s *last = NULL;

// Enqueue a button press
static bool entryEnqueue(int buttonState) {
    struct entry_s *entry = (struct entry_s *) malloc(sizeof(struct entry_s));
    if (entry == NULL)
        return false;
    entry->next = NULL;
    entry->buttonState = buttonState;
    if (last != NULL)
        last->next = entry;
    last = entry;
    if (first == NULL)
        first = entry;
    return true;
}

// Dequeue a button press FIFO
static bool entryDequeue(int *buttonState) {
    bool dequeued = false;
    if (first == NULL)
        return dequeued;
    if (first != NULL) {
        if (buttonState != NULL)
            *buttonState = first->buttonState;
        dequeued = true;
        struct entry_s *entry = first;
        first = entry->next;
        if (last == entry)
            last = NULL;
        free(entry);
    }
    return dequeued;
}

// Get the button that was pressed, and return the delay to wait for the next
static bool buttonPressPoll(int *button) {

    // Figure out what was pressed, if anything
    int buttonPressed = BUTTON_NONE;
	if (M5.BtnA.wasPressed()) {
		buttonPressed = BUTTON_PRESSED_L;
	} else if (M5.BtnB.wasPressed()) {
		buttonPressed = BUTTON_PRESSED_S;
	} else if (M5.BtnC.wasPressed()) {
		buttonPressed = BUTTON_PRESSED_R;
	} else if (M5.BtnA.wasReleased()) {
        buttonPressed = BUTTON_RELEASED;
	} else if (M5.BtnB.wasReleased()) {
        buttonPressed = BUTTON_RELEASED;
	} else if (M5.BtnC.wasReleased()) {
        buttonPressed = BUTTON_RELEASED;
	}

	// Update the button state after testing
	M5.update();
	
    // Exit with an indication of change
    *button = buttonPressed;
    return (buttonPressed != BUTTON_NONE);

}

// The display polling procedure
static void uiPollDisplay() {

    // If we're showing the home screen and the data has changed, refresh
    if (clearDisplay) {
        clearDisplay = false;
        displayClear(false);
    }

    // Refresh home screen so long as there are changes, noting that because of concurrency
    // it may again be changed while we are in the middle of menuHomeChanged()
    // Do not, however, do this forever or else we'll never recover from a
    // menu that perpetually updates itself.
    for (int i=0; i<5; i++) {

        // Process pending button presses
        int buttonState;
        while (entryDequeue(&buttonState))
            menuButton(buttonState);

        // Done if no data change
        if (!dataChanged)
            break;

        // Process data change
        dataChanged = false;
        menuHomeChanged();
    }

}

// Process all pending messages
static void buttonPoll() {
    int button;
    while (buttonPressPoll(&button))
        entryEnqueue(button);
}

// Data on the screen may have changed.  Note that as a matter of design, this may be called BEFORE uiInit,
// so don't do anything that might require everything to be initialized before calling.
void uiClear() {
    clearDisplay = true;
    dataChanged = true;
}

// Data on the screen may have changed
void uiRefreshStatus(bool wait) {
    dataChanged = true;
    uiPoll();
    while (wait && dataChanged)
        vTaskDelay(250);
}

// Clear the button queue
void uiFlushInput() {
    int state;
    while (entryDequeue(&state))
        ;
}

// Return true if all initializations have been completed and we're now active
bool uiIsActive() {
	return uiActive;
}

// Poll UI subsystems
void uiPoll() {

	// Exit if not initialized
    if (!uiInitialized)
        return;

	// Enable others to know that we are now running
	if (!uiActive) {
		uiActive = true;
		uiRefreshStatus(false);
	}
	
    // If we're not running the button task, poll it
    buttonPoll();

    // If we're not running the display task, poll it
    uiPollDisplay();
    menuPoll();

}

// Initialize the ui
void uiInit() {
    
    // Exit if already initialized
    static bool first = true;
    if (!first) return;
    first = false;

	// Initialize the display subsystem
    displayInit();

    // Initialize the menu subsystem, along with the procedure to display the "idle" screen
    menuInit(showHome);

    // Done
    uiInitialized = true;

}
