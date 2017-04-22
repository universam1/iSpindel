/**************************************************************
   LOXONE.cpp 
   access LOXONE server by http

 **************************************************************/



#ifndef _LOXONE_H_
#define _LOXONE_H_

#include <ESP8266WiFi.h>


#define CONNTIMEOUT 3000

class LoxoneHttp {
 public:
    LoxoneHttp(char* device, char* server, uint16_t port, char* userpass, char* url);
    bool sendHTTP();
    void add(char *variable_id, float value);
    ~LoxoneHttp();
    
 private:
    char* _device;
    char* _server;
    char* _userpass;
    char* _url;
    uint16_t _port;
    uint8_t maxValues;
    uint8_t currentValue;
    float * val;
    WiFiClient _client;
};

#endif
