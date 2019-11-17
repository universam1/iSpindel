#ifndef _MPUOffset_H_
#define _MPUOffset_H_

#include <Arduino.h>
#include <Hash.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include "Globals.h"

class MPUOffset
{
public:
   MPUOffset();
   void ForceHeader();
   void GetSmoothed();
   void Initialize();
   void SetOffsets(int16_t TheOffsets[6]);
   void ShowProgress();
   void PullBracketsOut();
   void SetAveraging(int16_t NewN);
   void calibrate();
   String getStatus();
   int16_t Offset[6];

private:
};
#endif