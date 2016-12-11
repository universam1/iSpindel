/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/


#include "genericHTTP.h"

genericHTTP::genericHTTP( char* device, char* server, uint16_t port, char* url) {
    _device = device;
    _server = server;
    _port = port;
    _url = url;
    maxValues = 5;
    currentValue = 0;
    val = (gValue *)malloc(maxValues*sizeof(gValue));
}

genericHTTP::~genericHTTP() { free(val); }

void genericHTTP::add(char *variable_id, float value) {
    (val+currentValue)->id = variable_id;
    (val+currentValue)->value_id = value;
    currentValue++;
}

bool genericHTTP::sendHTTP() {
    uint16_t i;
    String msg;
    
    StaticJsonBuffer<JSONARRAY> jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();

    data["name"] = _device;

    for (i = 0; i < currentValue; ) {
        data[String((val + i)->id)] = (val+i)->value_id;
        i++;
    }

    if (_client.connect(_server, _port)) {
        Serial.println(F("genericHTTP: posting"));

        msg = String("POST ");
        msg += String(_url);
        msg += String(F(" HTTP/1.1\r\nHost: "));
        msg += String(_server);
        msg += String(F("\r\nUser-Agent: "));
        msg += String(_device);
        msg += String(F("\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: "));
        msg += String(data.measureLength());
        //msg += String("\r\n");

        _client.println(msg);
        data.printTo(_client);
        _client.println();
        
        Serial.println(msg);
        data.printTo(Serial);

    } else {
        Serial.println(F("\nERROR genericHTTP: couldnt connect"));
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