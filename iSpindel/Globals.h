/**************************************************************

    "iSpindel"
    All rights reserverd by S.Lang <universam@web.de>

 **************************************************************/

#pragma once

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include <Arduino.h>
// #include <stdint.h>

// defines go here
#define FIRMWAREVERSION "4.0.0"

#define API_FHEM true
#define API_UBIDOTS true
#define API_GENERIC true
#define API_TCONTROL true

#define PORTALTIMEOUT 300

#define ADCDIVISOR 191.8
#define ONE_WIRE_BUS D6  // DS18B20 on ESP pin12
#define RESOLUTION 12 // 12bit resolution == 750ms update rate
#define OWinterval (800 / (1 << (12 - RESOLUTION))) 
#define CFGFILE "/config.json"
#define TKIDSIZE 35
#define MEDIANROUNDS 7
#define ACCINTERVAL 200
#define MEDIANAVRG 3

#define CBP_ENDPOINT (PSTR("/api/hydrometer/v1/data"))

#define DTUbiDots 0
#define DTThingspeak 1
#define DTCraftbeepPi 2
#define DTHTTP 3
#define DTTcontrol 4
#define DTFHEM  5
#define DTTCP  6

// Number of seconds after reset during which a 
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 1
// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

#define WIFIENADDR 1
#define WIFIRFENTOKEN 1234

// sleep management
#define RTCSLEEPADDR 5
#define MAXSLEEPTIME 3600 //TODO
#define EMERGENCYSLEEP (my_sleeptime*3 < MAXSLEEPTIME ? MAXSLEEPTIME : my_sleeptime*3)
#define LOWBATT 3.3

extern int16_t ax, ay, az;
extern float Volt, Temperatur, Tilt;

#endif