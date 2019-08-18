// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Test methods

#include "../../m5note.h"
#include "../ui/ui.h"

// Return an appropriate menu flag as to whether or not we're in "continuously-connected" mode
int flagTestIsContinuous(char *buf, int buflen) {
    J *rsp = NoteRequestResponse(NoteNewRequest("service.get"));
    if (rsp != NULL) {
        if (strcmp(JGetString(rsp, "mode"), "continuous") == 0)
            return MENU_FLAG_SELECTED;
    }
    return 0;
}

// Toggle the mode between periodic and continuous
int actionTestToggleContinuous(int buttonState) {
    bool isContinuous = flagTestIsContinuous(NULL, 0) == MENU_FLAG_SELECTED;
    J *req = NoteNewRequest("service.set");
    JAddStringToObject(req, "mode", isContinuous ? "periodic" : "continuous");
    NoteRequest(req);
    return MENU_ACTION_DISMISS;
}

// Add a Note to the file "test.qo"
int actionTestAdd(int buttonState) {
    static unsigned messagesSent = 0;
    J *body = JCreateObject();
    if (body != NULL) {
        // Use the temperature sensor on the Notecard to get a sample temperature measurement
        double temperature;
        // This results in a JSON message to Notecard formatted like:
        //     { "req"     : "card.temp" }
        NoteGetTemperature(&temperature);
        // Insert the temperature measurement into the body of our Note
        JAddNumberToObject(body, "temperature", temperature);

        // Use the voltage ADC on the Notecard to get a sample voltage measurement
        double voltage;
        // This results in a JSON message to Notecard formatted like:
        //     { "req"     : "card.voltage" }
        NoteGetVoltage(&voltage);
        // Insert the voltage measurement into the body of the Note
        JAddNumberToObject(body, "voltage", voltage);

        // Insert incrementing message count into the body of the Note
        JAddNumberToObject(body, "count", ++messagesSent);

        // EXAMPLE OF HOW TO INSERT A BOOL INTO THE BODY OF THE NOTE
        // Jbool truth = true;
        // JAddBoolToObject(body, "truth", truth);

        // EXAMPLE OF HOW TO INSERT A STRING INTO THE BODY OF THE NOTE
        // JAddStringToObject(body, "string", "value");

        // EXAMPLE OF HOW TO INSERT AN ARRAY INTO THE BODY OF THE NOTE
        // float myFloatArray[] = { 2.71828, 3.14159, 42 };
        // J *array = JCreateFloatArray(myFloatArray, sizeof(myFloatArray)/sizeof(myFloatArray[0]));
        // JAddItemToObject(body, "array", array);

        // EXAMPLE OF HOW TO INSERT AN OBJECT INTO THE BODY OF THE NOTE
        // J *object = JAddObjectToObject(body, "object");
        // JAddStringToObject(object, "first",  "Who");
        // JAddStringToObject(object, "second", "What");
        // JAddStringToObject(object, "third",  "I Don't Know");

        // Send the Note to the Notecard file "test.qo", but do not Sync immediately
        NoteSend("test.qo", body, false);
    }
    return MENU_ACTION_DISMISS;
}

// Force a synchronization manually, as opposed to waiting for the notecard to sync periodically on its own
int actionTestSync(int buttonState) {
    NoteRequest(NoteNewRequest("service.sync"));
    return MENU_ACTION_DISMISS;
}
