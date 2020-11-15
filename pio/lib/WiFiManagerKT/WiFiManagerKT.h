/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/esp8266/hardware/esp8266com/esp8266/libraries/DNSServer/examples/CaptivePortalAdvanced
   This version built by Ken Taylor https://github.com/kentaylor
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include "Globals.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <I2Cdev.h>
#include "MPUOffset.h"
#include <memory>
#undef min
#undef max
#include <algorithm>
extern "C" {
#include "user_interface.h"
}

#define WFM_LABEL_BEFORE 1
#define WFM_LABEL_AFTER 2
#define WFM_NO_LABEL 0

const char HTTP_ISPINDEL_IMG[] PROGMEM = "<img src=\"data:image/jpeg;base64,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\" style=\"padding: 40px; display: block; width: 80%;\"/>";

const char HTTP_200[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" charset=\"utf-8\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{v}</title>";

const char HTTP_STYLE[] PROGMEM = "<style>body,textarea,input,select{background: 0;border-radius: 0;font: 16px sans-serif;margin: 0}textarea,input,select{outline: 0;font-size: 14px;border: 1px solid #ccc;padding: 8px;width: 90%}.btn a{text-decoration: none}.container{margin: auto;width: 90%}@media(min-width:1200px){.container{margin: auto;width: 30%}}@media(min-width:768px) and (max-width:1200px){.container{margin: auto;width: 50%}}.btn,h2{font-size: 2em}h1{font-size: 3em}.btn{background: #0ae;border-radius: 4px;border: 0;color: #fff;cursor: pointer;display: inline-block;margin: 2px 0;padding: 10px 14px 11px;width: 100%}.btn:hover{background: #09d}.btn:active,.btn:focus{background: #08b}label>*{display: inline}form>*{display: block;margin-bottom: 10px}textarea:focus,input:focus,select:focus{border-color: #5ab}.msg{background: #def;border-left: 5px solid #59d;padding: 1.5em}.q{float: right;width: 64px;text-align: right}.l{background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;background-size: 1em}input[type='checkbox']{float: left;width: 20px}.table td{padding:.5em;text-align:left}.table tbody>:nth-child(2n-1){background:#ddd}</style>";

const char HTTP_SCRIPT[] PROGMEM = R"V0G0N(
<script>
var lAPI = [
{"name":"Ubidots",    "token":1,"server":0,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"empty",      "token":1,"server":0,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"CraftBeerPi","token":0,"server":1,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"HTTP",       "token":1,"server":1,"uri":1,"port":1,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"TControl",   "token":0,"server":1,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"FHEM",       "token":0,"server":1,"uri":0,"port":1,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"TCP",        "token":1,"server":1,"uri":0,"port":1,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"iSpindel.de","token":1,"server":0,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"InfluxDB",   "token":0,"server":1,"uri":0,"port":1,"channel":0,"db":1,"username":1,"password":1,"job":0,"instance":0},
{"name":"Prometheus", "token":0,"server":1,"uri":0,"port":1,"channel":0,"db":0,"username":0,"password":0,"job":1,"instance":1},
{"name":"MQTT",       "token":0,"server":1,"uri":0,"port":1,"channel":0,"db":0,"username":1,"password":1,"job":0,"instance":0},
{"name":"ThingSpeak", "token":1,"server":0,"uri":0,"port":0,"channel":1,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"Blynk",      "token":1,"server":0,"uri":0,"port":0,"channel":0,"db":0,"username":0,"password":0,"job":0,"instance":0},
{"name":"Brewblox",   "token":0,"server":1,"uri":1,"port":1,"channel":0,"db":0,"username":1,"password":1,"job":0,"instance":0}];
var $ = function (id) { return document.getElementById(id); };
var labels = document.getElementsByTagName('LABEL');
function set(id, show) {
  if ($(id)) {
    $(id).style.display = show ? 'block' : 'none';
    $(id).label.style.display = show ? 'block' : 'none';
  }
};
function c(l) { s.value = l.innerText || l.textContent; p.focus(); };
function sTS() {
  $('tempscale').value = parseInt($('TS').options[$('TS').selectedIndex].value);
};
function sAPI(val) {
  $('selAPI').value = val;
  var obj = lAPI[parseInt(val)];
  Object.keys(obj).forEach(function (key) { set(key, obj[key]);});
};
function fillopt() {
  for (el in lAPI) {
    if (lAPI[el].name == "empty") { continue; }
    var opt = document.createElement("option");
    opt.value = el;
    opt.innerHTML = lAPI[el].name;
    $('API').appendChild(opt);
  }
};
window.onload = function (e) {
  for (var i = 0; i < labels.length; i++) {
    if (labels[i].htmlFor != '') {
      var elem = $(labels[i].htmlFor);
      if (elem) elem.label = labels[i];
    }
  }
  value = $('selAPI').value;
  sAPI(value);
  $('TS').value = $('tempscale').value;
  fillopt();
  $('API').querySelector('option[value="'+value+'"]').selected = true;
}
window.onchange = function (e) {
  sTS();
};</script>)V0G0N";

const char HTTP_API_LIST[] PROGMEM = R"V0G0N(
<select id="API" onchange="sAPI(this.options[this.selectedIndex].value);"></select>)V0G0N";

const char HTTP_TEMPSCALE_LIST[] PROGMEM = R"V0G0N(
<select id="TS" onclick="sTS()">
<option value=0>Celsius</option>
<option value=1>Fahrenheit</option>
<option value=2>Kelvin</option>
</select>)V0G0N";

const char TYPE_HIDDEN[] = "type=\"hidden\"";
const char TYPE_NUMBER[] = "type=\"number\" step=\"any\"";

const char HTTP_HEADER_END[] PROGMEM = "</head><body><div class=\"container\">";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "<form action=\"/iSpindel\" method=\"get\"><button class=\"btn\">iSpindel Info</button></form><br/><form action=\"/wifi\" method=\"get\"><button class=\"btn\">Configuration</button></form><br/><form action=\"/mnt\" method=\"get\"><button class=\"btn\">Maintenance</button></form><br/><form action=\"/i\" method=\"get\"><button class=\"btn\">Information</button></form><br/><form action=\"/close\" method=\"get\"><button class=\"btn\">Exit Portal</button></form><br/>";
const char HTTP_ITEM[] PROGMEM = "<div><a href=\"#p\" onclick=\"c(this)\">{v}</a>&nbsp;<span class=\"q {i}\">{r}%</span></div>";
const char JSON_ITEM[] PROGMEM = "{\"SSID\":\"{v}\", \"Encryption\":{i}, \"Quality\":\"{r}\"}";
// const char HTTP_FORM_START[] PROGMEM = "<form method=\"get\" action=\"wifisave\"><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\"><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\">";
const char HTTP_FORM_START1[] PROGMEM = "<form method=\"get\" action=\"wifisave\"><label>SSID</label><input id=\"s\" name=\"s\" length=32 placeholder=\"SSID\" value=\"";
const char HTTP_FORM_START2[] PROGMEM = "\"><label>Password</label><input id=\"p\" name=\"p\" length=64 placeholder=\"password\" value=\"";
const char HTTP_FORM_START3[] PROGMEM = "\">";

const char HTTP_FORM_LABEL[] PROGMEM = "<label for=\"{i}\">{p}</label>";
const char HTTP_FORM_PARAM[] PROGMEM = "<input id=\"{i}\" name=\"{n}\" length={l} placeholder=\"{p}\" value=\"{v}\" {c}>";
const char HTTP_FORM_END[] PROGMEM = "<button class=\"btn\" type=\"submit\">save</button></form>";
const char HTTP_SAVED[] PROGMEM = "<div class=\"msg\"><strong>Credentials Saved</strong><br>Trying to connect ESP to the {x} network.<br>Give it 10 seconds or so and check <a href=\"/\">how it went.</a> <p/>The {v} you are connected to will be restarted on the radio channel of the {x} network and continue in normal mode thus not being accessible until manually resetted to configuration mode.";
const char HTTP_END[] PROGMEM = "</div></body></html>";
const char HTTP_UPDATE_FAI[] PROGMEM = "Update Failed!";
const char HTTP_UPDATE_SUC[] PROGMEM = "Update Success! Rebooting...";

#define WIFI_MANAGER_MAX_PARAMS 25

class WiFiManagerParameter
{
public:
  WiFiManagerParameter(const char *custom);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);

  const char *getID();
  const char *getValue();
  const char *getPlaceholder();
  int getValueLength();
  int getLabelPlacement();
  const char *getCustomHTML();

private:
  const char *_id;
  const char *_placeholder;
  char *_value;
  int _length;
  int _labelPlacement;
  const char *_customHTML;

  void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom, int labelPlacement);

  friend class WiFiManager;
};

class WiFiManager
{
public:
  WiFiManager();
  ~WiFiManager();

  boolean autoConnect();                                                  //Deprecated. Do not use.
  boolean autoConnect(char const *apName, char const *apPassword = NULL); //Deprecated. Do not use.

  //if you want to start the config portal
  boolean startConfigPortal();
  boolean startConfigPortal(char const *apName, char const *apPassword = NULL);

  // get the AP name of the config portal, so it can be used in the callback
  String getConfigPortalSSID();

  void header();

  void resetSettings();

  //sets timeout before webserver loop ends and exits even if there has been no setup.
  //usefully for devices that failed to connect at some point and got stuck in a webserver loop
  //in seconds setConfigPortalTimeout is a new name for setTimeout
  void setConfigPortalTimeout(unsigned long seconds);
  void setTimeout(unsigned long seconds);

  //sets timeout for which to attempt connecting, usefull if you get a lot of failed connects
  void setConnectTimeout(unsigned long seconds);

  void setDebugOutput(boolean debug);
  //defaults to not showing anything under 8% signal quality if called
  void setMinimumSignalQuality(int quality = 8);
  //sets a custom ip /gateway /subnet configuration
  void setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //sets config for a static IP
  void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //called when AP mode and config portal is started
  void setAPCallback(void (*func)(WiFiManager *));
  //called when settings have been changed and connection was successful
  void setSaveConfigCallback(void (*func)(void));
  //adds a custom parameter
  void addParameter(WiFiManagerParameter *p);
  //if this is set, it will exit after config, even if connection is unsucessful.
  void setBreakAfterConfig(boolean shouldBreak);
  //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
  //TODO
  //if this is set, customise style
  void setCustomHeadElement(const char *element);
  //if this is true, remove duplicated Access Points - defaut true
  void setRemoveDuplicateAPs(boolean removeDuplicates);
  //Scan for WiFiNetworks in range and sort by signal strength
  //space for indices array allocated on the heap and should be freed when no longer required
  int scanWifiNetworks(int **indicesptr);

  void setConfSSID(String ssid) { _ssid = ssid; }
  void setConfPSK(String psk) { _pass = psk; }

private:
  String _ssid = "";
  String _pass = "";
  std::unique_ptr<DNSServer> dnsServer;
  std::unique_ptr<ESP8266WebServer> server;

  void setupConfigPortal();
  void startWPS();
  char *getStatus(int status);

  const char *_apName = "no-net";
  const char *_apPassword = NULL;
  unsigned long _configPortalTimeout = 0;
  unsigned long _connectTimeout = 0;
  unsigned long _configPortalStart = 0;
  /* hostname for mDNS. Set to a valid internet address so that user
    will see an information page if they are connected to the wrong network */
  const char *myHostname = "192.168.4.1";
  int numberOfNetworks;
  int *networkIndices;
  int **networkIndicesptr = &networkIndices;

  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;

  int _paramsCount = 0;
  int _minimumQuality = -1;
  boolean _removeDuplicateAPs = true;
  boolean _shouldBreakAfterConfig = false;
  boolean _tryWPS = false;

  const char *_customHeadElement = "";

  //String        getEEPROMString(int start, int len);
  //void          setEEPROMString(int start, int len, String string);

  int status = WL_IDLE_STATUS;
  int connectWifi(String ssid, String pass);
  uint8_t waitForConnectResult();

  void handleRoot();
  void handleWifi();
  void handleWifiSave();
  void handleServerClose();
  void handleInfo();
  void handleiSpindel();
  void handleState();
  void handleScan();
  void handleReset();
  void handleNotFound();
  void handleMnt();
  void handleOffset();
  void handleUpdating();
  void handleUpdateDone();
  boolean captivePortal();
  void reportStatus(String &page);

  // DNS server
  const byte DNS_PORT = 53;

  //helpers
  int getRSSIasQuality(int RSSI);
  boolean isIp(String str);
  String toStringIp(IPAddress ip);

  boolean connect;
  boolean stopConfigPortal = false;
  boolean _debug = true;

  void (*_apcallback)(WiFiManager *) = NULL;
  void (*_savecallback)(void) = NULL;

  WiFiManagerParameter *_params[WIFI_MANAGER_MAX_PARAMS];

  template <typename Generic>
  void DEBUG_WM(Generic text);

  template <class T>
  auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s))
  {
    return obj->fromString(s);
  }
  auto optionalIPFromString(...) -> bool
  {
    DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
    return false;
  }
};

#endif
