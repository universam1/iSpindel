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
#define FIRMWAREVERSION "6.0.4"

#define API_FHEM true
#define API_UBIDOTS true
#define API_GENERIC true
#define API_TCONTROL true
#define API_INFLUXDB true
#define API_PROMETHEUS true
#define API_MQTT true
#define API_BLYNK true

//#define BLYNK_DEBUG
#define APP_DEBUG
#define BLYNK_PRINT Serial
#define BLYNK_NO_BUILTIN
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
#define DTBLYNK 11

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
#define EMERGENCYSLEEP (my_sleeptime * 3 < MAXSLEEPTIME ? MAXSLEEPTIME : my_sleeptime * 3)
#define LOWBATT 3.3

#define UNINIT 0

extern int16_t ax, ay, az;
extern float Volt, Temperatur, Tilt, Gravity;
extern int16_t my_aX, my_aY, my_aZ;

extern MPU6050_Base accelgyro;
extern bool saveConfig();
extern void formatSpiffs();

float scaleTemperature(float t);
String tempScaleLabel(void);

#endif
