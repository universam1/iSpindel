#genericTCP
####(iSpindle.py Version 0.2.1)

[English Version](README_en.md)

**Neu: Diagramme, siehe:**
[Charts Readme](web/README.md)

**Installations Anleitung für Raspbian:**
[INSTALL.md](INSTALL.md)

**Achtung: Update auf Firmware 5.x:**
Bitte die [Installations Anleitung](INSTALL.md) beachten.      

Dieses in Python geschriebene Server Skript dient dazu, von der iSpindel kommende Rohdaten über eine generische TCP Verbindung zu empfangen.
Auf zusätzlichen, unnötigen Overhead durch Protokolle wie http wird hierbei bewusst verzichtet, um den Stromverbrauch der iSpindel so gut es geht zu minimieren.
Die empfangenen Daten können als CSV (“Comma Separated Values”, also durch Kommas getrennte Werte) in Textdateien gespeichert (und so zum Beispiel in Excel leicht importiert) werden.
Ebenso ist es möglich, die Daten in einer MySQL Datenbank abzulegen.    
Somit hat man einen lokalen Server, der auch ohne Internet Anbindung funktioniert und den Einsatz der iSpindel im Heimnetzwerk ermöglicht.
Die Zugriffszeiten sind kürzer und dadurch sinkt natürlich auch der (ohnehin geringe) Stromverbrauch der iSpindel noch weiter.

Um nicht auf die Anbindung an Ubidots verzichten zu müssen, besteht aber auch die Option, die Daten zusätzlich dorthin weiterzuleiten.    
Das geschieht transparent, ohne dass die iSpindel auf die Verbindung warten muss. Der lokale Server fungiert dann sozusagen als Proxy.

Das Skript ist plattformunabhängig und kann z.B. auf einem Raspberry Pi, eingesetzt werden.
Aber auch der Einsatz auf einem evtl. gemieteten dedizierten (oder virtuellen) Server oder einem beliebigen Rechner im Heimnetz ist möglich.
Der Betrieb mehrerer iSpindeln gleichzeitig funktioniert problemlos und ohne Verzögerungen, da Multithreading implementiert ist.    
Getestet wurde es unter Mac OS X (Sierra) und Linux (Debian), es sollte aber auch unter Windows problemlos laufen.    
Die einzige Voraussetzung ist, dass Python installiert ist.

Für die Anbindung an MySQL muss auch der `python-mysql.connector` installiert sein.
In der Konfiguration der iSpindel wählt man **TCP** aus, und trägt die IP Adresse des Servers ein, auf dem das Skript läuft.
Als Port wählt man am besten die Voreinstellung **9501**.

Nun muss das Skript selbst unbedingt noch konfiguriert werden.
Dazu öffnet man es mit einem Text Editor und bearbeitet die gleich beschriebenen Einstellungen.

Dann wird es in einen beliebigen Pfad auf dem Zielsystem kopiert, `/usr/local/bin` bietet sich an, oder einfach `/home/pi`.
Mit `chmod 755 iSpindle.py` macht man es ausführbar und startet es mit `./iSpindle.py`.
Alternativ (z.B. unter Windows) startet man es mit `python iSpindle.py`.
Wenn alles funktioniert, beendet man das Skript, setzt `DEBUG = 0` und startet es im Hintergrund neu mit `./iSpindle.py &`.

###Konfiguration:

####Allgemeines:

	DEBUG = 0      
	PORT = 9501    
	HOST = '0.0.0.0'

Wenn **DEBUG** auf 1 gesetzt wird, werden auf der Konsole detaillierte Informationen ausgegeben.    
Während der ersten Inbetriebnahme ist dies sehr empfehlenswert.    
Falls der TCP **Port** 9501 bereits belegt ist (was unwahrscheinlich ist), kann man diesen auf einen anderen Wert einstellen.    
**HOST** legt fest, von welchen IP Adressen aus der Server erreichbar sein soll.    
Am besten lässt man das auf der Voreinstellung, also keine Einschränkungen.    
Von außerhalb des eigenen Netzwerks ist Port 9501 normalerweise sowieso nicht zu erreichen, es sei denn man konfiguriert den Router entsprechend (Port Forwarding).

####CSV:

	CSV = 0
	OUTPATH = '/home/pi/iSpindel'
	DELIMITER = ';'
	NEWLINE = '\r\n'
	DATETIME = 1    

Um die Ausgabe von CSV Dateien einzuschalten, muss **CSV** = 1 gesetzt werden.   
**OUTPATH** ist der Pfad, unter dem die CSV Datei gespeichert wird. Pro Spindel wird hierbei jeweils eine eigene Datei angelegt.
Wenn OUTPATH im Netzwerk freigegeben ist kann man sehr leicht eine solche Datei z.B. in Excel importieren.   
**DELIMITER** legt das Zeichen fest, durch welches die einzelnen Datenfelder separiert sind. Üblicherweise wählt man hier Komma oder Semikolon.   
**NEWLINE** definiert das Zeilenende. Ist die CSV Datei für Windows oder Office für Mac bestimmt, gilt die Voreinstellung.   
Für UNIX Systeme (Linux, Mac OS X) wählt man besser '\n'.   
**DATETIME** legt fest, ob das aktuelle Datum und Uhrzeit mit in die CSV Datei geschrieben werden sollen. Normalerweise dürfte das der Fall sein.

####MySQL

	SQL = 1
	SQL_HOST = '127.0.0.1'
	SQL_DB = 'iSpindle'
	SQL_TABLE = 'Data'
	SQL_USER = 'iSpindle'
	SQL_PASSWORD = 'xxxxxxxx'

Will man auf MySQL verzichten, setzt man **SQL** = 0.    
**SQL\_HOST** definiert die IP Adresse der Datenbank. Normalerweise ist das derselbe Rechner auf dem auch das Skript läuft; dies ist die Voreinstellung (“localhost” oder 127.0.0.1).    
Die restlichen Felder definieren den Zugang zur Datenbanktabelle.    
Die Default Einstellung geht davon aus, dass die Datenbank “iSpindle”; heißt, ebenso die ID des Datenbank Users.    
Die Tabelle, in welcher die Daten landen, heißt “Data”.    
Um die Tabelle mit den grundlegenden Feldern anzulegen, verwendet man am besten dieses SQL Statement:

	CREATE TABLE 'Data' (
		'Timestamp' datetime NOT NULL,
		'Name' varchar(64) COLLATE ascii_bin NOT NULL,
		'ID' varchar(64) COLLATE ascii_bin NOT NULL,
		'Angle' double NOT NULL,
		'Temperature' double NOT NULL,
		'Battery' double NOT NULL,
		'Gravity' double,
		PRIMARY KEY ('Timestamp', 'Name', 'ID')
	) 
	ENGINE=InnoDB DEFAULT CHARSET=ascii 
	COLLATE=ascii_bin COMMENT='iSpindle Data';

Als Datenbankbenutzer kann man natürlich das vordefinierte Admin Konto verwenden, oder aber (schöner) man legt einen Benutzer an, mit Zugriff auf diese Datenbank:

	CREATE USER 'iSpindle'@'localhost' IDENTIFIED BY 'password';
	GRANT ALL PRIVILEGES ON iSpindle . * TO 'iSpindle'@'localhost';
	FLUSH PRIVILEGES;

Weitere Felder können vom Skript dynamisch angelegt werden; dies ist aber nur zu empfehlen, wenn man eine eigene Firmware testen will, die zusätzliche Variablen ausgibt.    
Hierzu wird **ENABLE\_ADDCOLS** = 1 gesetzt. Für den Normalbetrieb ist egal, ob hier 0 oder 1 eingestellt ist.    
Falls der Server nach außen offen ist (z.B. extern gehostet), empfehle ich aber (in dieser Version des Skripts) ENABLE\_ADDCOLS auf 0 zu setzen um eventuelle SQL Injections zu verhindern(!).


####Ubidots Anbindung

	UBIDOTS = 1
	UBI_TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'

**UBIDOTS** = 0 schaltet die Ubidots Weiterleitung aus.    
In **UBI\_TOKEN** das eigene Token eintragen (siehe Dokumentation der iSpindel) und das war's auch schon.

Die Daten sollten nun sowohl wie gewohnt in Ubidots erscheinen als auch auf Eurem lokalen Server.
Auch neue iSpindeln (Devices) lassen sich so problemlos anlegen, für Ubidots macht es keinen Unterschied, ob die Daten von der iSpindel direkt kommen oder vom lokalen Server.

Viel Spaß,     
Euer Tozzi (stephan@sschreiber.de)
