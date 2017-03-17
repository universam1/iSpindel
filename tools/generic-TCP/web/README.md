# Diagramme vom lokalen Server 

[English Version](README_en.md)

OK, der lokale Server ist im Einsatz und funktioniert, jetzt wollen wir auch die Daten darstellen können, notfalls auch ohne Internetverbindung.     
Zu was war das Ganze sonst gut?  
Man kann zwar jederzeit per Excel eine CSV Datei importieren oder auch über ODBC auf die Datenbank zugreifen, aber das ist ein wenig umständlich.
Also musste noch eine einfache Methode her, um direkt mit dem Browser Diagramme anzeigen zu können.

Wir haben Apache2 und MySQL auf dem Raspi (oder einem anderen Server) installiert, quasi Overkill, haben also jede Menge Möglichkeiten.      

Ich habe mich für eine Lösung auf Basis von Highcharts entschieden (www.highcharts.com), welches für den Privatgebrauch kostenlos ist und eine schier unglaubliche Menge an Darstellungen bietet.
Da habe ich bisher nur an der äußersten Schicht gekratzt.       
Weiterhin verwende ich jQuery (jquery.com), um eventuell noch eine richtig gute AJAX Implementierung hinzubekommen, sowie moment.js (momentjs.com) für die Zeitzonen Problematik.      
Die Sommerzeit Umstellung sollte also automatisch berücksichtigt werden, werden wir ja bald sehen ob das gut funktioniert so.

Ansonsten immer noch treu dem Motto, so einfach aber effektiv wie möglich... ;)      
Es steckt nach wie vor wahrlich kein Hexenwerk dahinter.

Ich habe insgesamt folgende verschiedene Diagramme implementiert:

* angle.php - zeigt Winkel und Temperatur für vergangene X Stunden
* plato.php - zeigt Restextrakt und Temperatur für vergangene X Stunden bei **kalibrierter** iSpindel (ab Firmware 5.x)
* plato4.php - zeigt Restextrakt und Temperatur für vergangene X Stunden bei **nach alter Methode kalibrierter iSpindel** (Firmware 4.x)
* battery.php - zeigt den Ladezustand einer Spindel
* status.php - zeigt Ladezustand, Winkel und Temperatur einer Spindel, wie aktuell gemessen

Um das entsprechende Diagramm aufzurufen werden die Parameter per GET Methode übergeben.
Beispiele:

* http://meinraspi/iSpindle/angle.php?name=MeineSpindel1&hours=24
* http://meinraspi/iSpindle/status.php?name=MeineSpindel2

Mit reset_now kann man einen Zeitstempel (Beginn der Gärung) festlegen und bei der Grafik alle Werte nach diesem Zeitstempel anzeigen:
* http://meinraspi/iSpindle//reset_now.php?name=MeineSpindel2
* http://meinraspi/iSpindle/angle.php?name=MeineSpindel2&reset=true

Dafür kann man dann Lesezeichen anlegen und so auch auf einem Touchscreen schnell zur gewünschten Ansicht kommen.     

Ich hoffe, damit einen sinnvollen Grundstein gelegt zu haben, auf dem Ihr aufbauen könnt.
Für mich persönlich genügt das jetzt erst mal so wie es ist, aber es gibt natürlich eine Menge Verbesserungspotenzial.
Gerne beantworte ich auch Eure Fragen, sollte irgendwas nicht ausreichend kommentiert oder sonstwie unklar sein.

Auf Eure Weiterentwicklungen (die ja normalerweise aus eigenen Bedürfnissen entstehen) freue ich mich sehr.
Um Sam da zu entlasten, würde ich Euch aber bitten, dafür mein [Repository](https://github.com/DottoreTozzi/iSpindel) zu klonen und Eure Pull Requests zuerst an mich zu schicken.
Ich teste das dann und gebe die Änderungen an Sam weiter.

Die externen Libraries habe ich mit hier aufgenommen, um die Daten auch ohne Internet darstellen zu können.

### Noch ein paar Hinweise zur Installation:
#### Apache2:
Damit Apache die Diagramme "sehen" kann, müssen sie irgendwie in **/var/www/html** (oder einem Unterverzeichnis dort) zu finden sein.
Ich mache das mittels Symlink auf mein GIT Arbeitsverzeichnis, somit ist der Webserver nach GIT PULL sofort auf dem neuesten Stand:

      cd /var/www/html    
      sudo ln -s ~/iSpindel/tools/genericTCP/web/ iSpindle

#### Datenbankschnittstelle:
Um die Verbindung zur Datenbank herzustellen, muss die Datei include/common_db.php editiert werden:

      // configure your database connection here:
      define('DB_SERVER',"localhost");
      define('DB_NAME',"iSpindle");
      define('DB_USER',"iSpindle");
      define('DB_PASSWORD',"xxxx");

#### Kalibrierung nach "alter" Methode (Firmware < 5.0.1) 
Um plato4.php nutzen zu können, und den umgerechneten Restextrakt auszugeben, muss für jede iSpindel ein Datensatz hinterlegt werden.    
Die Verknüpfung erfolgt nicht über den Namen der Spindel, sondern über ihre Hardware ID, bleibt also erhalten wenn man die iSpindel umbenennt.    

Dazu brauchen wir erst eine neue Tabelle in der Datenbank.    
Ein Skript für beide Tabellen findet sich [hier](../MySQL_CreateTables.sql).    

Das Schema der neuen Tabelle:
     
     CREATE TABLE `Calibration` (
     `ID` varchar(64) COLLATE ascii_bin NOT NULL,
     `const1` double NOT NULL,
     `const2` double NOT NULL,
     `const3` double NOT NULL,
     PRIMARY KEY (`ID`)
     ) 
     ENGINE=InnoDB DEFAULT CHARSET=ascii 
     COLLATE=ascii_bin COMMENT='iSpindle Calibration Data';


ID ist die Hardware ID der Spindel. Diese wird mit übermittelt und gespeichert.     
const1, 2 und 3 sind die drei Koeffizienten des ermittelten Polynoms:

const1 * winkel<sup>2</sup> - const2 * winkel + const3

Am besten die Werte mit phpMyAdmin eintragen, oder:

    INSERT INTO Calibration (ID, const1, const2, const3)
    VALUES ('123456', 0.013355798, 0.776391729, 11.34675255);

Die Spindel Hardware ID kann aus der Daten Tabelle ermittelt werden, sie wird von der Spindel mitgeschickt.    

Viel Spaß,     
Tozzi       
<stephan@sschreiber.de>
