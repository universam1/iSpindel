/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/



#ifndef _GENHTTP_H_
#define _GENHTTP_H_

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>         //https://github.com/bblanchon/ArduinoJson

#define JSONARRAY 150
#define CONNTIMEOUT 3000

typedef struct genValue {
  char  * id;
  float value_id;
} gValue;


class genericHTTP {
 public:
    genericHTTP(char* device, char* server, uint16_t port, char* url);
    bool sendHTTP();
    void add(char *variable_id, float value);
    ~genericHTTP();
    
 private:
    char* _device;
    char* _server;
    char* _url;
    uint16_t _port;
    uint8_t maxValues;
    uint8_t currentValue;
    gValue * val;
    WiFiClient _client;
};

#endif