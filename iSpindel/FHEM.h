/**************************************************************
   FHEM.cpp 
   access FHEM server by http

 **************************************************************/



#ifndef _FHEM_H_
#define _FHEM_H_

#include <ESP8266WiFi.h>



class FhemHttp {
 public:
    FhemHttp(char* device, char* server, uint16_t port);
    bool sendHTTP();
    void add(char *variable_id, float value);
    ~FhemHttp();
    
 private:
    char* _device;
    char* _server;
   
    uint16_t _port;
    uint8_t maxValues;
    uint8_t currentValue;
    float * val;
    WiFiClient _client;
};

#endif
