/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/



#ifndef _TCONTROL_H_
#define _TCONTROL_H_

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>         //https://github.com/bblanchon/ArduinoJson
#include <WiFiUdp.h>

#define TCPort 4968

typedef struct TCValue {
  char  * id;
  float value_id;
} tValue;


class TControl {
 public:
    TControl(char* device, char* server, uint16_t port);
    bool sendUDP();
    void add(char *variable_id, float value);
    ~TControl();
    
 private:
    WiFiUDP Udp;
    char* _device;
    char* _server;
    uint16_t _port;
    uint8_t maxValues;
    uint8_t currentValue;
    tValue * val;
    WiFiClient _client;
};

#endif