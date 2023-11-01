/**************************************************************

    "iSpindel"
    All rights reserverd by S.Lang <universam@web.de>

 **************************************************************/

#ifndef _GLOBALS_H
#define _GLOBALS_H

#pragma once
//#define USE_DMP false
#include <Arduino.h>
#include <Hash.h>

#include <Ticker.h>

#include <I2Cdev.h>
#include <MPU6050.h>
extern Ticker flasher;

// defines go here
#define FIRMWAREVERSION "6.5.0_TGGT"

#define API_FHEM true
#define API_UBIDOTS true
#define API_GENERIC true
#define API_TCONTROL true
#define API_INFLUXDB true
#define API_PROMETHEUS true
#define API_MQTT true
#define API_THINGSPEAK true
#define API_BLYNK true
#define API_BREWBLOX true

//#define BLYNK_DEBUG
//#define APP_DEBUG
//#define BLYNK_PRINT Serial
#define BLYNK_NO_BUILTIN
#define BLYNK_NO_FANCY_LOGO
#define BLYNK_MAX_SENDBYTES 1200

#ifndef DEBUG
#define DEBUG true
#endif

#ifdef NO_CONSOLE
#define CONSOLE(x) \
    do             \
    {              \
    } while (0)
#define CONSOLELN CONSOLE
#define CONSOLEF CONSOLE
#else
#define CONSOLE(...)               \
    do                             \
    {                              \
        Serial.print(__VA_ARGS__); \
    } while (0)
#define CONSOLELN(...)               \
    do                               \
    {                                \
        Serial.println(__VA_ARGS__); \
    } while (0)
#endif

#define PORTALTIMEOUT 300

#define ADCDIVISOR 191.8
#define ONE_WIRE_BUS D6 // DS18B20 on ESP pin12
#define OW_PINS \
    (const uint8_t[]) { D1, D6 }
#define RESOLUTION 12 // 12bit resolution == 750ms update rate
#define OWinterval (760 / (1 << (12 - RESOLUTION)))
#define CFGFILE "/config.json"
#define TKIDSIZE 40
#define DNSSIZE 253

#define MEDIANROUNDSMAX 49
#define MEDIANROUNDSMIN 29
#define MEDIANAVRG MEDIANROUNDSMIN
#define MEDIAN_MAX_SIZE MEDIANROUNDSMAX

#define CBP_ENDPOINT "/api/hydrometer/v1/data"

#define DTUbiDots 0
#define DTThingspeak 1
#define DTCraftBeerPi 2
#define DTHTTP 3
#define DTTcontrol 4
#define DTFHEM 5
#define DTTCP 6
#define DTiSPINDELde 7
#define DTInfluxDB 8
#define DTPrometheus 9
#define DTMQTT 10
#define DTTHINGSPEAK 11
#define DTBLYNK 12
#define DTBREWBLOX 13

// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 1
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

#define WIFIENADDR 1
#define RTCVALIDFLAG 0xCAFEBABE

// sleep management
#define RTCSLEEPADDR 5
#define MAXSLEEPTIME 3600UL //TODO
#define EMERGENCYSLEEP (myData.my_sleeptime * 3 < MAXSLEEPTIME ? MAXSLEEPTIME : myData.my_sleeptime * 3)
#define LOWBATT 3.3

#define UNINIT 0

#define TEMP_CELSIUS 0
#define TEMP_FAHRENHEIT 1
#define TEMP_KELVIN 2

//BMP280
#define PRESS_PA 0
#define PRESS_KPA 1
#define PRESS_BAR 2
#define PRESS_FTLB 3
//BMP280

extern int16_t ax, ay, az;
extern float Volt, Temperatur, Tilt, Gravity, accTemp, TempAdjusted;
extern float GComp;
extern String strTComp;
extern float CTemp;

extern MPU6050 accelgyro;
extern bool saveConfig();
extern bool saveConfig(int16_t Offset[6]);
extern bool formatSpiffs();
extern void flash();

extern float scaleTemperatureFromC(float t, uint8_t tempscale);
extern String tempScaleLabel(void);
//extern uint8_t my_tempscale;

#define TEMPCOMP_NO 0
#define TEMPCOMP_YES 1
#define TEMPCOMP_ERR 2

struct iData
{
  char my_token[TKIDSIZE * 2];
  char my_name[TKIDSIZE] = "iSpindelDH[SG]";            //TGGT
  char my_server[DNSSIZE] = "log.brewfather.net";       //TGGT
  char my_uri[DNSSIZE] = "/ispindel?id=VizLHF6c1JOvJB"; //TGGT
  char my_db[TKIDSIZE] = "ispindel";
  char my_username[TKIDSIZE];
  char my_password[TKIDSIZE];
  char my_job[TKIDSIZE] = "ispindel";
  char my_instance[TKIDSIZE] = "000";
  char my_polynominal[1000] = "(1.0000E-6*(tilt^3))+(-1.3137E-4*(tilt^2))+(6.9680E-3*tilt)+(9.0704E-1)";
  uint8_t my_tcomp = TEMPCOMP_ERR;
  String strTComp = "err";
  float my_ctemp = 20.0;
  String my_ssid = "noharmdone";    //TGGT
  String my_psk = "dock-brief-odd"; //TGGT
  uint8_t my_api = DTHTTP;          //TGGT
  uint32_t my_sleeptime = 15 * 60;
  uint16_t my_port = 80;
  uint32_t my_channel;
  float my_vfact = ADCDIVISOR;
  int16_t my_Offset[6];
  uint8_t my_tempscale = TEMP_CELSIUS;
  int8_t my_OWpin = -1;
  char my_tempcalc[1000] = "temp+0";
};

extern iData myData;

//BMP280
extern double Pressure;
//BMP280

#endif
