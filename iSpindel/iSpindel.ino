/**************************************************************

"iSpindel"
All rights reserverd by S.Lang <universam@web.de>

**************************************************************/

// includes go here
#include "DallasTemperature.h"
#include "DoubleResetDetector.h" // https://github.com/datacute/DoubleResetDetector
#include "Globals.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "OneWire.h"
#include "RunningMedian.h"
#include "Ubidots.h"
#include "WiFiManagerKT.h"
#include "Wire.h"
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <FS.h>          //this needs to be first
#include <Ticker.h>

#ifdef API_UBIDOTS
#include "Ubidots.h"
#endif
#ifdef API_GENERIC
#include "genericHTTP.h"
#endif
#ifdef API_FHEM
#include "FHEM.h"
#endif
#ifdef API_TCONTROL
#include "TControl.h"
#endif // DATABASESYSTEM ==
#ifndef DEBUG
#define DEBUG 1 // uncomment this line to enable serial diagnostic messages
#endif          // !DEBUG 1

// definitions go here
MPU6050 accelgyro;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
DeviceAddress tempDeviceAddress;
Ticker flasher;
RunningMedian samples = RunningMedian(MEDIANROUNDS);

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

bool shouldSaveConfig = false;

char my_token[TKIDSIZE];
char my_name[TKIDSIZE] = "iSpindel000";
char my_server[TKIDSIZE];
char my_url[TKIDSIZE];
uint8_t my_api;
uint32_t my_sleeptime = 15;
uint16_t my_port = 80;

uint32_t DSreqTime;
float pitch, roll;

int16_t ax, ay, az;
float Volt, Temperatur, Tilt;

bool DSrequested = false;

bool isDebugEnabled() {
#ifdef DEBUG
  return true;
#endif // DEBUG
  return false;
}

// generic serial output
template <typename T> void SerialOut(const T aValue, bool newLine = true) {
  if (!isDebugEnabled())
    return;
  Serial.print(aValue);
  if (newLine)
    Serial.print("\n");
}

void SerialOut() { SerialOut(""); }

char *replace_char(char *input, char find, char replace) {
  char *output = (char *)malloc(strlen(input));
  for (int i = 0; i < strlen(input); i++) {
    if (input[i] == find)
      output[i] = replace;
    else
      output[i] = input[i];
  }
  output[strlen(input)] = '\0';
  return output;
}

// callback notifying us of the need to save config
void saveConfigCallback() {
  SerialOut(F("Should save config"));
  WiFi.setAutoReconnect(true);
  shouldSaveConfig = true;
}
bool readConfig() {
  SerialOut(F("mounting FS..."), false);

  if (SPIFFS.begin()) {
    SerialOut(F(" mounted!"));
    if (SPIFFS.exists(CFGFILE)) {
      // file exists, reading and loading
      SerialOut(F("reading config file"));
      File configFile = SPIFFS.open(CFGFILE, "r");
      if (configFile) {
        SerialOut(F("opened config file"));
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());

        if (json.success()) {
          SerialOut(F("\nparsed json"));

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

          SerialOut(F("parsed config:"));
          if (isDebugEnabled)
            json.printTo(Serial);
          return true;
        } else {
          SerialOut(F("ERROR: failed to load json config"));
          return false;
        }
      }
      SerialOut(F("ERROR: unable to open config file"));
    }
  } else {
    SerialOut(F(" ERROR: failed to mount FS!"));
    return false;
  }
}

bool shouldStartConfig() {

  // we make sure that configuration is properly set and we are not woken by
  // RESET button
  // ensure this was called

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
  while (!_wifiCred) {
    if (c > 10)
      break;
    SerialOut('.', false);
    delay(100);
    c++;
    _wifiCred = (WiFi.SSID() != "");
  }
  if (!_wifiCred)
    SerialOut("\nERROR no Wifi credentials");

  if (_validConf && !_dblreset && _wifiCred) {
    SerialOut(F("\nwoken from deepsleep, normal mode"));
    return false;
  }
  // config mode
  else {
    SerialOut(F("\ngoing to Config Mode"));
    return true;
  }
}

void validateInput(const char *input, char *output) {
  String tmp = String(input);
  tmp.trim();
  tmp.replace(' ', '_');
  tmp.toCharArray(output, tmp.length() + 1);
}

bool startConfiguration() {

  WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(PORTALTIMEOUT);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setBreakAfterConfig(true);

  WiFiManagerParameter api_list(HTTP_API_LIST);
  WiFiManagerParameter custom_api("selAPI", "selAPI", String(my_api).c_str(),
                                  20, TYPE_HIDDEN, WFM_NO_LABEL);

  WiFiManagerParameter custom_name("name", "iSpindel Name", my_name,
                                   TKIDSIZE);
  WiFiManagerParameter custom_sleep("sleep", "Update Intervall (s)",
                                    String(my_sleeptime).c_str(), 5);
  WiFiManagerParameter custom_token("token", "Ubidots Token", my_token,
                                    TKIDSIZE, TYPE_HIDDEN, WFM_NO_LABEL);
  WiFiManagerParameter custom_server("server", "Server Address",
                                     my_server, TKIDSIZE, TYPE_HIDDEN,
                                     WFM_NO_LABEL);
  WiFiManagerParameter custom_port("port", "Server Port",
                                   String(my_port).c_str(), TKIDSIZE,
                                   TYPE_HIDDEN, WFM_NO_LABEL);
  WiFiManagerParameter custom_url("url", "Server URL", my_url, TKIDSIZE,
                                  TYPE_HIDDEN, WFM_NO_LABEL);

  wifiManager.addParameter(&custom_name);
  wifiManager.addParameter(&custom_sleep);

  wifiManager.addParameter(&api_list);
  wifiManager.addParameter(&custom_api);

  wifiManager.addParameter(&custom_token);
  wifiManager.addParameter(&custom_server);
  wifiManager.addParameter(&custom_port);
  wifiManager.addParameter(&custom_url);

  wifiManager.startConfigPortal("iSpindel");
  SerialOut(F("started Portal"));

  validateInput(custom_name.getValue(), my_name);
  validateInput(custom_token.getValue(), my_token);
  validateInput(custom_server.getValue(), my_server);
  my_sleeptime = String(custom_sleep.getValue()).toInt();
  my_api = String(custom_api.getValue()).toInt();
  my_port = String(custom_port.getValue()).toInt();
  validateInput(custom_url.getValue(), my_url);

  // save the custom parameters to FS
  if (shouldSaveConfig) {
    SerialOut(F("saving config..."), false);

    // if SPIFFS is not usable
    if (!SPIFFS.begin() || !SPIFFS.exists(CFGFILE) ||
        !SPIFFS.open(CFGFILE, "w")) {
      SerialOut(F("\nneed to format SPIFFS: "), false);
      SPIFFS.end();
      SPIFFS.begin();
      SerialOut(SPIFFS.format());
    }

    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();

    json["Name"] = my_name;
    json["Token"] = my_token;
    json["Sleep"] = my_sleeptime;
    json["Server"] = my_server;
    json["API"] = my_api;
    json["Port"] = my_port;
    json["URL"] = my_url;

    File configFile = SPIFFS.open(CFGFILE, "w+");
    if (!configFile) {
      SerialOut(F("failed to open config file for writing"), true);
    } else
      SerialOut(F("saved successfully"), true);

    if (isDebugEnabled)
      json.printTo(Serial);

    json.printTo(configFile);
    configFile.close();
    SPIFFS.end();

    // Wifi config
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    // end save
    return true;
  }
  return false;
}

bool uploadData(uint8_t service) {
  char url[TKIDSIZE];
  uint16_t port;

#ifdef API_UBIDOTS
  if (service == DTUbiDots) {
    Ubidots ubiclient(my_token, my_name);
    ubiclient.add("tilt", Tilt);
    ubiclient.add("temperature", Temperatur);
    ubiclient.add("battery", Volt);
    // This creates a new Device, only with TCP transmission
    ubiclient.setDataSourceName(my_name);
    return ubiclient.sendAll(false);
  }
#endif

#ifdef API_GENERIC
  if ( (service == DTHTTP)||(service == DTCraftbeepPi) ) {

  if (service == DTHTTP) {
    SerialOut(F("\ncalling DTHTTP "));
    strcpy(url, my_url);
    port = my_port;
  } else if (service == DTCraftbeepPi) {
    SerialOut(F("\ncalling DTCraftbeepPi "));
    strcpy(url, CBP_ENDPOINT);
    port = 5000;
  }

  genericHTTP genclient(my_name, my_server, port, url);
  genclient.add("angle", Tilt);
  genclient.add("temperature", Temperatur);
  genclient.add("battery", Volt);
  return genclient.sendHTTP();
  }
#endif // DATABASESYSTEM
  
#ifdef API_FHEM
  if (service == DTFHEM) {
  FhemHttp fhemclient(my_name, my_server, my_port);
  fhemclient.add("angle", Tilt);
  fhemclient.add("temperature", Temperatur);
  fhemclient.add("battery", Volt);
  return fhemclient.sendHTTP();
  }
#endif // DATABASESYSTEM ==
#ifdef API_TCONTROL
  if (service == DTTcontrol) {
  TControl tcclient(my_name, my_server, my_port);
  tcclient.add("T", Temperatur);
  tcclient.add("D", Tilt);
  tcclient.add("U", Volt);
  return tcclient.sendUDP();
  }
#endif // DATABASESYSTEM ==
}

void goodNight() {

  drd.loop();

  // survive - 60min sleep time
  if (isSafeMode(Volt)) my_sleeptime = 60 * 60e6;

  SerialOut(F("\nsleeping: "), false);
  SerialOut(my_sleeptime, false);
  SerialOut(F("sec"));
  // WAKE_RF_DEFAULT --> auto reconnect after wakeup
  ESP.deepSleep(my_sleeptime * 1000UL * 1000UL, WAKE_RF_DEFAULT);
  // workaround proper power state init
  delay(500);
}

float getVolt() { return analogRead(A0) / ADCDIVISOR; }

void initDS18B20() {
  // Start up the DS18B20
  DS18B20.begin();
  DS18B20.setWaitForConversion(false);
  DS18B20.getAddress(tempDeviceAddress, 0);
  DS18B20.setResolution(tempDeviceAddress, RESOLUTION);
  requestTemp();
}

void initAccel() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin(D3, D4);

  // init the Accel
  accelgyro.initialize();
  accelgyro.setDLPFMode(MPU6050_DLPF_BW_5);
  accelgyro.setTempSensorEnabled(true);
}

float calculateTilt() {
  float _ax = ax;
  float _ay = ay;
  float _az = az;
  float pitch = (atan2(_ay, sqrt(_ax * _ax + _az * _az))) * 180.0 / M_PI;
  float roll = (atan2(_ax, sqrt(_ay * _ay + _az * _az))) * 180.0 / M_PI;
  return sqrt(pitch * pitch + roll * roll);
}

float getTilt() {

  // make sure enough time for Acc to start
  uint32_t start = ACCINTERVAL;

  for (uint8_t i = 0; i < MEDIANROUNDS; i++) {
    while (millis() - start < ACCINTERVAL)
      yield();
    start = millis();
    getAccSample();
    float _tilt = calculateTilt();
    SerialOut("Spl ", false);
    SerialOut(i, false);
    SerialOut(F(": "), false);
    SerialOut(_tilt);
    samples.add(_tilt);
  }
  return samples.getAverage(MEDIANAVRG);
}

void getAccSample() { accelgyro.getAcceleration(&ax, &az, &ay); }

float getTemperature(bool block = false) {
  // we need to wait for DS18b20 to finish conversion
  float t = Temperatur;

  // if we need the result we have to block
  while (block && (millis() - DSreqTime <= OWinterval))
    yield();

  if (millis() - DSreqTime > OWinterval) {
    t = DS18B20.getTempCByIndex(0);
    DSrequested = false;

    if (t == DEVICE_DISCONNECTED_C || // DISCONNECTED
        t == 85.0)                    // we read 85 uninitialized
    {
      SerialOut(F("ERROR: OW DISCONNECTED"));
    }
  }
  return t;
}

void requestTemp() {
  if (DSrequested == false) {
    DS18B20.requestTemperatures();
    DSreqTime = millis();
    DSrequested = true;
    // SerialOut("DEBUG DSreq:");
  }
}

void flash() {
  // triggers the LED
  Volt = getVolt();
  getAccSample();
  Tilt = calculateTilt();
  Temperatur = getTemperature(false);
  requestTemp();
}

float getBattery() {
	return analogRead(A0) / ADCDIVISOR;
}

bool isSafeMode(float _volt)  {
	if (_volt < LOWBATT) {
		SerialOut("\nWARNING: low Battery");
		return true;
	}
	else return false;
}

void setup() {

  Serial.begin(115200);
  SerialOut("\nFW " FIRMWAREVERSION);
  SerialOut(ESP.getSdkVersion());

  initDS18B20();

  initAccel();

  // decide whether we want configuration mode or normal mode
  if (shouldStartConfig()) {
    flasher.attach(1, flash);
    startConfiguration();
    flasher.detach();
  }
  // to make sure we wake up with STA but AP
  WiFi.mode(WIFI_STA);
  Volt = getBattery();
  // we try to survive
  if (isSafeMode(Volt)) WiFi.setOutputPower(0);
  else WiFi.setOutputPower(20.5);

  Tilt = getTilt();

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
  SerialOut(Temperatur);

  unsigned long startedAt = millis();
  SerialOut(F("After waiting "), false);
  int connRes = WiFi.waitForConnectResult();
  float waited = (millis() - startedAt);
  SerialOut(waited / 1000, false);
  SerialOut(F(" secs in setup() connection result is "), false);
  SerialOut(connRes);

  if (WiFi.status() == WL_CONNECTED) {
    SerialOut(WiFi.localIP());
    uploadData(my_api);
  } else {
    WiFi.reconnect();
    SerialOut("failed to connect");
  }

  goodNight();
}

void loop() { 
	SerialOut(F("should never be here!"));
}