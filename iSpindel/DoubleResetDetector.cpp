/*
	FILE: 		DoubleResetDetector.cpp
	VERSION: 	0.0.1
	PURPOSE: 	Trigger configure mode by resetting Arduino twice.
	LICENCE:	GPL v3 (http://www.gnu.org/licenses/gpl.html)
 */

#include "DoubleResetDetector.h"

// Flag which will be stored in RTC memory.
// A uint32_t is used so that two different magic numbers can be used, 
// without accidentally overwriting memory used for another purpose.
uint32_t doubleResetDetectorFlag;

DoubleResetDetector::DoubleResetDetector(int timeout, int address) {
	this->timeout = timeout*1000;
	this->address = address;
	doubleResetDetected = false;
	waitingForDoubleReset = false;
}

bool DoubleResetDetector::detectDoubleReset() {
	doubleResetDetected = detectRecentlyResetFlag();
	if (doubleResetDetected) {
		clearRecentlyResetFlag();
	} else {
		setRecentlyResetFlag();
		waitingForDoubleReset = true;
	}
	return doubleResetDetected;
}

void DoubleResetDetector::loop() {
	if (waitingForDoubleReset && millis() > timeout) stop();
}

void DoubleResetDetector::stop() {
	clearRecentlyResetFlag();
	waitingForDoubleReset = false;
}

bool DoubleResetDetector::detectRecentlyResetFlag() {
	doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG_CLEAR;
	ESP.rtcUserMemoryRead(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
	doubleResetDetected = doubleResetDetectorFlag == DOUBLERESETDETECTOR_FLAG_SET;
	return doubleResetDetected;
}

void DoubleResetDetector::setRecentlyResetFlag() {
	doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG_SET;
	ESP.rtcUserMemoryWrite(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
}

void DoubleResetDetector::clearRecentlyResetFlag() {
	doubleResetDetectorFlag = DOUBLERESETDETECTOR_FLAG_CLEAR;
	ESP.rtcUserMemoryWrite(address, &doubleResetDetectorFlag, sizeof(doubleResetDetectorFlag));
}
// EOF