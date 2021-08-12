/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#include "Sender.h"
#include "Globals.h"
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h> //https://github.com/blynkkk/blynk-library
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <MD5Builder.h>
#include <PubSubClient.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureBearSSL.h>

#define UBISERVER "industrial.api.ubidots.com"
#define BLYNKSERVER "blynk-cloud.com"
#define CONNTIMEOUT 2000
#define TIMECHECK 172800

SenderClass::SenderClass() {}

void SenderClass::add(String id, float value)
{
    _doc[id] = value;
}
void SenderClass::add(String id, String value)
{
    _doc[id] = value;
}
void SenderClass::add(String id, uint32_t value)
{
    _doc[id] = value;
}
void SenderClass::add(String id, int32_t value)
{
    _doc[id] = value;
}
String SenderClass::createMd5Hash(String str)
{
  _md5.begin();
  _md5.add(String(str));
  _md5.calculate();
  return _md5.toString();
}
void SenderClass::stopclient()
{
    _client.stop();
    _secureClient.stop();
    delay(100); // allow gracefull session close
}
bool SenderClass::RTCSyncToNTP()
{
    CONSOLE(F("Starting NTP Sync: "));
    time_t now = time(nullptr);
    if (now < TIMECHECK) {
        configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    }  
    while (now < TIMECHECK) {
        delay(500);
        CONSOLE(".");
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    CONSOLELN(""); CONSOLE(F("Current time set to: ")); CONSOLELN(asctime(&timeinfo));
    if (now > TIMECHECK){
        return true;
    }
    else {
        return false;
    }
}

bool SenderClass::mqttConnect(const String &server, uint16_t port, const String &name, const String &username, const String &password, const bool secure, const char CACert[], const char deviceCert[], const char deviceKey[]) {
    
    if (secure) {
        if (!RTCSyncToNTP()){
            CONSOLELN(F("ERROR - Time failed to be set. Secure connection will fail."));
        }
        // Configure the secure client
        BearSSL::X509List cert(CACert);
        _secureClient.setTrustAnchors(&cert);
        BearSSL::X509List client_crt(deviceCert);
        BearSSL::PrivateKey key(deviceKey);
        _secureClient.setClientRSACert(&client_crt, &key);
        _secureClient.setBufferSizes(512, 512);
        _secureClient.connect(server,port);
        // Allocate the Secure client to PubSubClient
        _mqttClient.setClient(_secureClient);
    }
    else {
        // Allocate the noraml WiFi client to the PubSubClient
        _mqttClient.setClient(_client);
    }
    _mqttClient.setServer(server.c_str(), port);
    _mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length) { this->mqttCallback(topic, payload, length); });

    byte i = 0;

    while (!_mqttClient.connected() && (i < 3))
    {
        CONSOLELN(F("Attempting MQTT connection"));
        // Attempt to connect
        boolean ret;
        if (username[0] == '\0')
        {
            ret = _mqttClient.connect(name.c_str());
        } else {
            ret = _mqttClient.connect(name.c_str(), username.c_str(), password.c_str());
        }
        if (ret)
        {
            CONSOLELN(F("Connected to MQTT"));
            return true;
        }
        else
        {
            CONSOLE(F("Failed MQTT connection, return code:"));

            int Status = _mqttClient.state();

            switch (Status)
            {
            case -4:
                CONSOLELN(F("Connection timeout"));
                break;

            case -3:
                CONSOLELN(F("Connection lost"));
                break;

            case -2:
                CONSOLELN(F("Connect failed"));
                break;

            case -1:
                CONSOLELN(F("Disconnected"));
                break;

            case 1:
                CONSOLELN(F("Bad protocol"));
                break;

            case 2:
                CONSOLELN(F("Bad client ID"));
                break;

            case 3:
                CONSOLELN(F("Unavailable"));
                break;

            case 4:
                CONSOLELN(F("Bad credentials"));
                break;

            case 5:
                CONSOLELN(F("Unauthorized"));
                break;
            }
            CONSOLELN(F("Retrying MQTT connection in 5 seconds"));
            // Wait 5 seconds before retrying
            i++;
            delay(5000);
        }
    }
    return false;
}

#if API_MQTT_HASSIO
bool SenderClass::enableHassioDiscovery(String server, uint16_t port, String username, String password, String name, String unit)
{
    bool response = mqttConnect(server, port, name, username, password);
    if (response)
    {
        _mqttClient.setBufferSize(512);
        auto chipid = String(ESP.getChipId(), HEX);
        String device = "\"dev\": { \"name\": \"" + name + "\",\"mdl\": \"ispindel\",\"sw\": \"" + FIRMWAREVERSION + "\",\"mf\": \"iSpindel\",\"ids\": [\"" + chipid + "\"]}";
        String topic = "homeassistant/sensor/iSpindel_" + chipid + "/";
        _mqttClient.publish((topic + "temperature/config").c_str(), ("{ \"uniq_id\": \"" + chipid + "_temp\", \"dev_cla\": \"temperature\", \"name\": \"Temperature\", \"unit_of_meas\": \"°" + unit + "\", \"val_tpl\": \"{{ value_json }}\", \"stat_t\": \"ispindel/" + name + "/temperature\"," + device + "}").c_str(), true);
        _mqttClient.publish((topic + "tilt/config").c_str(), ("{ \"uniq_id\": \"" + chipid + "_tilt\", \"name\": \"Tilt\", \"val_tpl\": \"{{ value_json }}\", \"stat_t\": \"ispindel/" + name + "/tilt\"," + device + "}").c_str(), true);
        _mqttClient.publish((topic + "battery/config").c_str(), ("{ \"uniq_id\": \"" + chipid + "_battery\", \"dev_cla\": \"voltage\", \"name\": \"Battery voltage\", \"unit_of_meas\": \"V\", \"val_tpl\": \"{{ value_json }}\", \"stat_t\": \"ispindel/" + name + "/battery\"," + device + "}").c_str(), true);
        _mqttClient.publish((topic + "rssi/config").c_str(), ("{ \"uniq_id\": \"" + chipid + "_rssi\", \"dev_cla\": \"signal_strength\", \"name\": \"Signal Strength\", \"unit_of_meas\": \"dB\", \"val_tpl\": \"{{ value_json }}\", \"stat_t\": \"ispindel/" + name + "/RSSI\"," + device + "}").c_str(), true);
        _mqttClient.publish((topic + "gravity/config").c_str(), ("{ \"uniq_id\": \"" + chipid + "_gravity\", \"name\": \"Gravity\", \"unit_of_meas\": \"°P\", \"val_tpl\": \"{{ value_json }}\", \"stat_t\": \"ispindel/" + name + "/gravity\"," + device + "}").c_str(), true);
        _mqttClient.loop();
    }

    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    stopclient();
    return response;
}

bool SenderClass::disableHassioDiscovery(String server, uint16_t port, String username, String password, String name)
{
    bool response = mqttConnect(server, port, name, username, password);
    if (response)
    {
        auto chipid = String(ESP.getChipId(), HEX);
        String topic = "homeassistant/sensor/iSpindel_" + chipid + "/";
        _mqttClient.publish((topic + "temperature/config").c_str(), "");
        _mqttClient.publish((topic + "tilt/config").c_str(), "");
        _mqttClient.publish((topic + "battery/config").c_str(), "");
        _mqttClient.publish((topic + "rssi/config").c_str(), "");
        _mqttClient.publish((topic + "gravity/config").c_str(), "");
        _mqttClient.loop();
    }
    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    stopclient();
    return response;
}
#endif

bool SenderClass::sendMQTT(String server, uint16_t port, String username, String password, String name)
{
    bool response = mqttConnect(server, port, name, username, password);
    if (response)
    {
        //MQTT publish values
        for (const auto &kv : _doc.as<JsonObject>())
        {
           CONSOLELN("MQTT publish: ispindel/" + name + "/" + kv.key().c_str() + "/" + kv.value().as<String>());
           _mqttClient.publish(("ispindel/" + name + "/" + kv.key().c_str()).c_str(), kv.value().as<String>().c_str());
           _mqttClient.loop(); //This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
        }
    }

    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    stopclient();
    return response;
}

void SenderClass::mqttCallback(char *topic, byte *payload, unsigned int length)
{
    CONSOLELN(F("MQTT message arrived ["));
    CONSOLELN(topic);
    CONSOLELN(F("] "));
    for (unsigned int i = 0; i < length; i++)
    {
        CONSOLE((char)payload[i]);
    }
}

bool SenderClass::sendSecureMQTT(char CACert[], char deviceCert[], char deviceKey[], String server, uint16_t port, String name, String topic)    //AWS
{
    bool response = mqttConnect(server, port, name, "", "", true, CACert, deviceCert, deviceKey);

    if (response)
    {
        String json;
        serializeJson(_doc, json);
        CONSOLELN("MQTT publish: " + topic);
        CONSOLELN(("{\"state\":{\"reported\":" + json  + "}, \"key\":\"" + name + "\"}").c_str());
        _mqttClient.publish(topic.c_str(), ("{\"state\":{\"reported\":" + json  + "}, \"key\":\"" + name + "\"}").c_str());
    }
    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    stopclient();
    return response;

}

String SenderClass::sendTCP(String server, uint16_t port)
{
    int timeout = 0;
    String response;
    serializeJson(_doc, Serial);

    if (_client.connect(server.c_str(), port))
    {
        CONSOLELN(F("\nSender: TCP stream"));
        serializeJson(_doc, _client);
        _client.println();
    }
    else
    {
        CONSOLELN(F("\nERROR Sender: couldnt connect"));
    }

    while (!_client.available() && timeout < CONNTIMEOUT)
    {
        timeout++;
        delay(1);
    }
    while (_client.available())
    {
        response += (char)_client.read();
    }
    CONSOLELN(response);
    stopclient();
    return response;
}

bool SenderClass::sendThingSpeak(String token, long Channel)
{
    int field = 0;
    unsigned long channelNumber = Channel;
    const char * writeAPIKey = token.c_str();

    serializeJson(_doc, Serial);
    ThingSpeak.begin(_client);

    CONSOLELN(F("\nSender: ThingSpeak posting"));

    for (const auto &kv : _doc.as<JsonObject>())
    {
        field++;
        ThingSpeak.setField(field, kv.value().as<String>());
    }
    // write to the ThingSpeak channel
    int x = ThingSpeak.writeFields(channelNumber, writeAPIKey);

    if(x == 200){
     Serial.println("Channel update successful.");
    }
    else{
     Serial.println("Problem updating channel. HTTP error code " + String(x));
     return false;
    }
    _client.stop();
    stopclient();
    return true;
    }

bool SenderClass::sendHTTPSPost(String server, String uri)
{
    String url = server + uri;   
    serializeJson(_doc, Serial);

    String json;
    serializeJson(_doc, json);
    
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;
    if(https.begin(*client, url) )
    {   
        // CONSOLELN(json);
        https.addHeader("Content-Type", "application/json");
        int httpCode = https.POST(json);
        

        if (httpCode > 0) 
        {
            if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
            {
            CONSOLELN(F("Should be connected..."));
            String payload = https.getString();
            CONSOLELN(payload);
            
            }
        
            else
            {
            CONSOLE(F("Connection failed Code ") );
            CONSOLELN(httpCode);
            }
        }
      https.end();
    }
    else
    {
        CONSOLELN(F("Connection failed"));
    }

    
    stopclient();
    return true;
}

bool SenderClass::sendGenericPost(String server, String uri, uint16_t port)
{
    serializeJson(_doc, Serial);
    HTTPClient http;

    CONSOLELN(F("HTTPAPI: posting"));
    // configure traged server and uri
    http.begin(_client, server, port, uri);
    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "application/json");

    String json;
    serializeJson(_doc, json);
    auto httpCode = http.POST(json);
    CONSOLELN(String(F("code: ")) + httpCode);

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            CONSOLELN(http.getString());
        }
    }
    else
    {
        CONSOLE(F("[HTTP] POST... failed, error: "));
        CONSOLELN(http.errorToString(httpCode));
    }

    http.end();
    stopclient();
    return true;
}

bool SenderClass::sendInfluxDB(String server, uint16_t port, String db, String name, String username, String password)
{
    HTTPClient http;

    String uri = "/write?db=";
    uri += db;

    CONSOLELN(String(F("INFLUXDB: posting to db: ")) + uri);
    // configure traged server and uri
    http.begin(_client, server, port, uri);

    if (username.length() > 0)
    {
        http.setAuthorization(username.c_str(), password.c_str());
    }

    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String msg;
    msg += "measurements,source=";
    msg += name;
    msg += " ";

    for (const auto &kv : _doc.as<JsonObject>())
    {
        msg += kv.key().c_str();
        msg += "=";
        // check if value is of type char, if so set value in double quotes
        if (kv.value().is<char *>())
        {
            msg += '"' + kv.value().as<String>() + '"';
        }
        else
        {
            msg += kv.value().as<String>();
        }
        msg += ",";
    }
    msg.remove(msg.length() - 1);

    CONSOLELN(String(F("POST data: ")) + msg);

    auto httpCode = http.POST(msg);
    CONSOLELN(String(F("code: ")) + httpCode);

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            CONSOLELN(http.getString());
        }
    }
    else
    {
        CONSOLE(F("[HTTP] POST... failed, error: "));
        CONSOLELN(http.errorToString(httpCode));
    }

    http.end();
    stopclient();
    return true;
}

bool SenderClass::sendPrometheus(String server, uint16_t port, String job, String instance)
{
    HTTPClient http;

    // the path looks like /metrics/job/<JOBNAME>[/instance/<INSTANCENAME>]
    String uri = "/metrics/job/";
    uri += job;
    uri += "/instance/";
    uri += instance;

    CONSOLELN(String("PROMETHEUS: posting to Prometheus Pushgateway: ") + uri);
    // configure traged server and uri
    http.begin(_client, server, port, uri);
    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "text/plain");

    String msg;

    //Build up the data for the Prometheus Pushgateway
    //A gauge is a metric that represents a single numerical value that can arbitrarily go up and down.
    for (const auto &kv : _doc.as<JsonObject>())
    {
        msg += "# TYPE ";
        msg += kv.key().c_str();
        msg += " gauge\n";
        msg += "# HELP ";
        msg += kv.key().c_str();
        msg += " The approximate value of ";
        msg += kv.key().c_str();
        msg += ".\n";
        msg += kv.key().c_str();
        msg += " ";
        msg += kv.value().as<String>();
        msg += "\n";
    }

    CONSOLELN(String("POST data: ") + msg);
    auto httpCode = http.POST(msg);

    // httpCode will be 202 on success
    if (httpCode == 202)
    {
        CONSOLELN(String("code: ") + httpCode);
        CONSOLELN(http.getString());
    }
    else
    {
        CONSOLELN(String("code: ") + httpCode);
        CONSOLELN(http.errorToString(httpCode));
        CONSOLELN(http.getString());
    }

    http.end();
    stopclient();
    return true;
}

bool SenderClass::sendUbidots(String token, String name)
{
    serializeJson(_doc, Serial);

    if (_client.connect(UBISERVER, 80))
    {
        CONSOLELN(F("\nSender: Ubidots posting"));

        String msg = F("POST /api/v1.6/devices/");
        msg += name;
        msg += "?token=";
        msg += token;
        msg += F(" HTTP/1.1\r\nHost: ");
        msg += UBISERVER;
        msg += F("\r\nUser-Agent: ESP8266\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ");
        msg += measureJson(_doc);
        msg += "\r\n";

        _client.println(msg);
        serializeJson(_doc, _client);
        _client.println();
        CONSOLELN(msg);
    }
    else
    {
        CONSOLELN(F("\nERROR Sender: couldnt connect"));
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
    stopclient();
    return true;
}

bool SenderClass::sendFHEM(String server, uint16_t port, String name)
{
    serializeJson(_doc, Serial);

    if (_client.connect(server.c_str(), port))
    {
        CONSOLELN(F("\nSender: FHEM get"));

        String msg = String("GET /fhem?cmd.Test=set%20");
        msg += name;

        for (const auto &kv : _doc.as<JsonObject>())
        {
            msg += "%20";
            msg += kv.value().as<String>();
        }

        msg += F("&XHR=1 HTTP/1.1\r\nHost: ");
        msg += server;
        msg += ":";
        msg += port;
        msg += "\r\nUser-Agent: ";
        msg += name;
        msg += F("\r\nConnection: close\r\n");

        _client.println(msg);
        CONSOLELN(msg);
    }
    else
    {
        CONSOLELN(F("\nERROR Sender: couldnt connect"));
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
    stopclient();
    return true;
}

bool SenderClass::sendTCONTROL(String server, uint16_t port)
{
    serializeJson(_doc, Serial);

    if (_client.connect(server.c_str(), port))
    {

        CONSOLELN(F("\nSender: TCONTROL"));
        String msg;

        for (const auto &kv : _doc.to<JsonObject>())
        {
            msg += kv.key().c_str();
            msg += ": ";
            msg += kv.value().as<String>();
            msg += " ";
        }
        msg.remove(msg.length() - 1);

        _client.println(msg);
        CONSOLELN(msg);
    }
    else
    {
        CONSOLELN(F("\nERROR Sender: couldnt connect"));
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
    stopclient();
    return true;
}

bool SenderClass::sendBlynk(char* token)
{
    //Blynk HTTP was taking 2 seconds longer and did not show in the App
    //when device was connected, therefore best to use their API.

    serializeJson(_doc, Serial);

    Blynk.config(token);

    byte i = 0;
    int _pin = 0;
    String _value;

    while (!Blynk.connected() && i<100)
    {
      Blynk.run();
      i++;
      delay(50);
    }

    if (Blynk.connected())
    {
        CONSOLELN(F("\nConnected to the Blynk server, sending data"));

        for (const auto &kv : _doc.as<JsonObject>())
        {
            _pin = atoi(kv.key().c_str());
            _value = kv.value().as<String>();
            Blynk.virtualWrite(_pin, _value);
        }
    }

    else {
        CONSOLELN(F("\nFailed to connect to Blynk, going to sleep"));
        return false;
    }

    delay(150);     //delay to allow last value to be sent;
    return true;
}

bool SenderClass::sendBrewblox(String server, uint16_t port, String topic, String username, String password, String name)
{
    bool response = mqttConnect(server, port, name, username, password);
    if (response)
    {
        String json;
        serializeJson(_doc, json);
        CONSOLELN("Brewblox MQTT publish: " + topic);
        _mqttClient.publish(topic.c_str(), ("{\"key\":\"" + name + "\",\"data\":" + json  + "}").c_str());
    }
    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    stopclient();
    return response;
}

uint32_t SenderClass::sendBricks()
{
  uint32_t next_request_ms = 60 * 1000;

  // dump json
  CONSOLELN(F("sendBricks called"));

  serializeJson(_doc, Serial);

  CONSOLELN(F("setting insecure"));
  _secureClient.setInsecure(); // unfortunately necessary, ESP8266 does not support SSL without hard coding certificates
  String url = "https://brewbricks.com/api/iot/v1";
  _secureClient.connect(url, 443);
  CONSOLELN(F("adding headers"));
  HTTPClient http; //Declare an object of class HTTPClient

  // configure traged server and uri
  http.begin(_secureClient, url);
  http.addHeader("User-Agent", "iSpindel");
  http.addHeader("Connection", "close");
  http.addHeader("Content-Type", "application/json");

  String json;
  CONSOLELN(F("serializing json"));
  serializeJson(_doc, json);
  CONSOLE(F("starting POST, payload: "));
  CONSOLELN(json);

  auto httpCode = http.POST(json);
  CONSOLELN(String(F("code: ")) + httpCode);

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString(); //Get the request response payload
        CONSOLE(F("Received response: "));
        CONSOLELN(payload);

        // this whole section below is to spare the JSON parser
        uint8 startIdx = payload.indexOf("next_request_ms") + 17;
        String nrmSubstring = payload.substring(startIdx);
        uint8 endIdx = -1;
        if (nrmSubstring.indexOf("}") != -1)
        {
            // end based on bracket
            endIdx = nrmSubstring.indexOf("}") + startIdx;
        }
        else if (nrmSubstring.indexOf(",") != -1)
        {
            // end based on comma, other value following
            endIdx = nrmSubstring.indexOf(",") + startIdx;
        }

        if (startIdx > 0 && endIdx > startIdx)
        {
            CONSOLE(F("next request string in "));
            String next_request_str = payload.substring(startIdx, endIdx);
            CONSOLE(next_request_str);
            CONSOLE(F("ms"));
            next_request_ms = next_request_str.toInt();
        }
    }
    }
    else
    {
      CONSOLE(F("[HTTP] POST... failed, error: "));
      CONSOLELN(http.errorToString(httpCode));
    }

    http.end();
    stopclient();
    return next_request_ms;
}