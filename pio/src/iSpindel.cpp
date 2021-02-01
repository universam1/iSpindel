/**************************************************************

"iSpindel"
All rights reserverd by S.Lang <universam@web.de>

**************************************************************/

// includes go here
#include <PubSubClient.h>
#include "Globals.h"
#include "MPUOffset.h"
// #endif
#include "OneWire.h"
#include "Wire.h"
// #include <Ticker.h>
#include "DallasTemperature.h"
#include "DoubleResetDetector.h" // https://github.com/datacute/DoubleResetDetector
#include "RunningMedian.h"
#include "WiFiManagerKT.h"
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <FS.h>          //this needs to be first
#include "tinyexpr.h"

#include "Sender.h"
// !DEBUG 1

// definitions go here
MPU6050 accelgyro;
OneWire *oneWire;
DallasTemperature DS18B20;
DeviceAddress tempDeviceAddress;
Ticker flasher;
RunningMedian samples = RunningMedian(MEDIANROUNDSMAX);
DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

#define TEMP_CELSIUS 0
#define TEMP_FAHRENHEIT 1
#define TEMP_KELVIN 2

int detectTempSensor(const uint8_t pins[]);
bool testAccel();

#ifdef USE_DMP
#include "MPU6050.h"

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;        // [w, x, y, z]         quaternion container
VectorInt16 aa;      // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;  // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld; // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity; // [x, y, z]            gravity vector
float euler[3];      // [psi, theta, phi]    Euler angle container
float ypr[3];        // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
#endif

bool shouldSaveConfig = false;

char my_token[TKIDSIZE * 2];
char my_name[TKIDSIZE] = "iSpindel000";
char my_server[DNSSIZE];
char my_uri[DNSSIZE];
char my_db[TKIDSIZE] = "ispindel";
char my_username[TKIDSIZE];
char my_password[TKIDSIZE];
char my_job[TKIDSIZE] = "ispindel";
char my_instance[TKIDSIZE] = "000";
char my_polynominal[250] = "-0.00031*tilt^2+0.557*tilt-14.054";

String my_ssid;
String my_psk;
uint8_t my_api;
uint32_t my_sleeptime = 15 * 60;
uint16_t my_port = 80;
uint32_t my_channel;
float my_vfact = ADCDIVISOR;
int16_t my_Offset[6];
uint8_t my_tempscale = TEMP_CELSIUS;
int8_t my_OWpin = -1;

uint32_t DSreqTime = 0;

int16_t ax, ay, az;
float Volt, Temperatur, Tilt, Gravity; // , corrGravity;

float scaleTemperature(float t)
{
  if (my_tempscale == TEMP_CELSIUS)
    return t;
  else if (my_tempscale == TEMP_FAHRENHEIT)
    return (1.8f * t + 32);
  else if (my_tempscale == TEMP_KELVIN)
    return t + 273.15f;
  else
    return t; // Invalid value for my_tempscale => default to celsius
}

String tempScaleLabel(void)
{
  if (my_tempscale == TEMP_CELSIUS)
    return "C";
  else if (my_tempscale == TEMP_FAHRENHEIT)
    return "F";
  else if (my_tempscale == TEMP_KELVIN)
    return "K";
  else
    return "C"; // Invalid value for my_tempscale => default to celsius
}

// callback notifying us of the need to save config
void saveConfigCallback()
{
  shouldSaveConfig = true;
}

void applyOffset()
{
  if (my_Offset[0] != UNINIT && my_Offset[1] != UNINIT && my_Offset[2] != UNINIT)
  {
    CONSOLELN(String("applying offsets: ") + my_Offset[0] + ":" + my_Offset[1] + ":" + my_Offset[2]);
    accelgyro.setXAccelOffset(my_Offset[0]);
    accelgyro.setYAccelOffset(my_Offset[1]);
    accelgyro.setZAccelOffset(my_Offset[2]);
    accelgyro.setXGyroOffset(my_Offset[3]);
    accelgyro.setYGyroOffset(my_Offset[4]);
    accelgyro.setZGyroOffset(my_Offset[5]);
    delay(1);

    CONSOLELN(String("confirming offsets: ") + accelgyro.getXAccelOffset() + ":" + accelgyro.getYAccelOffset() + ":" + accelgyro.getZAccelOffset());
  }
  else
    CONSOLELN(F("offsets not available"));
}

bool readConfig()
{
  CONSOLE(F("mounting FS..."));

  if (!SPIFFS.begin())
  {
    CONSOLELN(F(" ERROR: failed to mount FS!"));
    return false;
  }
  else
  {
    CONSOLELN(F(" mounted!"));
    if (!SPIFFS.exists(CFGFILE))
    {
      CONSOLELN(F("ERROR: failed to load json config"));
      return false;
    }
    else
    {
      // file exists, reading and loading
      CONSOLELN(F("reading config file"));
      File configFile = SPIFFS.open(CFGFILE, "r");
      if (!configFile)
      {
        CONSOLELN(F("ERROR: unable to open config file"));
      }
      else
      {
        size_t size = configFile.size();
        DynamicJsonDocument doc(size * 3);
        DeserializationError error = deserializeJson(doc, configFile);
        if (error)
        {
          CONSOLE(F("deserializeJson() failed: "));
          CONSOLELN(error.c_str());
        }
        else
        {
          if (doc.containsKey("Name"))
            strcpy(my_name, doc["Name"]);
          if (doc.containsKey("Token"))
            strcpy(my_token, doc["Token"]);
          if (doc.containsKey("Server"))
            strcpy(my_server, doc["Server"]);
          if (doc.containsKey("Sleep"))
            my_sleeptime = doc["Sleep"];
          if (doc.containsKey("API"))
            my_api = doc["API"];
          if (doc.containsKey("Port"))
            my_port = doc["Port"];
          if (doc.containsKey("Channel"))
            my_channel = doc["Channel"];
          if (doc.containsKey("URI"))
            strcpy(my_uri, doc["URI"]);
          if (doc.containsKey("DB"))
            strcpy(my_db, doc["DB"]);
          if (doc.containsKey("Username"))
            strcpy(my_username, doc["Username"]);
          if (doc.containsKey("Password"))
            strcpy(my_password, doc["Password"]);
          if (doc.containsKey("Job"))
            strcpy(my_job, doc["Job"]);
          if (doc.containsKey("Instance"))
            strcpy(my_instance, doc["Instance"]);
          if (doc.containsKey("Vfact"))
            my_vfact = doc["Vfact"];
          if (doc.containsKey("TS"))
            my_tempscale = doc["TS"];
          if (doc.containsKey("OWpin"))
            my_OWpin = doc["OWpin"];
          if (doc.containsKey("SSID"))
            my_ssid = (const char *)doc["SSID"];
          if (doc.containsKey("PSK"))
            my_psk = (const char *)doc["PSK"];
          if (doc.containsKey("POLY"))
            strcpy(my_polynominal, doc["POLY"]);

          if (doc.containsKey("Offset"))
          {
            for (size_t i = 0; i < (sizeof(my_Offset) / sizeof(*my_Offset)); i++)
            {
              my_Offset[i] = doc["Offset"][i];
            }
          }

          CONSOLELN(F("parsed config:"));
#ifdef DEBUG
          serializeJson(doc, Serial);
          CONSOLELN();
#endif
        }
      }
    }
  }
  return true;
}

bool shouldStartConfig(bool validConf)
{

  // we make sure that configuration is properly set and we are not woken by
  // RESET button
  // ensure this was called

  rst_info *_reset_info = ESP.getResetInfoPtr();
  uint8_t _reset_reason = _reset_info->reason;

  // The ESP reset info is sill buggy. see http://www.esp8266.com/viewtopic.php?f=32&t=8411
  // The reset reason is "5" (woken from deep-sleep) in most cases (also after a power-cycle)
  // I added a single reset detection as workaround to enter the config-mode easier
  CONSOLE(F("Boot-Mode: "));
  CONSOLELN(ESP.getResetReason());
  bool _poweredOnOffOn = _reset_reason == REASON_DEFAULT_RST || _reset_reason == REASON_EXT_SYS_RST;
  if (_poweredOnOffOn)
    CONSOLELN(F("power-cycle or reset detected, config mode"));

  bool _dblreset = drd.detectDoubleReset();
  if (_dblreset)
    CONSOLELN(F("\nDouble Reset detected"));

  bool _wifiCred = (WiFi.SSID() != "");
  uint8_t c = 0;
  if (!_wifiCred)
    WiFi.begin();
  while (!_wifiCred)
  {
    if (c > 10)
      break;
    CONSOLE('.');
    delay(100);
    c++;
    _wifiCred = (WiFi.SSID() != "");
  }
  if (!_wifiCred)
    CONSOLELN(F("\nERROR no Wifi credentials"));

  if (validConf && !_dblreset && _wifiCred && !_poweredOnOffOn)
  {
    CONSOLELN(F("\nwoken from deepsleep, normal mode"));
    return false;
  }
  // config mode
  else
  {
    CONSOLELN(F("\ngoing to Config Mode"));
    return true;
  }
}

void validateInput(const char *input, char *output)
{
  String tmp = input;
  tmp.trim();
  tmp.replace(' ', '_');
  tmp.toCharArray(output, tmp.length() + 1);
}

String htmlencode(String str)
{
  String encodedstr = "";
  char c;

  for (uint16_t i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);

    if (isalnum(c))
    {
      encodedstr += c;
    }
    else
    {
      encodedstr += "&#";
      encodedstr += String((uint8_t)c);
      encodedstr += ';';
    }
  }
  return encodedstr;
}

bool startConfiguration()
{

  WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(PORTALTIMEOUT);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setBreakAfterConfig(true);

  WiFiManagerParameter api_list(HTTP_API_LIST);
  WiFiManagerParameter custom_api("selAPI", "selAPI", String(my_api).c_str(),
                                  20, TYPE_HIDDEN, WFM_NO_LABEL);

  WiFiManagerParameter custom_name("name", "iSpindel Name", htmlencode(my_name).c_str(),
                                   TKIDSIZE * 2);
  WiFiManagerParameter custom_sleep("sleep", "Update Interval (s)",
                                    String(my_sleeptime).c_str(), 6, TYPE_NUMBER);
  WiFiManagerParameter custom_token("token", "Token", htmlencode(my_token).c_str(),
                                    TKIDSIZE * 2 * 2);
  WiFiManagerParameter custom_server("server", "Server Address",
                                     my_server, DNSSIZE);
  WiFiManagerParameter custom_port("port", "Server Port",
                                   String(my_port).c_str(), TKIDSIZE,
                                   TYPE_NUMBER);
  WiFiManagerParameter custom_channel("channel", "Channelnumber",
                                      String(my_channel).c_str(), TKIDSIZE, TYPE_NUMBER);
  WiFiManagerParameter custom_uri("uri", "Path / URI", my_uri, DNSSIZE);
  WiFiManagerParameter custom_db("db", "InfluxDB db", my_db, TKIDSIZE);
  WiFiManagerParameter custom_username("username", "Username", my_username, TKIDSIZE);
  WiFiManagerParameter custom_password("password", "Password", my_password, TKIDSIZE);
  WiFiManagerParameter custom_job("job", "Prometheus job", my_job, TKIDSIZE);
  WiFiManagerParameter custom_instance("instance", "Prometheus instance", my_instance, TKIDSIZE);
  WiFiManagerParameter custom_vfact("vfact", "Battery conversion factor",
                                    String(my_vfact).c_str(), 7, TYPE_NUMBER);
  WiFiManagerParameter tempscale_list(HTTP_TEMPSCALE_LIST);
  WiFiManagerParameter custom_tempscale("tempscale", "tempscale",
                                        String(my_tempscale).c_str(),
                                        5, TYPE_HIDDEN, WFM_NO_LABEL);

  wifiManager.addParameter(&custom_name);
  wifiManager.addParameter(&custom_sleep);
  wifiManager.addParameter(&custom_vfact);

  WiFiManagerParameter custom_tempscale_hint("<label for=\"TS\">Unit of temperature</label>");
  wifiManager.addParameter(&custom_tempscale_hint);
  wifiManager.addParameter(&tempscale_list);
  wifiManager.addParameter(&custom_tempscale);
  WiFiManagerParameter custom_api_hint("<hr><label for=\"API\">Service Type</label>");
  wifiManager.addParameter(&custom_api_hint);

  wifiManager.addParameter(&api_list);
  wifiManager.addParameter(&custom_api);

  wifiManager.addParameter(&custom_token);
  wifiManager.addParameter(&custom_server);
  wifiManager.addParameter(&custom_port);
  wifiManager.addParameter(&custom_channel);
  wifiManager.addParameter(&custom_uri);
  wifiManager.addParameter(&custom_db);
  wifiManager.addParameter(&custom_username);
  wifiManager.addParameter(&custom_password);
  wifiManager.addParameter(&custom_job);
  wifiManager.addParameter(&custom_instance);
  WiFiManagerParameter custom_polynom_lbl("<hr><label for=\"POLYN\">Gravity conversion<br/>ex. \"-0.00031*tilt^2+0.557*tilt-14.054\"</label>");
  wifiManager.addParameter(&custom_polynom_lbl);
  WiFiManagerParameter custom_polynom("POLYN", "Polynominal", htmlencode(my_polynominal).c_str(), 250 * 2, WFM_NO_LABEL);
  wifiManager.addParameter(&custom_polynom);

  wifiManager.setConfSSID(htmlencode(my_ssid));
  wifiManager.setConfPSK(htmlencode(my_psk));

  CONSOLELN(F("started Portal"));
  wifiManager.startConfigPortal("iSpindel");

  strcpy(my_polynominal, custom_polynom.getValue());

  validateInput(custom_name.getValue(), my_name);
  validateInput(custom_token.getValue(), my_token);
  validateInput(custom_server.getValue(), my_server);
  validateInput(custom_db.getValue(), my_db);
  validateInput(custom_username.getValue(), my_username);
  validateInput(custom_password.getValue(), my_password);
  validateInput(custom_job.getValue(), my_job);
  validateInput(custom_instance.getValue(), my_instance);
  my_sleeptime = String(custom_sleep.getValue()).toInt();

  my_api = String(custom_api.getValue()).toInt();
  my_port = String(custom_port.getValue()).toInt();
  my_channel = String(custom_channel.getValue()).toInt();
  my_tempscale = String(custom_tempscale.getValue()).toInt();
  validateInput(custom_uri.getValue(), my_uri);

  String tmp = custom_vfact.getValue();
  tmp.trim();
  tmp.replace(',', '.');
  my_vfact = tmp.toFloat();
  if (my_vfact < ADCDIVISOR * 0.8 || my_vfact > ADCDIVISOR * 1.25)
    my_vfact = ADCDIVISOR;

  // save the custom parameters to FS
  if (shouldSaveConfig)
  {
    // Wifi config
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);

    return saveConfig();
  }
  return false;
}

bool formatSpiffs()
{
  CONSOLE(F("\nneed to format SPIFFS: "));
  SPIFFS.end();
  SPIFFS.begin();
  CONSOLELN(SPIFFS.format());
  return SPIFFS.begin();
}

bool saveConfig(int16_t Offset[6])
{
  std::copy(Offset, Offset + 6, my_Offset);
  CONSOLELN(String("new offsets: ") + Offset[0] + ":" + Offset[1] + ":" + Offset[2]);
  CONSOLELN(String("confirming offsets: ") + accelgyro.getXAccelOffset() + ":" + accelgyro.getYAccelOffset() + ":" + accelgyro.getZAccelOffset());

  return saveConfig();
}

bool saveConfig()
{
  CONSOLE(F("saving config...\n"));

  // if SPIFFS is not usable
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    if (!formatSpiffs())
    {
      Serial.println("Failed to format file system - hardware issues!");
      return false;
    }
  }

  DynamicJsonDocument doc(2048);

  doc["Name"] = my_name;
  doc["Token"] = my_token;
  doc["Sleep"] = my_sleeptime;
  // first reboot is for test
  my_sleeptime = 1;
  doc["Server"] = my_server;
  doc["API"] = my_api;
  doc["Port"] = my_port;
  doc["Channel"] = my_channel;
  doc["URI"] = my_uri;
  doc["DB"] = my_db;
  doc["Username"] = my_username;
  doc["Password"] = my_password;
  doc["Job"] = my_job;
  doc["Instance"] = my_instance;
  doc["Vfact"] = my_vfact;
  doc["TS"] = my_tempscale;
  doc["OWpin"] = my_OWpin;
  doc["POLY"] = my_polynominal;
  doc["SSID"] = WiFi.SSID();
  doc["PSK"] = WiFi.psk();

  JsonArray array = doc.createNestedArray("Offset");
  for (auto &&i : my_Offset)
  {
    array.add(i);
  }

  File configFile = SPIFFS.open(CFGFILE, "w");
  if (!configFile)
  {
    CONSOLELN(F("failed to open config file for writing"));
    SPIFFS.end();
    return false;
  }
  else
  {
    serializeJson(doc, configFile);
#ifdef DEBUG
    serializeJson(doc, Serial);
#endif
    configFile.flush();
    configFile.close();
    SPIFFS.gc();
    SPIFFS.end();
    CONSOLELN(F("\nsaved successfully"));
    return true;
  }
}

bool processResponse(String response)
{
  DynamicJsonDocument doc(1024);

  DeserializationError error = deserializeJson(doc, response);
  if (!error && doc.containsKey("interval"))
  {
    uint32_t interval = doc["interval"];
    if (interval != my_sleeptime &&
        interval < 24 * 60 * 60 &&
        interval > 10)
    {
      my_sleeptime = interval;
      CONSOLE(F("Received new Interval config: "));
      CONSOLELN(interval);
      return saveConfig();
    }
  }
  return false;
}

bool uploadData(uint8_t service)
{
  SenderClass sender;

#ifdef API_UBIDOTS
  if (service == DTUbiDots)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling Ubidots"));
    return sender.sendUbidots(my_token, my_name);
  }
#endif

#ifdef API_MQTT
  if (service == DTMQTT)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("temp_units", tempScaleLabel());
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling MQTT"));
    return sender.sendMQTT(my_server, my_port, my_username, my_password, my_name);
  }
#endif

#ifdef API_THINGSPEAK
  if (service == DTTHINGSPEAK)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("temp_units", tempScaleLabel());
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling ThingSpeak"));
    return sender.sendThingSpeak(my_token, my_channel);
  }
#endif

#ifdef API_INFLUXDB
  if (service == DTInfluxDB)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("temp_units", tempScaleLabel());
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling InfluxDB"));
    CONSOLELN(String(F("Sending to db: ")) + my_db + String(F(" w/ credentials: ")) + my_username + String(F(":")) + my_password);
    return sender.sendInfluxDB(my_server, my_port, my_db, my_name, my_username, my_password);
  }
#endif

#ifdef API_PROMETHEUS
  if (service == DTPrometheus)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", Temperatur);
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling Prometheus Pushgateway"));
    return sender.sendPrometheus(my_server, my_port, my_job, my_instance);
  }
#endif

#ifdef API_GENERIC
  if ((service == DTHTTP) || (service == DTCraftBeerPi) || (service == DTiSPINDELde) || (service == DTTCP))
  {

    sender.add("name", my_name);
    sender.add("ID", ESP.getChipId());
    if (my_token[0] != 0)
      sender.add("token", my_token);
    sender.add("angle", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("temp_units", tempScaleLabel());
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());

    if (service == DTHTTP)
    {
      CONSOLELN(F("\ncalling HTTP"));
      return sender.sendGenericPost(my_server, my_uri, my_port);
    }
    else if (service == DTCraftBeerPi)
    {
      CONSOLELN(F("\ncalling CraftbeerPi"));
      return sender.sendGenericPost(my_server, CBP_ENDPOINT, 5000);
    }
    else if (service == DTiSPINDELde)
    {
      CONSOLELN(F("\ncalling iSPINDELde"));
      return sender.sendTCP("ispindle.de", 9501);
    }
    else if (service == DTTCP)
    {
      CONSOLELN(F("\ncalling TCP"));
      String response = sender.sendTCP(my_server, my_port);
      return processResponse(response);
    }
  }
#endif // DATABASESYSTEM

#ifdef API_FHEM
  if (service == DTFHEM)
  {
    sender.add("angle", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("temp_units", tempScaleLabel());
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("ID", ESP.getChipId());
    CONSOLELN(F("\ncalling FHEM"));
    return sender.sendFHEM(my_server, my_port, my_name);
  }
#endif // DATABASESYSTEM ==
#ifdef API_TCONTROL
  if (service == DTTcontrol)
  {
    sender.add("T", scaleTemperature(Temperatur));
    sender.add("D", Tilt);
    sender.add("U", Volt);
    sender.add("G", Gravity);
    CONSOLELN(F("\ncalling TCONTROL"));
    return sender.sendTCONTROL(my_server, 4968);
  }
#endif // DATABASESYSTEM ==

#ifdef API_BLYNK
  if (service == DTBLYNK)
  {
    String tempToSend = String( scaleTemperature( Temperatur ), 1 );
    sender.add("20", tempToSend);           //send temperature without the unit to the graph first
    String voltToSend = String(Volt, 2);
    sender.add("30", voltToSend);           //send temperature without the unit to the graph first

    tempToSend += "°";
    tempToSend += tempScaleLabel();         // Add temperature unit to the String

    sender.add("1", String(Tilt, 1)+"°");
    sender.add("2", tempToSend);
    sender.add("3", voltToSend+"V");
    sender.add("4", String(Gravity, 3));
    return sender.sendBlynk(my_token);
  }
#endif

#ifdef API_BREWBLOX
  if (service == DTBREWBLOX)
  {
    sender.add("Tilt[deg]", Tilt);
    sender.add("Temperature[deg" + tempScaleLabel() + "]", scaleTemperature(Temperatur));
    sender.add("Battery[V]", Volt);
    sender.add("Gravity", Gravity);
    sender.add("Rssi[dBm]", WiFi.RSSI());
    CONSOLELN(F("\ncalling BREWBLOX"));
    return sender.sendBrewblox(my_server, my_port, my_uri, my_username, my_password, my_name);
  }
#endif
  return false;
}

void goodNight(uint32_t seconds)
{

  uint32_t _seconds = seconds;
  uint32_t left2sleep = 0;
  uint32_t validflag = RTCVALIDFLAG;

  drd.stop();

  // workaround for DS not floating
  pinMode(my_OWpin, OUTPUT);
  digitalWrite(my_OWpin, LOW);

  // we need another incarnation before work run
  if (_seconds > MAXSLEEPTIME)
  {
    left2sleep = _seconds - MAXSLEEPTIME;
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR + 1, &validflag, sizeof(validflag));
    CONSOLELN(String(F("\nStep-sleep: ")) + MAXSLEEPTIME + "s; left: " + left2sleep + "s; RT: " + millis());
    ESP.deepSleep(MAXSLEEPTIME * 1e6, WAKE_RF_DISABLED);
  }
  // regular sleep with RF enabled after wakeup
  else
  {
    // clearing RTC to mark next wake
    left2sleep = 0;
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR + 1, &validflag, sizeof(validflag));
    CONSOLELN(String(F("\nFinal-sleep: ")) + _seconds + "s; RT: " + millis());
    // WAKE_RF_DEFAULT --> auto reconnect after wakeup
    ESP.deepSleep(_seconds * 1e6, WAKE_RF_DEFAULT);
  }
  // workaround proper power state init
  delay(500);
}
void sleepManager()
{
  uint32_t left2sleep, validflag;
  ESP.rtcUserMemoryRead(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));
  ESP.rtcUserMemoryRead(RTCSLEEPADDR + 1, &validflag, sizeof(validflag));

  // check if we have to incarnate again
  if (left2sleep != 0 && !drd.detectDoubleReset() && validflag == RTCVALIDFLAG)
  {
    goodNight(left2sleep);
  }
  else
  {
    CONSOLELN(F("Worker run!"));
  }
}

void requestTemp()
{
  if (!DSreqTime)
  {
    DS18B20.requestTemperatures();
    DSreqTime = millis();
  }
}

void initDS18B20()
{
  if (my_OWpin == -1)
  {
    my_OWpin = detectTempSensor(OW_PINS);
    if (my_OWpin == -1)
    {
      CONSOLELN(F("ERROR: cannot find a OneWire Temperature Sensor!"));
      return;
    }
  }
  pinMode(my_OWpin, OUTPUT);
  digitalWrite(my_OWpin, LOW);
  delay(100);
  oneWire = new OneWire(my_OWpin);
  DS18B20 = DallasTemperature(oneWire);
  DS18B20.begin();

  bool device = DS18B20.getAddress(tempDeviceAddress, 0);
  if (!device)
  {
    my_OWpin = detectTempSensor(OW_PINS);
    if (my_OWpin == -1)
    {
      CONSOLELN(F("ERROR: cannot find a OneWire Temperature Sensor!"));
      return;
    }
    else
    {
      delete oneWire;
      oneWire = new OneWire(my_OWpin);
      DS18B20 = DallasTemperature(oneWire);
      DS18B20.begin();
      DS18B20.getAddress(tempDeviceAddress, 0);
    }
  }

  DS18B20.setWaitForConversion(false);
  DS18B20.setResolution(tempDeviceAddress, RESOLUTION);
  requestTemp();
}

bool isDS18B20ready()
{
  return millis() - DSreqTime > OWinterval;
}

void initAccel()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin(D3, D4);
  Wire.setClock(100000);
  Wire.setClockStretchLimit(2 * 230);

  testAccel();
  // init the Accel
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  accelgyro.setDLPFMode(MPU6050_DLPF_BW_5);
  accelgyro.setTempSensorEnabled(true);
#ifdef USE_DMP
  accelgyro.setDMPEnabled(true);
  packetSize = accelgyro.dmpGetFIFOPacketSize();
#endif
  accelgyro.setInterruptLatch(0); // pulse
  accelgyro.setInterruptMode(1);  // Active Low
  accelgyro.setInterruptDrive(1); // Open drain
  accelgyro.setRate(17);
  accelgyro.setIntDataReadyEnabled(true);
  applyOffset();
}

float calculateTilt()
{
  float _ax = ax;
  float _ay = ay;
  float _az = az;
  return acos(_az / (sqrt(_ax * _ax + _ay * _ay + _az * _az))) * 180.0 / M_PI;
}

bool testAccel()
{
  uint8_t res = Wire.status();
  if (res != I2C_OK)
    CONSOLELN(String(F("I2C ERROR: ")) + res);

  bool con = accelgyro.testConnection();
  if (!con)
    CONSOLELN(F("Acc Test Connection ERROR!"));

  return res == I2C_OK && con == true;
}

void getAccSample()
{
  accelgyro.getAcceleration(&ax, &az, &ay);
}

float getTilt()
{
  uint32_t start = millis();
  uint8_t i = 0;

  for (; i < MEDIANROUNDSMAX; i++)
  {
    while (!accelgyro.getIntDataReadyStatus())
      delay(2);
    getAccSample();
    float _tilt = calculateTilt();
    samples.add(_tilt);

    if (i >= MEDIANROUNDSMIN && isDS18B20ready())
      break;
  }
  CONSOLE("Samples:");
  CONSOLE(++i);
  CONSOLE(" min:");
  CONSOLE(samples.getLowest());
  CONSOLE(" max:");
  CONSOLE(samples.getHighest());
  CONSOLE(" time:");
  CONSOLELN(millis() - start);
  return samples.getAverage(MEDIANAVRG);
}

float getTemperature(bool block = false)
{
  float t = Temperatur;

  // we need to wait for DS18b20 to finish conversion
  if (!DSreqTime ||
      (!block && !isDS18B20ready()))
    return t;

  // if we need the result we have to block
  while (!isDS18B20ready())
    delay(10);
  DSreqTime = 0;

  t = DS18B20.getTempCByIndex(0);

  if (t == DEVICE_DISCONNECTED_C || // DISCONNECTED
      t == 85.0)                    // we read 85 uninitialized
  {
    CONSOLELN(F("ERROR: OW DISCONNECTED"));
    pinMode(my_OWpin, OUTPUT);
    digitalWrite(my_OWpin, LOW);
    delay(100);
    oneWire->reset();

    CONSOLELN(F("OW Retry"));
    initDS18B20();
    delay(OWinterval);
    t = getTemperature(false);
  }

  return t;
}

int detectTempSensor(const uint8_t pins[])
{

  for (uint8_t p = 0; p < sizeof(pins); p++)
  {
    const byte pin = pins[p];
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius;

    CONSOLE(F("scanning for OW device on pin: "));
    CONSOLELN(pin);
    OneWire ds(pin);

    if (!ds.search(addr))
    {
      CONSOLELN(F("No devices found!"));
      ds.reset_search();
      delay(250);
      continue;
    }

    CONSOLE("Found device with ROM =");
    for (i = 0; i < 8; i++)
    {
      CONSOLE(' ');
      CONSOLE(addr[i], HEX);
    }

    if (OneWire::crc8(addr, 7) != addr[7])
    {
      CONSOLELN(" CRC is not valid!");
      continue;
    }
    CONSOLELN();

    // the first ROM byte indicates which chip
    switch (addr[0])
    {
    case 0x10:
      CONSOLELN("  Chip = DS18S20"); // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      CONSOLELN("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      CONSOLELN("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      CONSOLELN("Device is not a DS18x20 family device.");
      continue;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1); // start conversion, with parasite power on at the end

    delay(900); // maybe 750ms is enough, maybe not
    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE); // Read Scratchpad

    CONSOLE("  Data = ");
    CONSOLE(present, HEX);
    CONSOLE(" ");
    for (i = 0; i < 9; i++)
    { // we need 9 bytes
      data[i] = ds.read();
      CONSOLE(data[i], HEX);
      CONSOLE(" ");
    }
    CONSOLE(" CRC=");
    CONSOLELN(OneWire::crc8(data, 8), HEX);

    // Convert the data to actual temperature
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s)
    {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10)
      {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    }
    else
    {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00)
        raw = raw & ~7; // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20)
        raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40)
        raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    CONSOLE(F("  Temperature = "));
    CONSOLE(celsius);
    CONSOLELN(F(" Celsius, "));
    return pin;
  }
  return -1;
}

float getBattery()
{
  analogRead(A0); // drop first read
  return analogRead(A0) / my_vfact;
}

float calculateGravity()
{
  double _tilt = Tilt;
  double _temp = Temperatur;
  float _gravity = 0;
  int err;
  te_variable vars[] = {{"tilt", &_tilt}, {"temp", &_temp}};
  te_expr *expr = te_compile(my_polynominal, vars, 2, &err);

  if (expr)
  {
    _gravity = te_eval(expr);
    te_free(expr);
  }
  else
  {
    CONSOLELN(String(F("Parse error at ")) + err);
  }
  return _gravity;
}

void flash()
{
  // triggers the LED
  Volt = getBattery();
  if (testAccel())
    getAccSample();
  Tilt = calculateTilt();
  Temperatur = getTemperature(false);
  Gravity = calculateGravity();
  requestTemp();
}

bool isSafeMode(float _volt)
{
  if (_volt < LOWBATT)
  {
    CONSOLELN(F("\nWARNING: low Battery"));
    return true;
  }
  else
    return false;
}

bool connectBackupCredentials()
{
  WiFi.disconnect();
  WiFi.begin(my_ssid.c_str(), my_psk.c_str());
  CONSOLELN(F("Rescued Wifi credentials"));

  CONSOLE(F("   -> waited for "));
  unsigned long startedAt = millis();
  // int connRes = WiFi.waitForConnectResult();
  uint8_t wait = 0;
  while (WiFi.status() == WL_DISCONNECTED)
  {
    delay(200);
    wait++;
    if (wait > 50)
      break;
  }
  auto waited = (millis() - startedAt);
  CONSOLE(waited);
  CONSOLE(F("ms, result "));
  CONSOLELN(WiFi.status());

  if (WiFi.status() == WL_DISCONNECTED)
    return false;
  else
    return true;
}

void setup()
{

  Serial.begin(115200);

  CONSOLELN(F("\nFW " FIRMWAREVERSION));
  CONSOLELN(ESP.getSdkVersion());

  sleepManager();

  bool validConf = readConfig();
  if (!validConf)
    CONSOLELN(F("\nERROR config corrupted"));
  initDS18B20();
  initAccel();

  // decide whether we want configuration mode or normal mode
  if (shouldStartConfig(validConf))
  {
    uint32_t tmp;
    ESP.rtcUserMemoryRead(WIFIENADDR, &tmp, sizeof(tmp));

    // DIRTY hack to keep track of WAKE_RF_DEFAULT --> find a way to read WAKE_RF_*
    if (tmp != RTCVALIDFLAG)
    {
      drd.setRecentlyResetFlag();
      tmp = RTCVALIDFLAG;
      ESP.rtcUserMemoryWrite(WIFIENADDR, &tmp, sizeof(tmp));
      CONSOLELN(F("reboot RFCAL"));
      ESP.deepSleep(100000, WAKE_RFCAL);
      delay(500);
    }
    else
    {
      tmp = 0;
      ESP.rtcUserMemoryWrite(WIFIENADDR, &tmp, sizeof(tmp));
    }

    flasher.attach(1, flash);

    // rescue if wifi credentials lost because of power loss
    if (!startConfiguration())
    {
      // test if ssid exists
      if (WiFi.SSID() == "" &&
          my_ssid != "" && my_psk != "")
      {
        connectBackupCredentials();
      }
    }
    uint32_t left2sleep = 0;
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));

    flasher.detach();
  }
  // to make sure we wake up with STA but AP
  WiFi.mode(WIFI_STA);
  Volt = getBattery();
  // we try to survive
  if (isSafeMode(Volt))
    WiFi.setOutputPower(0);
  else
    WiFi.setOutputPower(20.5);

#ifndef USE_DMP
  Tilt = getTilt();
#else
  while (fifoCount < packetSize)
  {
    //do stuff
    CONSOLELN(F("wait DMP"));

    fifoCount = accelgyro.getFIFOCount();
  }
  if (fifoCount == 1024)
  {
    CONSOLELN(F("FIFO overflow"));
    accelgyro.resetFIFO();
  }
  else
  {
    fifoCount = accelgyro.getFIFOCount();

    accelgyro.getFIFOBytes(fifoBuffer, packetSize);

    accelgyro.resetFIFO();

    fifoCount -= packetSize;

    accelgyro.dmpGetQuaternion(&q, fifoBuffer);
    accelgyro.dmpGetEuler(euler, &q);

    /*
    for (int i = 1; i < 64; i++) {
    CONSOLE(fifoBuffer[i]);
    CONSOLE(" ");
    }
    */

    CONSOLE(F("euler\t"));
    CONSOLE((euler[0] * 180 / M_PI));
    CONSOLE("\t");
    CONSOLE(euler[1] * 180 / M_PI);
    CONSOLE("\t");
    CONSOLELN(euler[2] * 180 / M_PI);

    ax = euler[0];
    ay = euler[2];
    az = euler[1];

    float _ax = ax;
    float _ay = ay;
    float _az = az;
    Tilt = acos(_az / (sqrt(_ax * _ax + _ay * _ay + _az * _az))) * 180.0 / M_PI;
  }
#endif

  float accTemp = accelgyro.getTemperature() / 340.00 + 36.53;
  accelgyro.setSleepEnabled(true);

  CONSOLE("x: ");
  CONSOLE(ax);
  CONSOLE(" y: ");
  CONSOLE(ay);
  CONSOLE(" z: ");
  CONSOLELN(az);

  CONSOLE(F("Tilt: "));
  CONSOLELN(Tilt);
  CONSOLE("Tacc: ");
  CONSOLELN(accTemp);
  CONSOLE("Volt: ");
  CONSOLELN(Volt);

  // call as late as possible since DS needs converge time
  Temperatur = getTemperature(true);
  CONSOLE("Temp: ");
  CONSOLELN(Temperatur);

  // calc gravity on user defined polynominal
  Gravity = calculateGravity();
  CONSOLE(F("Gravity: "));
  CONSOLELN(Gravity);

  // water anomaly correction
  // float _temp = Temperatur - 4; // polynominal at 4
  // float wfact = 0.00005759 * _temp * _temp * _temp - 0.00783198 * _temp * _temp - 0.00011688 * _temp + 999.97;
  // corrGravity = Gravity - (1 - wfact / 1000);
  // CONSOLE(F("\tcorrGravity: "));
  // CONSOLELN(corrGravity);

  if (WiFi.status() != WL_CONNECTED)
  {
    unsigned long startedAt = millis();
    CONSOLE(F("After waiting "));
    // int connRes = WiFi.waitForConnectResult();
    uint8_t wait = 0;
    while (WiFi.status() == WL_DISCONNECTED)
    {
      delay(200);
      wait++;
      if (wait > 50)
        break;
    }
    auto waited = (millis() - startedAt);
    CONSOLE(waited);
    CONSOLE(F("ms, result "));
    CONSOLELN(WiFi.status());
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    CONSOLE(F("IP: "));
    CONSOLELN(WiFi.localIP());
    uploadData(my_api);
  }
  else
  {
    CONSOLELN(F("Failed to connect -> trying to restore connection..."));

    if (connectBackupCredentials())
      CONSOLE(F("   -> Connection restored!"));
    else
      CONSOLE(F("   -> Failed to restore connection..."));
  }

  // survive - 60min sleep time
  if (isSafeMode(Volt))
  {
    my_sleeptime = EMERGENCYSLEEP;
  }

  goodNight(my_sleeptime);
}

void loop()
{
  CONSOLELN(F("should never be here!"));
}
