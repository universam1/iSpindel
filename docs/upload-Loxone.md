# Datenupload nach Loxone


iSpindel kann die Daten an einen lokalen [Loxone](http://loxone.com) Miniserver übertragen. Somit können die iSpindel-Werte innerhalb der Loxone Apps und des Web Interfaces visualisiert werden. Durch eine Auswertung kann die Funktionalität erweitert werden z.B. um eine Steuerung der Gärtemperatur, oder einer Benachrichtigung, sobald die Gärung abgeschlossen ist.

![Loxone Visualisierung](/pics/loxone-app-screenshot.png)


## Funktionsweise

Die iSpindel überträgt die Daten als Text via Http Get mit Authentifizierung an einen Virtuellen Texteingang im Loxone Miniserver. Ein PicoC-Programm wertet den Text aus, d.h. zerlegt ihn in die unterschiedlichen Werte und stellt die Daten an den Ausgängen TQ1 (iSpindel-Name), AQ1 (Tilt), AQ2 (berechneter Plato-Wert), AQ3 (Temperatur) und AQ4 (Akkuspannung) bereit.


## Einrichtung in Loxone

In /tools/loxone befindet sich ein Beispielprojekt für Loxone Config. Die Komponenten einfach mit Copy und Paste in das eigene Projekt einfügen. Beim Einfügen wird leider der Virtuelle Texteingang nicht richtig übernommen, daher muss dieser neu angelegt werden. Dazu einen neuen „Virtuellen Texteingang“ einfügen und mit dem iSpindel-Programmbaustein am Eingang TI1 verbinden. Jetzt noch die Formel für die Plato-Berechnung im iSpindel-Programmbaustein ändern (die Zeile ist im PicoC-Code markiert). Nach Belieben die maximale Updatefrequenz im Skript anpassen – in der vorletzten Zeile "sleep(5000);" wartet 5 Sekunden, bis der Input erneut ausgewertet wird. So kann z.B. zu Testzwecken öfter aktualisiert werden – oder seltener, um den Miniserver zu entlasten.

![Loxone Konfiguration](/pics/loxone-Miniserver-configuration.png)


## Einrichtung iSpindel

In der Konfiguration den Service Type auf „Loxone“ stellen, die Server Adresse, Portnummer, Loxone Login-Name und -Passwort eintragen. In der Server URL muss die Adresse von dem Virtuellen Texteingang stehen; "/dev/sps/io/<Bezeichnung des Virtuellen Texteingangs>/". Bei mir ist das: "/dev/sps/io/VTI1/".

![iSpindel Konfiguration](/pics/loxone-iSpindel-configuration.jpg)


## Sicherheitshinweis

Benutzername und Passwort werden in der iSpindel in Klartext gespeichert und übertragen. Somit kann theoretisch jeder, der Zugang zum selben Netzwerk hat, den Netzwerkverkehr abhören und die Zugangsdaten ermitteln. Aus Sicherheitsgründen sollte iSpindel nur eingesetzt werden, wenn auf den Loxone Miniserver ausschließlich im lokalen Netzwerk oder über VPN zugegriffen wird.
