// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

// Functions for all actions dispatched from menus

#pragma once

// These location modes correspond by design (with a few extensions) to card.location.mode
#define LOCATION_INDOOR		"off"
#define LOCATION_OUTDOOR	"motion"
#define	LOCATION_MOBILE		"continuous"
#define LOCATION_PERIODIC	"periodic"
#define LOCATION_FIXED		"fixed"
#define LOCATION_TRACK		"track"
#define LOCATION_UNKNOWN	"unknown"

//
// The various actions in their respective files
//

int actionSetSerialNumber(int buttonState);
int actionSetProductID(int buttonState);
int actionProductID(int buttonState);
int actionEnv(int buttonState);
int actionAboutNotecard(int buttonState);
int flagIsLocIndoor(char *buf, int buflen);
int actionSetLocIndoor(int arg);
int flagIsLocOutdoor(char *buf, int buflen);
int actionSetLocOutdoor(int arg);
int flagIsLocMobile(char *buf, int buflen);
int actionSetLocMobile(int arg);
int flagIsLocTrack(char *buf, int buflen);
int actionSetLocTrack(int arg);
int flagIsLocPeriodic(char *buf, int buflen);
int actionSetLocPeriodic(int arg);
int actionSetLocPeriod(int arg);
int actionFactoryRestore(int buttonState);
int actionSetSyncPeriod(int buttonState);
int flagTestIsContinuous(char *buf, int buflen);
int actionTestToggleContinuous(int buttonState);
int actionTestAdd(int buttonState);
int actionTestSync(int buttonState);
