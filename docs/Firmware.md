# Firmware

## Flashen über Web Interface

- download the latest .bin firmware from this repository 
### https://github.com/universam1/iSpindel/releases

- in `Maintenance` menu select this file and press update

![flash web](/pics/maintenance.png)

## Flashen über USB

- Download und Starten von [NodeMCU-Flasher](https://github.com/nodemcu/nodemcu-flasher/raw/master/Win32/Release/ESP8266Flasher.exe)
- neuestes Release aus [/bin Folder](https://github.com/universam1/iSpindel/raw/master/bin/) downloaden
> Wichtig: ```Download``` button wählen um die Datei korrekt herunterzuladen
- COM Port den Wemos auswählen
- Tab ```Config``` als erste Datei die ```.bin``` auswählen
- Tab ```Flashen``` klicken, Fortschrittsbalken ist zu sehen
- Nach erfolgreichem Flash ```RESET``` Taste drücken

> Evtl. über Serial Monitor (bsp. Arduino IDE) die Statusmeldungen sehen.

> Der ```Config Mode``` ist aktiv wenn die LED im 1s Rhythmus blinkt. Man kann ihn beenden über Link im Portal oder über Timeout 5min.
