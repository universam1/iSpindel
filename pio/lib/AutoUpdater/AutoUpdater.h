/**************************************************************

    "iSpindel"
    changes by S.Lang <universam@web.de>

 **************************************************************/

#ifndef _AUTOUPDATER_H_
#define _AUTOUPDATER_H_

#include "Globals.h"

class AutoUpdaterClass
{
public:
  AutoUpdaterClass();
  // ~AutoUpdaterClass();

private:
  String getLatestFWVersion(String &, String &);
  bool isNewer(String current, String upstream);
  bool flash(String url);
  // WiFiClient _client;
  // // StaticJsonBuffer<200> _jsonBuffer;
  // DynamicJsonBuffer _jsonBuffer;
  // // JsonObject data;
  // JsonVariant _jsonVariant;
};

#endif
