#include "MPUOffset.h"

const char LBRACKET = '[';
const char RBRACKET = ']';
const char COMMA = ',';

const int iAx = 0;
const int iAy = 1;
const int iAz = 2;
const int iGx = 3;
const int iGy = 4;
const int iGz = 5;

const int NFast = 1000;  // the bigger, the better (but slower)
const int NSlow = 10000; // ..
const int LinesBetweenHeaders = 5;
int LowValue[6];
int HighValue[6];
int Smoothed[6];
int LowOffset[6];
int HighOffset[6];
int Target[6];
int LinesOut;
int N;
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
  int Offset[6];

  SetOffsets(Offset);
} // Initialize

void MPUOffset::SetOffsets(int TheOffsets[6])
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

void MPUOffset::SetAveraging(int NewN)
{
  N = NewN;
  Serial.print(F("averaging "));
  Serial.print(N);
  Serial.println(F(" readings each time"));
} // SetAveraging

void MPUOffset::calibrate()
{
  boolean StillWorking;
  int NewOffset[6];
  boolean AllBracketsNarrow;

  uint32_t starttime = millis();

  Initialize();
  for (int i = iAx; i <= iGz; i++)
  {                // set targets and initial guesses
    Target[i] = 0; // must fix for ZAccel
    HighOffset[i] = 0;
    LowOffset[i] = 0;
  } // set targets and initial guesses
  Target[iAz] = 16384;
  SetAveraging(NFast);

  Serial.println(F("expanding:"));
  ForceHeader();
  PullBracketsOut();

  Serial.println(F("\nclosing in:"));
  AllBracketsNarrow = false;
  ForceHeader();
  StillWorking = true;
  while (StillWorking)
  {
    statustext = String(F("calibrating... [")) + String((millis() - starttime) / 1000) + "s]";
    StillWorking = false;
    if (AllBracketsNarrow && (N == NFast))
    {
      SetAveraging(NSlow);
    }
    else
    {
      AllBracketsNarrow = true;
    } // tentative
    for (int i = iAx; i <= iGz; i++)
    {
      if (HighOffset[i] <= (LowOffset[i] + 1))
      {
        NewOffset[i] = LowOffset[i];
      }
      else
      { // binary search
        StillWorking = true;
        NewOffset[i] = (LowOffset[i] + HighOffset[i]) / 2;
        if (HighOffset[i] > (LowOffset[i] + 10))
        {
          AllBracketsNarrow = false;
        }
      } // binary search
    }
    SetOffsets(NewOffset);
    GetSmoothed();
    for (int i = iAx; i <= iGz; i++)
    { // closing in
      if (Smoothed[i] > Target[i])
      { // use lower half
        HighOffset[i] = NewOffset[i];
        HighValue[i] = Smoothed[i];
      } // use lower half
      else
      { // use upper half
        LowOffset[i] = NewOffset[i];
        LowValue[i] = Smoothed[i];
      } // use upper half
    }   // closing in
    ShowProgress();
  } // still working
  Serial.println(F("-------------- done --------------"));
  statustext = "done!";
  accelgyro.getAcceleration(&ax, &ay, &az);
  Serial.print(F("a/g:\t"));
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  Serial.println();

  saveConfig(NewOffset[iAx], NewOffset[iAy], NewOffset[iAz]);

} // setup

String MPUOffset::getStatus()
{
  return statustext;
}