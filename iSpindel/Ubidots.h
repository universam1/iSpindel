/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

/*
Copyright (c) 2013-2016 Ubidots.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Made by Mateo Velez - Metavix for Ubidots Inc

*/

#ifndef _Ubidots_H_
#define _Ubidots_H_

#include <ESP8266WiFi.h>
#include "WiFiUdp.h"
#include <ArduinoJson.h>         //https://github.com/bblanchon/ArduinoJson
#define JSONARRAY 200

#define TIME_SERVER (PSTR("pool.ntp.org"))

#define SERVER "translate.ubidots.com"
#define PORT 9010

#define HTTPSERVER "things.ubidots.com"
#define HTTPPORT 80
#define USER_AGENT "ESP8266"
#define VERSION "1.0"

typedef struct Value {
  char  *id;
  char *context;
  float value_id;
  unsigned long timestamp;
} Value;

class Ubidots {
 public:
    Ubidots(char* token, char* agent = USER_AGENT, char* server = SERVER);
    bool sendAll(bool type = false);
    bool sendHTTP();
    bool sendTLATE();
    float getValue(char* id);
    void add(char *variable_id, float value);
    void add(char *variable_id, float value, char *ctext);
    void add(char *variable_id, float value, unsigned long timestamp);
    void add(char *variable_id, float value, char *ctext, unsigned long timestamp);
    bool wifiConnection(char *ssid, char *pass);
    void setDataSourceName(char* dataSoruceName);
    void setDataSourceLabel(char* dataSoruceLabel);
    unsigned long ntpUnixTime ();

 private:
    char* _agent;
    char* _token;
    char* _server;
    char* _dsName;
    char* _dsTag;
    uint8_t maxValues;
    uint8_t currentValue;
    Value * val;
    float parseValue(String body);
    WiFiClient _client;
    WiFiUDP udp;
};

#endif