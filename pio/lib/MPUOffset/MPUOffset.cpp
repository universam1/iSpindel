#include "MPUOffset.h"

const char LBRACKET = '[';
const char RBRACKET = ']';
const char COMMA = ',';

const int16_t iAx = 0;
const int16_t iAy = 1;
const int16_t iAz = 2;
const int16_t iGx = 3;
const int16_t iGy = 4;
const int16_t iGz = 5;

const int16_t NFast = 1000;  // the bigger, the better (but slower)
const int16_t NSlow = 10000; // ..
const int16_t LinesBetweenHeaders = 5;
int16_t LowValue[6];
int16_t HighValue[6];
int16_t Smoothed[6];
int16_t LowOffset[6];
int16_t HighOffset[6];
int16_t Target[6];
int16_t LinesOut;
int16_t N;
String statustext = "idle";

MPUOffset::MPUOffset() {}

void MPUOffset::ForceHeader()
{
  LinesOut = 99;
}

void MPUOffset::GetSmoothed()
{
  int16_t RawValue[6];
  long Sums[6];
  for (int i = iAx; i <= iGz; i++)
  {
    Sums[i] = 0;
  }

  for (int i = 1; i <= N; i++)
  { // get sums
    yield();
    accelgyro.getMotion6(&RawValue[iAx], &RawValue[iAy], &RawValue[iAz],
                         &RawValue[iGx], &RawValue[iGy], &RawValue[iGz]);
    for (int j = iAx; j <= iGz; j++)
      Sums[j] = Sums[j] + RawValue[j];
  } // get sums
  for (int i = iAx; i <= iGz; i++)
  {
    Smoothed[i] = (Sums[i] + N / 2) / N;
  }
} // GetSmoothed

void MPUOffset::Initialize()
{
  statustext = "Initialize";
  // // join I2C bus (I2Cdev library doesn't do this automatically)
  //  Wire.begin(D3, D4);

  // Serial.begin(115200);

  // // initialize device
  // Serial.println(F("Initializing I2C devices..."));
  // accelgyro.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  accelgyro.setDLPFMode(MPU6050_DLPF_BW_5);

  SetOffsets(Offset);

  accelgyro.CalibrateAccel(6);
  accelgyro.CalibrateGyro(6);
  Serial.println("\nat 600 Readings");
  accelgyro.PrintActiveOffsets();
  Serial.println();
  // accelgyro.CalibrateAccel(1);
  // accelgyro.CalibrateGyro(1);
  // Serial.println("700 Total Readings");
  // accelgyro.PrintActiveOffsets();
  // Serial.println();
  // accelgyro.CalibrateAccel(1);
  // accelgyro.CalibrateGyro(1);
  // Serial.println("800 Total Readings");
  // accelgyro.PrintActiveOffsets();
  // Serial.println();
  // accelgyro.CalibrateAccel(1);
  // accelgyro.CalibrateGyro(1);
  // Serial.println("900 Total Readings");
  // accelgyro.PrintActiveOffsets();
  // Serial.println();
  // accelgyro.CalibrateAccel(1);
  // accelgyro.CalibrateGyro(1);
  // Serial.println("1000 Total Readings");
  // accelgyro.PrintActiveOffsets();
  // Serial.println("\n\n Any of the above offsets will work nice \n\n Lets proof the PID tuning using another method:");
} // Initialize

void MPUOffset::SetOffsets(int16_t TheOffsets[6])
{
  accelgyro.setXAccelOffset(TheOffsets[iAx]);
  accelgyro.setYAccelOffset(TheOffsets[iAy]);
  accelgyro.setZAccelOffset(TheOffsets[iAz]);
  accelgyro.setXGyroOffset(TheOffsets[iGx]);
  accelgyro.setYGyroOffset(TheOffsets[iGy]);
  accelgyro.setZGyroOffset(TheOffsets[iGz]);
} // SetOffsets

void MPUOffset::ShowProgress()
{
  if (LinesOut >= LinesBetweenHeaders)
  { // show header
    Serial.println(F("\tXAccel\t\t\tYAccel\t\t\t\tZAccel"));
    LinesOut = 0;
  } // show header
  for (int i = iAx; i <= iAz; i++)
  {
    Serial.print(LBRACKET);
    Serial.print(LowOffset[i]),
        Serial.print(COMMA);
    Serial.print(HighOffset[i]);
    Serial.print(F("] --> ["));
    Serial.print(LowValue[i]);
    Serial.print(COMMA);
    Serial.print(HighValue[i]);
    if (i == iAz)
    {
      Serial.println(RBRACKET);
    }
    else
    {
      Serial.print("]\t");
    }
  }
  LinesOut++;
} // ShowProgress

void MPUOffset::PullBracketsOut()
{
  boolean Done = false;
  int NextLowOffset[6];
  int NextHighOffset[6];

  SetOffsets(HighOffset);
  GetSmoothed();
  for (int i = iAx; i <= iGz; i++)
  {
    HighValue[i] = Smoothed[i]; // needed for ShowProgress
  }

  while (!Done)
  {
    Done = true;
    SetOffsets(LowOffset);
    GetSmoothed();
    for (int i = iAx; i <= iGz; i++)
    { // got low values
      LowValue[i] = Smoothed[i];
      if (LowValue[i] >= Target[i])
      {
        Done = false;
        NextLowOffset[i] = LowOffset[i] - 1000;
      }
      else
      {
        NextLowOffset[i] = LowOffset[i];
      }
    } // got low values
    ShowProgress();
    for (int i = iAx; i <= iGz; i++)
    {
      LowOffset[i] = NextLowOffset[i]; // had to wait until ShowProgress done
    }
  } // keep going

  Done = false;
  while (!Done)
  {
    Done = true;
    SetOffsets(HighOffset);
    GetSmoothed();
    for (int i = iAx; i <= iGz; i++)
    { // got high values
      HighValue[i] = Smoothed[i];
      if (HighValue[i] <= Target[i])
      {
        Done = false;
        NextHighOffset[i] = HighOffset[i] + 1000;
      }
      else
      {
        NextHighOffset[i] = HighOffset[i];
      }
    } // got high values
    ShowProgress();
    for (int i = iAx; i <= iGz; i++)
    {
      HighOffset[i] = NextHighOffset[i]; // had to wait until ShowProgress done
    }
  } // keep going
} // PullBracketOut

void MPUOffset::SetAveraging(int16_t NewN)
{
  N = NewN;
  Serial.print(F("averaging "));
  Serial.print(N);
  Serial.println(F(" readings each time"));
} // SetAveraging

void MPUOffset::calibrate()
{
  Initialize();

  Offset[iAx] = accelgyro.getXAccelOffset();
  Offset[iAy] = accelgyro.getYAccelOffset();
  Offset[iAz] = accelgyro.getZAccelOffset();
  Offset[iGx] = accelgyro.getXGyroOffset();
  Offset[iGy] = accelgyro.getYGyroOffset();
  Offset[iGz] = accelgyro.getZGyroOffset();

  saveConfig(Offset);

} // setup

String MPUOffset::getStatus()
{
  return statustext;
}