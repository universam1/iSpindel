/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#include "Sender.h"
#include "Globals.h"
#include <PubSubClient.h>
#include <ThingSpeak.h>
#include <BlynkSimpleEsp8266.h> //https://github.com/blynkkk/blynk-library

#define UBISERVER "things.ubidots.com"
#define BLYNKSERVER "blynk-cloud.com"
#define CONNTIMEOUT 2000

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
void SenderClass::stopclient()
{
    _client.stop();
    delay(100); // allow gracefull session close
}

bool SenderClass::mqttConnect(const String &server, uint16_t port, const String &name, const String &username, const String &password) {
    _mqttClient.setClient(_client);
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
        response += _client.read();
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
        msg += F(" HTTP/1.1\r\nHost: things.ubidots.com\r\nUser-Agent: ESP8266\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ");
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

        for (const auto &kv : _doc.to<JsonObject>())
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

//Blynk HTTP was taking 2 seconds longer and did not show in the App
//when device was connected, therefore best to use their API.
bool SenderClass::sendBlynk(char* token)
{
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
