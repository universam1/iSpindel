/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#ifndef _SENDER_H_
#define _SENDER_H_

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

class SenderClass
{
public:
  SenderClass();
  bool sendTCP(String server, uint16_t port = 80);
  bool sendGenericPost(String server, String url, uint16_t port = 80);
  bool sendGenericPostSecure(String server, String url, uint16_t port = 443, String Fingerprint = "");
  
  bool sendUbidots(String token, String name);
  bool sendFHEM(String server, uint16_t port, String name);
  bool sendTCONTROL(String server, uint16_t port);
  void add(String id, float value);
  void add(String id, String value);
  void add(String id, int32_t value);
  void add(String id, uint32_t value);
  // ~SenderClass();

private:
  WiFiClient _client;
  // StaticJsonBuffer<200> _jsonBuffer;
  DynamicJsonBuffer _jsonBuffer;
  // JsonObject data;
  JsonVariant _jsonVariant;
};

#endif
