// Copyright 2019 Blues Inc.  All rights reserved. 
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "m5note.h"

// By using a notehub.io account "your-email@address.com", data will flow to your notehub.io Personal Project.
// Otherwise, you may specify the Product ID that you selected in your notehub.io's Project Settings.
// If you'd like to enter the Product ID on the device itself, leave this "".
#define NOTEHUB_PRODUCT_ID "EMAIL_REGISTERED_WITH_NOTEHUB.IO"
  
// Tell the Notehub which of its accounts manages this device, and configure how often to upload.
void setup() {

    // Set up for debug output.  If you open Arduino's serial terminal window, you'll be able to
    // watch JSON objects being transferred to and from the Notecard for each request.
    Serial.begin(115200);
    NoteSetDebugOutputStream(Serial);

    // Set I2C as the port we'll use for the Notecard
    NoteInitI2C();

    // Initialize the M5Stack libraries as well as the display
    uiInit();
    menuInit(homeScreen);

    // Do a test transaction to see if the notecard is present
    while (true) {
        if (NoteRequest(NoteNewRequest("service.get")))
            break;
        displayClear();
        displayCenteredLine(FONT_SMALL, "waiting for notecard...");
    }

    // If we'd prefer to set the product ID here, do it.
    if (NOTEHUB_PRODUCT_ID[0] != '\0')
        NoteSetProductID(NOTEHUB_PRODUCT_ID);

}
  
// Main loop
void loop() {

    // Perform polled updates
    uiPoll();

}
