# Fragen und Antworten FAQ

- ***Es funktioniert nicht, ich sehe nichts***  
Am Besten du liest mit was die Spindel als Informationen ausgiebt. Das geht über einen Serial Monitor wie HTerm oder Arduino hat das auch. Die Spindel per Micro USB anschliessen, dann den Com Port mit obigen Serial Monitor öffnen und mitlesen. Das bringt Licht ins Dunkel.

- ***Wie komme ich auf die iSpindel - das Portal***  
Anschalten, 3-4 maliges drücken der Reset Taste stellt sie einen Access Point auf. D.h. du siehst ein neues WLAN "iSpindel" mit dem du dich verbindest. Jetzt wirst den Browser automatisch auf die Seite [http://192.168.4.1](http://192.168.4.1) dich weiterleiten. Falls nicht steuer sie selbst an.
Wenn du nun auf der Portalseite bist, unter `Configuration` dann kannst du deinen Token eingeben und das WLAN Passwort dass du für dein Netzwerk benötigst. dann klickst du auf `safe` und sie wird ihre Daten an Ubidots senden. Damit ist sie im Normalbetrieb und nicht mehr zu erreichen

- ***mein Wlan-Passwort bleibt nicht gespeichert***  
Das ist korrekt, wenn man das Portal wieder startet (reset Taste) werden die Wlan Daten gelöscht. Zur Zeit so implementiert.

- ***Wenn die Spindel außerhalb des Sekundenintvallblinkmodus ist, kann ich sie erst wieder erreichen wenn ich die Resettaste betätigt habe***  
Reset Taste wäre nur sinnvoll wenn man was ändern möchte. Normalerweise wacht sie selber auf nach dem Intervall und geht in 'Normalbetrieb'. 
Man kann die Spindel im Normalbetrieb _nicht_ erreichen, sie sendet ausschließlich. (Powermanagement)

- ***Warum kann ich auf die iSpindel nicht im Normalbetrieb zugreifen***  
Der kritische Teil der Laufzeit ist dass die iSpindel so kurz wie nur möglich aktiv ist und sich die maximale Zeit im `deep sleep`  befindet! Der Wemos im Aktiv - Betrieb hat einen über 300x Energiebedarf, um daher die Laufzeit zu erreichen ist dieser Trick unumgänglich.
- ***Die Micro USBBuchse ist für Programmaktualisierungen?***
Korrekt, siehe Firmwareaktualisierung
- ***Wie kann ich die Spannungsanzeige kalibrieren?***
Ab Firmware 3 kann man den Umrechnungfaktor anpassen. Standardmässig ist der Faktor 191.8, diesen umrechnen:
```
angezeigte Spannung / gemessene Spannung  * jetziger Faktor (191.8) = neuer Faktor
```
