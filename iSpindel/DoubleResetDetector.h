/*
	FILE: 		DoubleResetDetector.h
	VERSION: 	0.0.1
	PURPOSE: 	Trigger configure mode by resetting Arduino twice.
	LICENCE:	GPL v3 (http://www.gnu.org/licenses/gpl.html)
 */

#ifndef DoubleResetDetector_H__
#define DoubleResetDetector_H__

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define DOUBLERESETDETECTOR_VERSION "0.0.2"
#define DOUBLERESETDETECTOR_FLAG_SET 0xD0D01234
#define DOUBLERESETDETECTOR_FLAG_CLEAR 0xD0D04321

class DoubleResetDetector
{
public:
	DoubleResetDetector(int timeout, int address);
	bool detectDoubleReset();
	bool doubleResetDetected;
	void loop();
	void stop();
	void clearRecentlyResetFlag();
	void setRecentlyResetFlag();
	
private:
	int timeout;
	int address;
	bool waitingForDoubleReset;
	bool detectRecentlyResetFlag();
	uint32_t doubleResetDetectorFlag;
};
#endif // DoubleResetDetector_H__