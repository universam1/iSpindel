/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#include "Sender.h"

#define UBISERVER "things.ubidots.com"
#define CONNTIMEOUT 2000

SenderClass::SenderClass()
{
    _jsonVariant = _jsonBuffer.createObject();
}
void SenderClass::add(String id, float value)
{
    _jsonVariant[id] = value;
}
void SenderClass::add(String id, String value)
{
    _jsonVariant[id] = value;
}
void SenderClass::add(String id, uint32_t value)
{
    _jsonVariant[id] = value;
}
void SenderClass::add(String id, int32_t value)
{
    _jsonVariant[id] = value;
}

bool SenderClass::sendTCP(String server, uint16_t port)
// transmit data to the given server by butting a JSON string as TCP stream to the server (?)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {
        Serial.println(F("Sender: TCP stream"));
        _jsonVariant.printTo(_client);
        _client.println();
    }
    else
    {
        Serial.println(F("\nERROR Sender: couldnt connect"));
    }

    int timeout = 0;
    while (!_client.available() && timeout < CONNTIMEOUT)
    {
        timeout++;
        delay(1);
    }
    while (_client.available())
    {
        char c = _client.read();
        Serial.write(c);
    }
    // currentValue = 0;
    _client.stop();
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendGenericPost(String server, String url, uint16_t port, String fingerprint)
// transmit data to the given server by using the POST method and a JSON string
{
    _jsonVariant.printTo(Serial);

    HTTPClient http;

    // configure traged server and url
    String full_url = server + ":" + port + url;
    Serial.println("[HTTP] connecting to url: \"" + full_url + "\"");
    if (fingerprint.length() != 40) {
      // use http
      http.begin(full_url);
    } else {
      // use httpS with a 40 char fingerprint
      Serial.println("[HTTP] using https: expected fingerprint of server certificate: \"" + fingerprint+"\"");
      http.begin(full_url, fingerprint);
    }

    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "application/json");

    String json;
    _jsonVariant.printTo(json);
    auto httpCode = http.POST(json);
    Serial.println(String("code: ") + httpCode);

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            Serial.println(http.getString());
        }
    }
    else
    {
        Serial.print(F("[HTTP] POST... failed, error: "));
        Serial.println(http.errorToString(httpCode));
    }

    http.end();
}

bool SenderClass::sendUbidots(String token, String name)
// transmit data to Ubidots server by using their POST method
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(UBISERVER, 80))
    {
        Serial.println(F("Sender: Ubidots posting"));

        String msg = F("POST /api/v1.6/devices/");
        msg += name;
        msg += "?token=";
        msg += token;
        msg += F(" HTTP/1.1\r\nHost: things.ubidots.com\r\nUser-Agent: ESP8266\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ");
        msg += _jsonVariant.measureLength();
        msg += "\r\n";

        _client.println(msg);
        Serial.println(msg);

        _jsonVariant.printTo(_client);
        _client.println();
        Serial.println(msg);
    }
    else
    {
        Serial.println(F("\nERROR Sender: couldnt connect"));
    }

    int timeout = 0;
    while (!_client.available() && timeout < CONNTIMEOUT)
    {
        timeout++;
        delay(1);
    }
    while (_client.available())
    {
        char c = _client.read();
        Serial.write(c);
    }
    // currentValue = 0;
    _client.stop();
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendFHEM(String server, uint16_t port, String name)
// transmit data to local FHEM server  using their POST method
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {
        Serial.println(F("\nSender: FHEM get"));

        String msg = String("GET /fhem?cmd.Test=set%20");
        msg += name;

        for (const auto &kv : _jsonVariant.as<JsonObject>())
        {
            msg += "%20";
            msg += kv.value.as<String>();
        }

        msg += F("&XHR=1 HTTP/1.1\r\nHost: ");
        msg += server;
        msg += ":";
        msg += port;
        msg += "\r\nUser-Agent: ";
        msg += name;
        msg += F("\r\nConnection: close\r\n");

        _client.println(msg);
        Serial.println(msg);
    }
    else
    {
        Serial.println(F("\nERROR Sender: couldnt connect"));
    }

    int timeout = 0;
    while (!_client.available() && timeout < CONNTIMEOUT)
    {
        timeout++;
        delay(1);
    }
    while (_client.available())
    {
        char c = _client.read();
        Serial.write(c);
    }
    // currentValue = 0;
    _client.stop();
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendTCONTROL(String server, uint16_t port)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {

        Serial.println(F("Sender: TCONTROL"));
        String msg;

        for (const auto &kv : _jsonVariant.as<JsonObject>())
        {
            msg += kv.key;
            msg += ": ";
            msg += kv.value.as<String>();
            msg += " ";
        }
        msg.remove(msg.length() - 1);

        _client.println(msg);
        Serial.println(msg);
    }
    else
    {
        Serial.println(F("\nERROR Sender: couldnt connect"));
    }

    int timeout = 0;
    while (!_client.available() && timeout < CONNTIMEOUT)
    {
        timeout++;
        delay(1);
    }
    while (_client.available())
    {
        char c = _client.read();
        Serial.write(c);
    }
    // currentValue = 0;
    _client.stop();
    delay(100); // allow gracefull session close
    return true;
}
