/**************************************************************

"iSpindel"
All rights reserverd by S.Lang <universam@web.de>

**************************************************************/

// includes go here
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
MPU6050_Base accelgyro;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DeviceAddress tempDeviceAddress;
Ticker flasher;
RunningMedian samples = RunningMedian(MEDIANROUNDS);

#define TEMP_CELSIUS 0
#define TEMP_FAHRENHEIT 1
#define TEMP_KELVIN 2

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

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

char my_token[TKIDSIZE];
char my_name[TKIDSIZE] = "iSpindel000";
char my_server[TKIDSIZE];
char my_url[TKIDSIZE];
char my_db[TKIDSIZE] = "ispindel";
char my_job[TKIDSIZE] = "ispindel";
char my_instance[TKIDSIZE] = "000";
char my_polynominal[70] = "-0.00031*tilt^2+0.557*tilt-14.054";

String my_ssid;
String my_psk;
uint8_t my_api;
uint32_t my_sleeptime = 15 * 60;
uint16_t my_port = 80;
float my_vfact = ADCDIVISOR;
int16_t my_aX = UNINIT, my_aY = UNINIT, my_aZ = UNINIT;
uint8_t my_tempscale = TEMP_CELSIUS;

uint32_t DSreqTime;
float pitch, roll;

int16_t ax, ay, az;
float Volt, Temperatur, Tilt, Gravity; // , corrGravity;

bool DSrequested = false;

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

// callback notifying us of the need to save config
void saveConfigCallback()
{
  shouldSaveConfig = true;
}

void applyOffset()
{
  if (my_aX != UNINIT && my_aY != UNINIT && my_aZ != UNINIT)
  {
    CONSOLELN(F("applying offsets"));
    accelgyro.setXAccelOffset(my_aX);
    accelgyro.setYAccelOffset(my_aY);
    accelgyro.setZAccelOffset(my_aZ);
  }
  else
    CONSOLELN(F("offsets not available"));
}

bool readConfig()
{
  CONSOLE(F("mounting FS..."));

  if (SPIFFS.begin())
  {
    CONSOLELN(F(" mounted!"));
    if (SPIFFS.exists(CFGFILE))
    {
      // file exists, reading and loading
      CONSOLELN(F("reading config file"));
      File configFile = SPIFFS.open(CFGFILE, "r");
      if (configFile)
      {
        CONSOLELN(F("opened config file"));
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());

        if (json.success())
        {
          CONSOLELN(F("\nparsed json"));

          if (json.containsKey("Name"))
            strcpy(my_name, json["Name"]);
          if (json.containsKey("Token"))
            strcpy(my_token, json["Token"]);
          if (json.containsKey("Server"))
            strcpy(my_server, json["Server"]);
          if (json.containsKey("Sleep"))
            my_sleeptime = json["Sleep"];
          if (json.containsKey("API"))
            my_api = json["API"];
          if (json.containsKey("Port"))
            my_port = json["Port"];
          if (json.containsKey("URL"))
            strcpy(my_url, json["URL"]);
          if (json.containsKey("DB"))
            strcpy(my_db, json["DB"]);
          if (json.containsKey("Job"))
            strcpy(my_job, json["Job"]);
          if (json.containsKey("Instance"))
            strcpy(my_instance, json["Instance"]);            
          if (json.containsKey("Vfact"))
            my_vfact = json["Vfact"];
          if (json.containsKey("TS"))
            my_tempscale = json["TS"];
          if (json.containsKey("SSID"))
            my_ssid = (const char *)json["SSID"];
          if (json.containsKey("PSK"))
            my_psk = (const char *)json["PSK"];
          if (json.containsKey("POLY"))
            strcpy(my_polynominal, json["POLY"]);

          my_aX = UNINIT;
          my_aY = UNINIT;
          my_aZ = UNINIT;

          if (json.containsKey("aX"))
            my_aX = json["aX"];
          if (json.containsKey("aY"))
            my_aY = json["aY"];
          if (json.containsKey("aZ"))
            my_aZ = json["aZ"];
          applyOffset();

          CONSOLELN(F("parsed config:"));
#ifdef DEBUG
          json.printTo(Serial);
#endif
          return true;
        }
        else
        {
          CONSOLELN(F("ERROR: failed to load json config"));
          return false;
        }
      }
      CONSOLELN(F("ERROR: unable to open config file"));
    }
  }
  else
  {
    CONSOLELN(F(" ERROR: failed to mount FS!"));
    return false;
  }
}

bool shouldStartConfig()
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
  CONSOLELN(_reset_reason);
  bool _poweredOnOffOn = _reset_reason == REASON_DEFAULT_RST || _reset_reason == REASON_EXT_SYS_RST;
  if (_poweredOnOffOn)
    CONSOLELN(F("power-cycle or reset detected, config mode"));

  bool _dblreset = drd.detectDoubleReset();
  if (_dblreset)
    CONSOLELN(F("\nDouble Reset detected"));
  bool _validConf = readConfig();
  if (!_validConf)
    CONSOLELN(F("\nERROR config corrupted"));

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

  if (_validConf && !_dblreset && _wifiCred && !_poweredOnOffOn)
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

String urlencode(String str)
{
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++)
  {
    c = str.charAt(i);
    if (c == ' ')
    {
      encodedString += '+';
    }
    else if (isalnum(c))
    {
      encodedString += c;
    }
    else
    {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9)
      {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9)
      {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}

String htmlencode(String str)
{
  String encodedstr = "";
  char c;
  uint8_t b;

  for (int i = 0; i < str.length(); i++)
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
                                   TKIDSIZE);
  WiFiManagerParameter custom_sleep("sleep", "Update Intervall (s)",
                                    String(my_sleeptime).c_str(), 6, TYPE_NUMBER);
  WiFiManagerParameter custom_token("token", "Token", htmlencode(my_token).c_str(),
                                    TKIDSIZE);
  WiFiManagerParameter custom_server("server", "Server Address",
                                     my_server, TKIDSIZE);
  WiFiManagerParameter custom_port("port", "Server Port",
                                   String(my_port).c_str(), TKIDSIZE,
                                   TYPE_NUMBER);
  WiFiManagerParameter custom_url("url", "Server URL", my_url, TKIDSIZE);
  WiFiManagerParameter custom_db("db", "InfluxDB db", my_db, TKIDSIZE);
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
  wifiManager.addParameter(&custom_url);
  wifiManager.addParameter(&custom_db);
  wifiManager.addParameter(&custom_job);
  wifiManager.addParameter(&custom_instance);
  WiFiManagerParameter custom_polynom_lbl("<hr><label for=\"POLYN\">Gravity conversion<br/>ex. \"0.00438*(tilt)*(tilt) + 0.13647*(tilt) - 6.96\"</label>");
  wifiManager.addParameter(&custom_polynom_lbl);
  WiFiManagerParameter custom_polynom("POLYN", "Polynominal", htmlencode(my_polynominal).c_str(), 70, WFM_NO_LABEL);
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
  validateInput(custom_job.getValue(), my_job);
  validateInput(custom_instance.getValue(), my_instance);
  my_sleeptime = String(custom_sleep.getValue()).toInt();

  my_api = String(custom_api.getValue()).toInt();
  my_port = String(custom_port.getValue()).toInt();
  my_tempscale = String(custom_tempscale.getValue()).toInt();
  validateInput(custom_url.getValue(), my_url);

  String tmp = custom_vfact.getValue();
  tmp.trim();
  tmp.replace(',', '.');
  my_vfact = tmp.toFloat();
  if (my_vfact < ADCDIVISOR * 0.8 || my_vfact > ADCDIVISOR * 1.2)
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

void formatSpiffs()
{
  CONSOLE(F("\nneed to format SPIFFS: "));
  SPIFFS.end();
  SPIFFS.begin();
  CONSOLELN(SPIFFS.format());
}

bool saveConfig()
{
  CONSOLE(F("saving config..."));

  // if SPIFFS is not usable
  if (!SPIFFS.begin() || !SPIFFS.exists(CFGFILE) ||
      !SPIFFS.open(CFGFILE, "w"))
    formatSpiffs();

  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  json["Name"] = my_name;
  json["Token"] = my_token;
  json["Sleep"] = my_sleeptime;
  // first reboot is for test
  my_sleeptime = 1;
  json["Server"] = my_server;
  json["API"] = my_api;
  json["Port"] = my_port;
  json["URL"] = my_url;
  json["DB"] = my_db;
  json["Job"] = my_job;
  json["Instance"] = my_instance;
  json["Vfact"] = my_vfact;
  json["TS"] = my_tempscale;

  // Store current Wifi credentials
  json["SSID"] = WiFi.SSID();
  json["PSK"] = WiFi.psk();

  json["POLY"] = my_polynominal;
  json["aX"] = my_aX;
  json["aY"] = my_aY;
  json["aZ"] = my_aZ;

  File configFile = SPIFFS.open(CFGFILE, "w+");
  if (!configFile)
  {
    CONSOLELN(F("failed to open config file for writing"));
    SPIFFS.end();
    return false;
  }
  else
  {
#ifdef DEBUG
    json.printTo(Serial);
#endif
    json.printTo(configFile);
    configFile.close();
    SPIFFS.end();
    CONSOLELN(F("saved successfully"));
    return true;
  }
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

#ifdef API_INFLUXDB
  if (service == DTInfluxDB)
  {
    sender.add("tilt", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    CONSOLELN(F("\ncalling InfluxDB"));
    CONSOLELN(String(F("Sending to db: ")) + my_db);
    return sender.sendInfluxDB(my_server, my_port, my_db, my_name);
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
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());

    if (service == DTHTTP)
    {
      CONSOLELN(F("\ncalling HTTP"));
      // return sender.send(my_server, my_url, my_port);
      return sender.sendGenericPost(my_server, my_url, my_port);
    }
    else if (service == DTCraftBeerPi)
    {
      CONSOLELN(F("\ncalling CraftbeerPi"));
      // return sender.send(my_server, CBP_ENDPOINT, 5000);
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
      return sender.sendTCP(my_server, my_port);
    }
  }
#endif // DATABASESYSTEM

#ifdef API_FHEM
  if (service == DTFHEM)
  {
    sender.add("angle", Tilt);
    sender.add("temperature", scaleTemperature(Temperatur));
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
    return sender.sendTCONTROL(my_server, my_port);
  }
#endif // DATABASESYSTEM ==
}

void goodNight(uint32_t seconds)
{

  uint32_t _seconds = seconds;
  uint32_t left2sleep = 0;
  uint32_t validflag = RTCVALIDFLAG;

  drd.stop();

  // workaround for DS not floating
  pinMode(ONE_WIRE_BUS, OUTPUT);
  digitalWrite(ONE_WIRE_BUS, LOW);

  // we need another incarnation before work run
  if (_seconds > MAXSLEEPTIME)
  {
    left2sleep = _seconds - MAXSLEEPTIME;
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR + 1, &validflag, sizeof(validflag));
    CONSOLELN(String(F("\nStep-sleep: ")) + MAXSLEEPTIME + "s; left: " + left2sleep + "s; RT:" + millis());
    ESP.deepSleep(MAXSLEEPTIME * 1e6, WAKE_RF_DISABLED);
    // workaround proper power state init
    delay(500);
  }
  // regular sleep with RF enabled after wakeup
  else
  {
    // clearing RTC to mark next wake
    left2sleep = 0;
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR, &left2sleep, sizeof(left2sleep));
    ESP.rtcUserMemoryWrite(RTCSLEEPADDR + 1, &validflag, sizeof(validflag));
    CONSOLELN(String(F("\nFinal-sleep: ")) + _seconds + "s; RT:" + millis());
    // WAKE_RF_DEFAULT --> auto reconnect after wakeup
    ESP.deepSleep(_seconds * 1e6, WAKE_RF_DEFAULT);
    // workaround proper power state init
    delay(500);
  }
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
  if (DSrequested == false)
  {
    DS18B20.requestTemperatures();
    DSreqTime = millis();
    DSrequested = true;
  }
}

void initDS18B20()
{

  // workaround for DS not enough power to boot
  pinMode(ONE_WIRE_BUS, OUTPUT);
  digitalWrite(ONE_WIRE_BUS, LOW);
  delay(100);

  DS18B20.begin();
  DS18B20.setWaitForConversion(false);
  DS18B20.getAddress(tempDeviceAddress, 0);
  DS18B20.setResolution(tempDeviceAddress, RESOLUTION);
  requestTemp();
}

void initAccel()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin(D3, D4);
  Wire.setClock(100000);
  Wire.setClockStretchLimit(2 * 230);

  // init the Accel
  accelgyro.initialize();
  accelgyro.setDLPFMode(MPU6050_DLPF_BW_5);
  accelgyro.setTempSensorEnabled(true);
#ifdef USE_DMP
  accelgyro.setDMPEnabled(true);
  packetSize = accelgyro.dmpGetFIFOPacketSize();
#endif
}

float calculateTilt()
{
  float _ax = ax;
  float _ay = ay;
  float _az = az;
  float pitch = (atan2(_ay, sqrt(_ax * _ax + _az * _az))) * 180.0 / M_PI;
  float roll = (atan2(_ax, sqrt(_ay * _ay + _az * _az))) * 180.0 / M_PI;
  return sqrt(pitch * pitch + roll * roll);
}

void getAccSample()
{
  uint8_t res = Wire.status();
  uint8_t con = accelgyro.testConnection();
  if (res == I2C_OK && con == true)
    accelgyro.getAcceleration(&ax, &az, &ay);
  else
  {
    CONSOLELN(String(F("I2C ERROR: ")) + res + " con:" + con);
  }
}

float getTilt()
{
  // make sure enough time for Acc to start
  uint32_t start = ACCINTERVAL;

  for (uint8_t i = 0; i < MEDIANROUNDS; i++)
  {
    while (millis() - start < ACCINTERVAL)
      yield();
    start = millis();
    getAccSample();
    float _tilt = calculateTilt();
    CONSOLE(F("Spl "));
    CONSOLE(i);
    CONSOLE(": ");
    CONSOLELN(_tilt);
    samples.add(_tilt);
  }
  return samples.getAverage(MEDIANAVRG);
}

float getTemperature(bool block = false)
{
  // we need to wait for DS18b20 to finish conversion
  float t = Temperatur;

  // if we need the result we have to block
  while (block && (millis() - DSreqTime <= OWinterval))
    yield();

  if (millis() - DSreqTime >= OWinterval)
  {
    t = DS18B20.getTempCByIndex(0);
    DSrequested = false;

    if (t == DEVICE_DISCONNECTED_C || // DISCONNECTED
        t == 85.0)                    // we read 85 uninitialized
    {
      CONSOLELN(F("ERROR: OW DISCONNECTED"));
      pinMode(ONE_WIRE_BUS, OUTPUT);
      digitalWrite(ONE_WIRE_BUS, LOW);
      delay(100);
      oneWire.reset();

      if (block)
      {
        CONSOLELN(F("OW Retry"));
        initDS18B20();
        delay(OWinterval + 100);
        t = getTemperature(false);
      }
    }
  }
  return t;
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
  CONSOLELN(F("Rescue Wifi credentials"));
  delay(100);
}

void setup()
{

  Serial.begin(115200);

  CONSOLELN(F("\nFW " FIRMWAREVERSION));
  CONSOLELN(ESP.getSdkVersion());

  sleepManager();

  initAccel();
  initDS18B20();

  // decide whether we want configuration mode or normal mode
  if (shouldStartConfig())
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
    float pitch = (atan2(_ay, sqrt(_ax * _ax + _az * _az))) * 180.0 / M_PI;
    float roll = (atan2(_ax, sqrt(_ay * _ay + _az * _az))) * 180.0 / M_PI;
    Tilt = sqrt(pitch * pitch + roll * roll);
  }
#endif

  Temperatur = accelgyro.getTemperature() / 340.00 + 36.53;
  accelgyro.setSleepEnabled(true);

  CONSOLE(F("\na: "));
  CONSOLE(ax);
  CONSOLE(F("\t"));
  CONSOLE(ay);
  CONSOLE(F("\t"));
  CONSOLE(az);

  CONSOLE(F("\tabsTilt: "));
  CONSOLE(Tilt);
  CONSOLE(F("\tT: "));
  CONSOLE(Temperatur);
  CONSOLE(F("\tV: "));
  CONSOLE(Volt);

  // call as late as possible since DS needs converge time
  Temperatur = getTemperature(true);
  CONSOLE(F("\towT: "));
  CONSOLE(Temperatur);

  // calc gravity on user defined polynominal
  Gravity = calculateGravity();
  CONSOLE(F("\tGravity: "));
  CONSOLELN(Gravity);

  // water anomaly correction
  // float _temp = Temperatur - 4; // polynominal at 4
  // float wfact = 0.00005759 * _temp * _temp * _temp - 0.00783198 * _temp * _temp - 0.00011688 * _temp + 999.97;
  // corrGravity = Gravity - (1 - wfact / 1000);
  // CONSOLE(F("\tcorrGravity: "));
  // CONSOLELN(corrGravity);

  unsigned long startedAt = millis();
  CONSOLE(F("After waiting "));
  // int connRes = WiFi.waitForConnectResult();
  uint8_t wait = 0;
  while (WiFi.status() == WL_DISCONNECTED)
  {
    delay(100);
    wait++;
    if (wait > 50)
      break;
  }
  auto waited = (millis() - startedAt);
  CONSOLE(waited);
  CONSOLE(F("ms, result "));
  CONSOLELN(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    CONSOLE(F("IP: "));
    CONSOLELN(WiFi.localIP());
    uploadData(my_api);
    delay(100); // workaround for https://github.com/esp8266/Arduino/issues/2750
  }
  else
  {
    connectBackupCredentials();
    CONSOLELN(F("failed to connect"));
  }

  // survive - 60min sleep time
  if (isSafeMode(Volt))
    my_sleeptime = EMERGENCYSLEEP;
  goodNight(my_sleeptime);
}

void loop()
{
  CONSOLELN(F("should never be here!"));
}
