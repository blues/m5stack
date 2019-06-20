// Copyright 2019 Blues Inc.  All rights reserved. 
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include "m5note.h"
  
// Tell the Notehub which of its accounts manages this device, and configure how often to upload.
void setup() {

	// Set up for debug output.  If you open Arduino's serial terminal window, you'll be able to
	// watch JSON objects being transferred to and from the Notecard for each request.
	Serial.begin(115200);
    NoteSetDebugOutputPort(&Serial);

	// Set I2C as the port we'll use for the Notecard
    NoteInitI2C();

	// Initialize the M5Stack libraries as well as the display
    uiInit();

	// Perform one-time Notecard initializations
	while (true) {
	    J *req = NoteNewRequest("service.set");
	    JAddStringToObject(req, "product", "ray@ozzie.net");
	    JAddStringToObject(req, "mode", "off");
		if (NoteRequest(req))
			break;
	    displayClear(false);
		displayCenteredBegin(FONT_SMALL);
		displayCentered("waiting for notecard...");
		displayCenteredEnd();
	}

}
  
// Main loop
void loop() {

	// Perform polled updates
    uiPoll();

}
