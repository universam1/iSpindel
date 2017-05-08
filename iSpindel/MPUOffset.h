#ifndef _MPUOffset_H_
#define _MPUOffset_H_

#include "Globals.h"
// #include "I2Cdev.h"
// // #include "MPU6050.h"
// #include "Wire.h"

class MPUOffset {
 public:
    MPUOffset();
    void ForceHeader();
    void GetSmoothed();
    void Initialize();
    void SetOffsets(int TheOffsets[6]);
    void ShowProgress();
    void PullBracketsOut();
    void SetAveraging(int NewN);
    void calibrate();
    String getStatus();
 private:
};
#endif