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
char my_polynominal[70] = "-0.00031*tilt^2+0.557*tilt-14.054";

String my_ssid;
String my_psk;
uint8_t my_api;
uint32_t my_sleeptime = 15 * 60;
uint16_t my_port = 80;
float my_vfact = ADCDIVISOR;
int16_t my_aX = UNINIT, my_aY = UNINIT, my_aZ = UNINIT;

uint32_t DSreqTime;
float pitch, roll;

int16_t ax, ay, az;
float Volt, Temperatur, Tilt, Gravity;
// , corrGravity;

bool DSrequested = false;

bool isDebugEnabled()
{
#ifdef DEBUG
  return true;
#endif // DEBUG
  return false;
}

// generic serial output
template <typename T>
void SerialOut(const T aValue, bool newLine = true)
{
  if (!isDebugEnabled())
    return;
  Serial.print(aValue);
  if (newLine)
    Serial.print("\n");
}

void SerialOut() { SerialOut(""); }

// callback notifying us of the need to save config
void saveConfigCallback()
{
  // WiFi.setAutoReconnect(true);
  shouldSaveConfig = true;
}

void applyOffset()
{
  if (my_aX != UNINIT && my_aY != UNINIT && my_aZ != UNINIT)
  {
    SerialOut(F("applying offsets"));
    accelgyro.setXAccelOffset(my_aX);
    accelgyro.setYAccelOffset(my_aY);
    accelgyro.setZAccelOffset(my_aZ);
  }
  else
    SerialOut(F("offsets not available"));
}

bool readConfig()
{
  SerialOut(F("mounting FS..."), false);

  if (SPIFFS.begin())
  {
    SerialOut(F(" mounted!"));
    if (SPIFFS.exists(CFGFILE))
    {
      // file exists, reading and loading
      SerialOut(F("reading config file"));
      File configFile = SPIFFS.open(CFGFILE, "r");
      if (configFile)
      {
        SerialOut(F("opened config file"));
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());

        if (json.success())
        {
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
          if (json.containsKey("Vfact"))
            my_vfact = json["Vfact"];

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

          SerialOut(F("\nparsed config:"));
          if (isDebugEnabled)
            json.printTo(Serial);
            Serial.println();
          return true;
        }
        else
        {
          SerialOut(F("ERROR: failed to load json config"));
          return false;
        }
      }
      SerialOut(F("ERROR: unable to open config file"));
    }
  }
  else
  {
    SerialOut(F(" ERROR: failed to mount FS!"));
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
  SerialOut("Boot-Mode: ", false);
  SerialOut(_reset_reason);
  bool _poweredOnOffOn = _reset_reason == REASON_DEFAULT_RST || _reset_reason == REASON_EXT_SYS_RST;
  if (_poweredOnOffOn)
    SerialOut("power-cycle or reset detected, config mode");

  bool _dblreset = drd.detectDoubleReset();
  if (_dblreset)
    SerialOut("\nDouble Reset detected");
  bool _validConf = readConfig();
  if (!_validConf)
    SerialOut("\nERROR config corrupted");

  bool _wifiCred = (WiFi.SSID() != "");
  uint8_t c = 0;
  if (!_wifiCred)
    WiFi.begin();
  while (!_wifiCred)
  {
    if (c > 10)
      break;
    SerialOut('.', false);
    delay(100);
    c++;
    _wifiCred = (WiFi.SSID() != "");
  }
  if (!_wifiCred)
    SerialOut("\nERROR no Wifi credentials");

  if (_validConf && !_dblreset && _wifiCred && !_poweredOnOffOn)
  {
    SerialOut(F("\nwoken from deepsleep, normal mode"));
    return false;
  }
  // config mode
  else
  {
    SerialOut(F("\ngoing to Config Mode"));
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
  WiFiManagerParameter custom_vfact("vfact", "Battery conversion factor",
                                    String(my_vfact).c_str(), 7, TYPE_NUMBER);

  wifiManager.addParameter(&custom_name);
  wifiManager.addParameter(&custom_sleep);
  wifiManager.addParameter(&custom_vfact);

  WiFiManagerParameter custom_api_hint("<hr><label for=\"API\">Service Type</label>");
  wifiManager.addParameter(&custom_api_hint);

  wifiManager.addParameter(&api_list);
  wifiManager.addParameter(&custom_api);

  wifiManager.addParameter(&custom_token);
  wifiManager.addParameter(&custom_server);
  wifiManager.addParameter(&custom_port);
  wifiManager.addParameter(&custom_url);
  WiFiManagerParameter custom_polynom_lbl("<hr><label for=\"POLYN\">Gravity conversion<br/>ex. \"0.00438*(tilt)*(tilt) + 0.13647*(tilt) - 6.96\"</label>");
  wifiManager.addParameter(&custom_polynom_lbl);
  WiFiManagerParameter custom_polynom("POLYN", "Polynominal", htmlencode(my_polynominal).c_str(), 70, WFM_NO_LABEL);
  wifiManager.addParameter(&custom_polynom);

  wifiManager.setConfSSID(htmlencode(my_ssid));
  wifiManager.setConfPSK(htmlencode(my_psk));

  SerialOut(F("started Portal"));
  wifiManager.startConfigPortal("iSpindel");

  strcpy(my_polynominal, custom_polynom.getValue());

  validateInput(custom_name.getValue(), my_name);
  validateInput(custom_token.getValue(), my_token);
  validateInput(custom_server.getValue(), my_server);
  my_sleeptime = String(custom_sleep.getValue()).toInt();

  my_api = String(custom_api.getValue()).toInt();
  my_port = String(custom_port.getValue()).toInt();
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
    saveConfig();
    return true;
  }
  return false;
}

void formatSpiffs()
{
  SerialOut(F("\nneed to format SPIFFS: "), false);
  SPIFFS.end();
  SPIFFS.begin();
  SerialOut(SPIFFS.format());
}

bool saveConfig()
{
  SerialOut(F("saving config..."), false);

  // if SPIFFS is not usable
  if (!SPIFFS.begin() || !SPIFFS.exists(CFGFILE) ||
      !SPIFFS.open(CFGFILE, "w"))
    formatSpiffs();

  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();

  json["Name"] = my_name;
  json["Token"] = my_token;
  json["Sleep"] = my_sleeptime;
  json["Server"] = my_server;
  json["API"] = my_api;
  json["Port"] = my_port;
  json["URL"] = my_url;
  json["Vfact"] = my_vfact;

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
    SerialOut(F("failed to open config file for writing"), true);
    SPIFFS.end();
    return false;
  }
  else
  {
    if (isDebugEnabled)
      json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    SPIFFS.end();
    SerialOut(F("saved successfully"), true);
    return true;
  }
}

bool downloadData(uint8_t service)
{
  SenderClass receiver;
  String response;

  if (service == DTUbiDots)
  {
    SerialOut(F("\nrequesting Ubidots"), true);
    response = receiver.getUbidotsValue(my_token, my_name, "interval");
    if (response.length() > 0)
    {
      uint32_t _interval = receiver.getInterval(response);
      if (_interval > 10 && _interval < 24 * 3600 && my_sleeptime != _interval)
      {
        Serial.print(F("received new interval: "));
        Serial.println(_interval);
        my_sleeptime = _interval;
        saveConfig();
        return true;
      }
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
    sender.add("temperature", Temperatur);
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());
    SerialOut(F("\ncalling Ubidots"), true);
    return sender.sendUbidots(my_token, my_name);
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
    sender.add("temperature", Temperatur);
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("interval", my_sleeptime);
    sender.add("RSSI", WiFi.RSSI());

    if (service == DTHTTP)
    {
      SerialOut(F("\ncalling HTTP"));
      // return sender.send(my_server, my_url, my_port);
      return sender.sendGenericPost(my_server, my_url, my_port);
    }
    else if (service == DTCraftBeerPi)
    {
      SerialOut(F("\ncalling CraftbeerPi"));
      // return sender.send(my_server, CBP_ENDPOINT, 5000);
      return sender.sendGenericPost(my_server, CBP_ENDPOINT, 5000);
    }
    else if (service == DTiSPINDELde)
    {
      SerialOut(F("\ncalling iSPINDELde"));
      return sender.sendTCP("ispindle.de", 9501);
    }
    else if (service == DTTCP)
    {
      SerialOut(F("\ncalling TCP"));
      return sender.sendTCP(my_server, my_port);
    }
  }
#endif // DATABASESYSTEM

#ifdef API_FHEM
  if (service == DTFHEM)
  {
    sender.add("angle", Tilt);
    sender.add("temperature", Temperatur);
    sender.add("battery", Volt);
    sender.add("gravity", Gravity);
    sender.add("ID", ESP.getChipId());
    SerialOut(F("\ncalling FHEM"));
    return sender.sendFHEM(my_server, my_port, my_name);
  }
#endif // DATABASESYSTEM ==
#ifdef API_TCONTROL
  if (service == DTTcontrol)
  {
    sender.add("T", Temperatur);
    sender.add("D", Tilt);
    sender.add("U", Volt);
    sender.add("G", Gravity);
    SerialOut(F("\ncalling TCONTROL"));
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
    SerialOut(String(F("\nStep-sleep: ")) + MAXSLEEPTIME + "s; left: " + left2sleep + "s; RT:" + millis());
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
    SerialOut(String(F("\nFinal-sleep: ")) + _seconds + "s; RT:" + millis());
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
    SerialOut(F("Worker run!"));
  }
}

void requestTemp()
{
  if (DSrequested == false)
  {
    DS18B20.requestTemperatures();
    DSreqTime = millis();
    DSrequested = true;
    // SerialOut("DEBUG DSreq:");
  }
}

void initDS18B20()
{

  // workaround for DS not enough power to boot
  pinMode(ONE_WIRE_BUS, OUTPUT);
  digitalWrite(ONE_WIRE_BUS, LOW);
  delay(100);
  // digitalWrite(ONE_WIRE_BUS, HIGH);
  // delay(500);
  // oneWire.reset();
  // Start up the DS18B20
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
    SerialOut(String("I2C ERROR: MPU6050 unaccessible! ") + res + " con:" + con);
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
    SerialOut("Spl ", false);
    SerialOut(i, false);
    SerialOut(": ", false);
    SerialOut(_tilt);
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
      SerialOut(F("ERROR: DS18b20 Temp Sensor DISCONNECTED"));
      pinMode(ONE_WIRE_BUS, OUTPUT);
      digitalWrite(ONE_WIRE_BUS, LOW);
      delay(100);
      // digitalWrite(ONE_WIRE_BUS, HIGH);
      // delay(500);
      oneWire.reset();

      if (block)
      {
        SerialOut(F("OW Retry"));
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
    Serial.println(String("Parse error at ") + err);
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
    SerialOut("\nWARNING: low Battery");
    return true;
  }
  else
    return false;
}

bool connectBackupCredentials()
{
  WiFi.disconnect();
  WiFi.begin(my_ssid.c_str(), my_psk.c_str());
  SerialOut(F("Rescue Wifi credentials"));
  delay(100);
}

void setup()
{

  Serial.begin(115200);

  SerialOut("\nFW " FIRMWAREVERSION);
  SerialOut(ESP.getSdkVersion());

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
      Serial.println(F("reboot RFCAL"));
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
    Serial.println("wait DMP");

    fifoCount = accelgyro.getFIFOCount();
  }
  if (fifoCount == 1024)
  {
    Serial.println("FIFO overflow");
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
        Serial.print(fifoBuffer[i]);
        Serial.print(" ");
      }
   */

    Serial.print("euler\t");
    Serial.print((euler[0] * 180 / M_PI));
    Serial.print("\t");
    Serial.print(euler[1] * 180 / M_PI);
    Serial.print("\t");
    Serial.println(euler[2] * 180 / M_PI);

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

  SerialOut(F("\na: "), false);
  SerialOut(ax, false);
  SerialOut(F("\t"), false);
  SerialOut(ay, false);
  SerialOut(F("\t"), false);
  SerialOut(az, false);

  SerialOut(F("\tabsTilt: "), false);
  SerialOut(Tilt, false);
  SerialOut(F("\tT: "), false);
  SerialOut(Temperatur, false);
  SerialOut(F("\tV: "), false);
  SerialOut(Volt, false);

  // call as late as possible since DS needs converge time
  Temperatur = getTemperature(true);
  SerialOut(F("\towT: "), false);
  SerialOut(Temperatur, false);

  // calc gravity on user defined polynominal
  Gravity = calculateGravity();
  SerialOut(F("\tGravity: "), false);
  SerialOut(Gravity, true);

  // water anomaly correction
  // float _temp = Temperatur - 4; // polynominal at 4
  // float wfact = 0.00005759 * _temp * _temp * _temp - 0.00783198 * _temp * _temp - 0.00011688 * _temp + 999.97;
  // corrGravity = Gravity - (1 - wfact / 1000);
  // SerialOut(F("\tcorrGravity: "), false);
  // SerialOut(corrGravity, true);

  unsigned long startedAt = millis();
  SerialOut(F("After waiting "), false);
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
  SerialOut(waited, false);
  SerialOut(F("ms, result "), false);
  SerialOut(WiFi.status());

  if (WiFi.status() == WL_CONNECTED)
  {
    SerialOut("IP: ", false);
    SerialOut(WiFi.localIP());

    if (shouldSaveConfig == false)
      downloadData(my_api);

    uploadData(my_api);
    delay(100); // workaround for https://github.com/esp8266/Arduino/issues/2750
  }
  else
    connectBackupCredentials();

  // survive - 60min sleep time
  if (isSafeMode(Volt))
    my_sleeptime = EMERGENCYSLEEP;

  // First reboot after saving is test
  if (shouldSaveConfig)
    my_sleeptime = 1;

  goodNight(my_sleeptime);
}

void loop()
{
  SerialOut(F("should never be here!"));
}
