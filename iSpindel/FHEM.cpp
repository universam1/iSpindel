/**************************************************************
   FHEM.cpp 
   access FHEM server by http

 **************************************************************/

// http://homer:8083/fhem?cmd.Test=set%20iSpindel%2011.5%2011.4%2013&XHR=1

#include "FHEM.h"

FhemHttp::FhemHttp( char* device, char* server, uint16_t port) {
    _device = device;
    _server = server;
    _port = port;
    
    maxValues = 5;
    currentValue = 0;
    val = (float *)malloc(maxValues*sizeof(float));
}
FhemHttp::~FhemHttp() {
  free(val);
}

void FhemHttp::add(char *variable_id, float value) {
   // (val+currentValue)->id = variable_id;
    *(val+currentValue) = value;
    currentValue++;
}

bool FhemHttp::sendHTTP() {
    uint16_t i;
    String msg;
    

    if (_client.connect(_server, _port)) {
        Serial.println(F("Posting your variables"));

        msg = String("GET ");
        msg += String("/fhem?cmd.Test=set%20");
        msg += String(_device);
        for (i = 0; i < currentValue;i++ ) {
          msg += String("%20");
          msg += String(*(val+i));
        }
        

        msg += String("&XHR=1");
        msg += String(" HTTP/1.1\r\nHost: ");
        msg += String(_server);
        msg += String(":");
        msg += String(_port);
        msg += String("\r\nUser-Agent: ");
        msg += String(_device);
        msg += String("\r\nConnection: close\r\n");

        _client.println(msg);
        
        Serial.println(msg);

    }
    int timeout = 0;
    while(!_client.available() && timeout < 5000) {
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
