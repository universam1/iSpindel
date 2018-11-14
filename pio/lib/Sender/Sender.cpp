/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#include "Sender.h"
#include "Globals.h"
#include <PubSubClient.h>

#define UBISERVER "things.ubidots.com"
#define BREWERSFRIENDSERVER "log.brewersfriend.com"
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

bool SenderClass::sendMQTT(String server, uint16_t port, String username, String password, String name)
{
    _mqttClient.setClient(_client);
    _mqttClient.setServer(server.c_str(), port);
    _mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length) { this->mqttCallback(topic, payload, length); });

    byte i = 0;

    while (!_mqttClient.connected() && (i < 3))
    {
        CONSOLELN(F("Attempting MQTT connection"));
        // Attempt to connect
        if (_mqttClient.connect(name.c_str(), username.c_str(), password.c_str()))
        {
            CONSOLELN(F("Connected to MQTT"));
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
    //MQTT publish values
    for (const auto &kv : _jsonVariant.as<JsonObject>())
    {
        CONSOLELN("MQTT publish: ispindel/" + name + "/" + kv.key + "/" + kv.value.as<String>());
        _mqttClient.publish(("ispindel/" + name + "/" + kv.key).c_str(), kv.value.as<String>().c_str());
        _mqttClient.loop(); //This should be called regularly to allow the client to process incoming messages and maintain its connection to the server.
    }

    CONSOLELN(F("Closing MQTT connection"));
    _mqttClient.disconnect();
    delay(100); // allow gracefull session close
    return true;
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
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {
        CONSOLELN(F("\nSender: TCP stream"));
        _jsonVariant.printTo(_client);
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
    _client.stop();
    delay(50); // allow gracefull session close
    return response;
}

bool SenderClass::sendGenericPost(String server, String url, uint16_t port)
{
    _jsonVariant.printTo(Serial);

    HTTPClient http;

    CONSOLELN(F("HTTPAPI: posting"));
    // configure traged server and url
    http.begin(server, port, url);
    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "application/json");

    String json;
    _jsonVariant.printTo(json);
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
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendInfluxDB(String server, uint16_t port, String db, String name, String username, String password)
{
    HTTPClient http;

    String uri = "/write?db=";
    uri += db;

    CONSOLELN(String(F("INFLUXDB: posting to db: ")) + uri);
    // configure traged server and url
    http.begin(server, port, uri);

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

    for (const auto &kv : _jsonVariant.as<JsonObject>())
    {
        msg += kv.key;
        msg += "=";
        // check if value is of type char, if so set value in double quotes
        if ( kv.value.is<char*>() ) {
            msg += '"' + kv.value.as<String>() + '"';
        }else{
            msg += kv.value.as<String>();
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
    delay(100); // allow gracefull session close
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
    // configure traged server and url
    http.begin(server, port, uri);
    http.addHeader("User-Agent", "iSpindel");
    http.addHeader("Connection", "close");
    http.addHeader("Content-Type", "text/plain");

    String msg;

    //Build up the data for the Prometheus Pushgateway
    //A gauge is a metric that represents a single numerical value that can arbitrarily go up and down.
    for (const auto &kv : _jsonVariant.as<JsonObject>())
    {
        msg += "# TYPE ";
        msg += kv.key;
        msg += " gauge\n";
        msg += "# HELP ";
        msg += kv.key;
        msg += " The approximate value of ";
        msg += kv.key;
        msg += ".\n";
        msg += kv.key;
        msg += " ";
        msg += kv.value.as<String>();
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
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendUbidots(String token, String name)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(UBISERVER, 80))
    {
        CONSOLELN(F("\nSender: Ubidots posting"));

        String msg = F("POST /api/v1.6/devices/");
        msg += name;
        msg += "?token=";
        msg += token;
        msg += F(" HTTP/1.1\r\nHost: things.ubidots.com\r\nUser-Agent: ESP8266\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ");
        msg += _jsonVariant.measureLength();
        msg += "\r\n";

        _client.println(msg);
        _jsonVariant.printTo(_client);
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
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendFHEM(String server, uint16_t port, String name)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {
        CONSOLELN(F("\nSender: FHEM get"));

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
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendTCONTROL(String server, uint16_t port)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(server.c_str(), port))
    {

        CONSOLELN(F("\nSender: TCONTROL"));
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
    delay(100); // allow gracefull session close
    return true;
}

bool SenderClass::sendBrewersfriend(String token, String name)
{
    _jsonVariant.printTo(Serial);

    if (_client.connect(BREWERSFRIENDSERVER, 80))
    {
        CONSOLELN(F("\nSender: Brewersfriend posting"));

        String msg = F("POST /ispindel/");
        msg += token;
        msg += F(" HTTP/1.1\r\nHost: log.brewersfriend.com\r\nUser-Agent: ESP8266\r\nConnection: close\r\nContent-Type: application/json\r\nContent-Length: ");
        msg += _jsonVariant.measureLength();
        msg += "\r\n";

        _client.println(msg);
        _jsonVariant.printTo(_client);
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
    delay(100); // allow gracefull session close
    return true;
}
