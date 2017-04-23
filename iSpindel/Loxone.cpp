/**************************************************************
   LOXONE.cpp 
   access LOXONE server by http

 **************************************************************/

// http://admin:password@192.168.1.10/dev/sps/io/VTI1/iSpindel%2011.5%2011.4%2013

#include "Loxone.h"

LoxoneHttp::LoxoneHttp( char* device, char* server, uint16_t port, char* userpass, char* url) {
    _device = device;
    _server = server;
    _port = port;
    _userpass = userpass;
    _url = url;
    
    maxValues = 5;
    currentValue = 0;
    val = (float *)malloc(maxValues*sizeof(float));
}
LoxoneHttp::~LoxoneHttp() {
  free(val);
}

void LoxoneHttp::add(char *variable_id, float value) {
   // (val+currentValue)->id = variable_id;
    *(val+currentValue) = value;
    currentValue++;
}

bool LoxoneHttp::sendHTTP() {
    uint16_t i;
    String msg;
    

    if (_client.connect(_server, _port)) {
        Serial.println(F("Getting your variables"));

        msg = String("GET ");
        msg += String(_url);
        msg += String(_device);
        for (i = 0; i < currentValue;i++ ) {
          msg += String("%20");
          msg += String(*(val+i));
        }
        
        msg += String(" HTTP/1.1\r\nHost: ");
        msg += String(_server);
        msg += String(":");
        msg += String(_port);
        msg += String("\r\nAuthorization: Basic ");
        msg += String(_userpass);
        msg += String("\r\nUser-Agent: ");
        msg += String(_device);
        msg += String("\r\nConnection: close\r\n");

        _client.println(msg);
        
        Serial.println(msg);

    }
    int timeout = 0;
    while(!_client.available() && timeout < CONNTIMEOUT) {
        timeout++;
        delay(1);
    }
    while (_client.available()) {
        char c = _client.read();
        Serial.write(c);
    }
    currentValue = 0;
    _client.stop();
    return true;
}
