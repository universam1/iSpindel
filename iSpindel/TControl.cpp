/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/
#include "TControl.h"

TControl::TControl( char* device, char* server, uint16_t port) {
    
    _device = device;
    _server = server;
    _port = port;
    maxValues = 5;
    currentValue = 0;
    val = (tValue *)malloc(maxValues*sizeof(tValue));
}

TControl::~TControl() { free(val); }

void TControl::add(char *variable_id, float value) {
    (val+currentValue)->id = variable_id;
    (val+currentValue)->value_id = value;
    currentValue++;
}

bool TControl::sendUDP() {
    uint16_t i;
    String msg;

    for (i = 0; i < currentValue; ) {
        msg += String((val + i)->id);
        msg += ": ";
        msg += String((val+i)->value_id);
        i++;
        if (i < currentValue) msg += " ";
    }

    Serial.println(F("TConctrol UDP: sending"));

    Udp.beginPacket(_server, TCPort);
    Udp.print(msg);
    Udp.endPacket();

    return true;
}