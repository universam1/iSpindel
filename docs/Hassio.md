# Home Assistant
Die iSpindel unterstützt den Datentransfer per MQTT-Protokoll, das Hausautomationssystem normalerweise verarbeiten können. 

Ab der iSpindel Firmware Version 7.2 unterstützt iSpindel die Discovery Funktion für MQTT Daten des **<em>Home Assistant</em>**. Damit entfällt die manuelle MQTT-Sensor Konfiguration in **<em>Home Assistant</em>** und die Daten können sehr einfach verwendet werden.

Bei der Suche nach **<em>Home Assistant</em>** und iSpindel findet man eine weitere Implentierung auf der Basis von ESPHome. Bei dieser wird nicht die Standard-iSpindel Firmware eingesetzt. ESPHome verwendet yaml-Code um die Logik des Microcontrolers zu beschreiben und generiert hieraus dann die Firmware.

## Home Assistant vorbereiten

- MQTT-Broker in **<em>Home Assistant</em>** konfigurieren. 
Falls der MQTT-Broker in **<em>Home Assistant</em>** noch nicht installiert und konfiguriert ist, bitte einer der zahlreichen Tutorial im Netz verwenden.

   Für die iSpindel Konfiguration benötigte MQTT-Broker Informationen:
   
   - Benutzername des MQTT-Accounts
   - Kennwort des MQTT-Account
   - IP Address des Hosts auf dem der MQTT-Broker läuft. Die Verwendung eines Hostnamens anstelle der IP Adresse wird häufig scheitern.
   - IP Port-Nummer des MQTT-Services (üblich: 1883)

- Für vom **<em>Home Assistant</em>** unabhängige Untersuchungen ist die Installation des MQTT-Explorers hilfreich.

## iSpindel für den **<em>Home Assistant</em>** konfigurieren


## iSpindel Daten in **<em>Home Assistant</em>** nutzen

## Beispiele Automation rund um iSpindel