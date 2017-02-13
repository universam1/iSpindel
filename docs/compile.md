# Compile from Sources

## Requirements

- Arduino 1.6.11 - 13
- ESP8266 core for Arduino
- ArduinoJson
- SDK ***1.5.4*** 
> Make sure not to use <= **SDK 1.5.3** since this is know to have issues!


## Used libraries

- https://github.com/esp8266/Arduino
- https://github.com/bblanchon/ArduinoJson

## Note

Do **not** use the Board Manager to install the ESP8266 core.    
Clone from github into ~/Documents/hardware instead, as described in the documentation.
Else, you will likely end up using the wrong SDK and run into problems.
