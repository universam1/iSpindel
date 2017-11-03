/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

// #include <Arduino.h>

#include <ESP8266WiFi.h>
// #include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>
#include "AutoUpdater.h"
// crazy PIO dependency workaround
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

struct Version
{
    uint8_t major = 0, minor = 0, revision = 0;

    Version(String version)
    {
        // sscanf(version.c_str(), "%d.%d.%d.%d", &major, &minor, &revision, &build);

        // as string
        uint8_t startindex = 0;
        uint8_t endindex = version.indexOf('.', 0);
        major = version.substring(startindex, endindex).toInt();

        startindex = endindex + 1;
        endindex = version.indexOf('.', startindex);
        minor = version.substring(startindex, endindex).toInt();

        startindex = endindex + 1;
        endindex = version.indexOf('.', startindex);
        revision = version.substring(startindex, endindex).toInt();

        Serial.printf("version: %d : %d : %d", major, minor, revision);
    }

    bool operator<(const Version &other)
    {
        if (major < other.major)
            return true;
        if (minor < other.minor)
            return true;
        if (revision < other.revision)
            return true;
        // if (build < other.build)
        // 	return true;
        return false;
    }

    bool operator==(const Version &other)
    {
        return major == other.major && minor == other.minor && revision == other.revision
            // && build == other.build
            ;
    }
};

AutoUpdaterClass::AutoUpdaterClass()
{
    String UpstreamVersion, InstalledVersion, URL;
    InstalledVersion = FIRMWAREVERSION;

    getLatestFWVersion(UpstreamVersion, URL);

    if (UpstreamVersion && URL)
    {
        if (isNewer(InstalledVersion, UpstreamVersion))
        {
            Serial.print("flashing ");
            Serial.println(UpstreamVersion);
            flash(URL);
        }
        else
        {
            Serial.println("up to date!");
        }
    }
    else
    {
        Serial.println(F("could not retrieve metadata"));
    }
}

String AutoUpdaterClass::getLatestFWVersion(String &_version, String &_URL)
{
    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    // configure traged server and url
    //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
    http.begin("https://api.github.com/repos/universam1/iSpindel/releases/latest"); //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            // String payload = http.getString();
            // Serial.println(payload);

            const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(12) + 3 * JSON_OBJECT_SIZE(17) + 3000;
            DynamicJsonBuffer jsonBuffer(bufferSize);
            // const char *json = http.getStreamPtr();
            String json = http.getString();
            JsonObject &root = jsonBuffer.parseObject(json);

            _version = (const char *)root["tag_name"];
            Serial.println(String("decoded latest FW: ") + _version);

            JsonObject &assets0 = root["assets"][0];
            _URL = (const char *)assets0["browser_download_url"];
            Serial.println(String("download: ")+ _URL);

            http.end();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
    }
}

bool AutoUpdaterClass::isNewer(String current, String upstream)
{
    // Todo: proper compare
    // return current != upstream;
    return Version(current) < Version(upstream);
}

bool AutoUpdaterClass::flash(String url)
{
    t_httpUpdate_return ret = ESPhttpUpdate.update(url);
    //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        Serial.println(F("HTTP_UPDATE_NO_UPDATES"));
        break;

    case HTTP_UPDATE_OK:
        Serial.println(F("HTTP_UPDATE_OK"));
        break;
    }
}
