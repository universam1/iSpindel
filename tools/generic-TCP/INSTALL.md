# Installationsanleitung für Raspberry Pi (Raspbian)
### Schritt-für-Schritt

[English Version](INSTALL_en.md)

### Update auf Firmware Version 5.x:
Um die neue Firmware einsetzen zu können, bitte das Skript auf die neueste Version aktualisieren.     
Falls die Datenbank schon besteht, muss in die Tabelle "Data" ein neues Feld eingefügt werden:

	USE iSpindle;
	ALTER TABLE Data ADD Gravity double NOT NULL DEFAULT 0;

Bei einer Neuinstallation ist dies bereits im Folgenden berücksichtigt.

### Vorbemerkung:
Es mag so aussehen, als würde hier viel zu viel Ballast installiert, das geht alles auch schlanker.
Stimmt.
Aber die meisten werden ja Raspbian verwenden, und das ist schon so aufgebläht dass ich da kein schlechtes Gewissen habe, noch Apache, Samba und MySQL mit draufzupacken.
Der Raspi 3 schafft das sowieso problemlos, aber auch die kleineren Modelle sollten klarkommen.
Die ganze Installation braucht gute 5GB, also eine SD mit 8 GB sollte reichen, ab 16GB reicht der Speicherplatz auf jeden Fall.
Wer auf die Diagramme und auf phpmyadmin verzichten will, also auf die Möglichkeit, die Datenbank mit einer html Oberfläche zu administrieren, kann den Apache2 und phpmyadmin auch weglassen.

### Raspbian vorbereiten
- raspi-config: ssh einschalten, Netzwerkverbindung herstellen (hierfür braucht man einmalig eine Tastatur und einen HDMI Bildschirm)             
- Oder: Beim Vorbereiten der SD Karte im /boot Verzeichnis eine leere Datei "ssh" anlegen. Dann kann man sich direkt über einen anderen Rechner im Netzwerk per SSH einloggen und braucht weder Bildschirm noch Tastatur am Raspi.
- Verbinden mit putty (Windows) oder Terminal (Mac OS X, Linux) und ssh:

	ssh pi@[ip-adresse oder hostname] 
	Passwort: raspberry (ändern)

System auf neuesten Stand bringen:

	sudo apt-get update
	sudo apt-get dist-upgrade

### MySQL Datenbank, Apache2 Webserver und phpMyAdmin Datenbank GUI 

#### Installieren:

	sudo apt-get install apache2 mysql-server mysql-client php5-mysql python-mysql.connector

Passwort für Datenbank root Benutzer eingeben.

	sudo apt-get install phpmyadmin

Apache2 als Webserver auswählen, Datenbank root Passwort wieder eingeben.
MySQL kann nun über http://[meinraspi]/phpmyadmin erreicht werden.

Die folgenden Schritte lassen sich über phpadmin im Browser erledigen (dazu als root einloggen und oben den Reiter "SQL" anklicken) oder über die Kommandozeile machen, dazu einfach aus dem Terminal aufrufen:

	mysql -u root -p

Ihr werdet aufgefordert, das obige Passwort wieder einzugeben.        
Danach landet Ihr auf einem **mysql>** Prompt.

#### Datenbank erstellen und auswählen:
	CREATE DATABASE iSpindle;
	USE iSpindle;

#### Tabelle(n) anlegen:

Am besten gleich beide Tabellen (Daten und Kalibrierung) anlegen.       
[Hier](./MySQL_CreateTables.sql) ist ein [SQL Skript](./MySQL_CreateTables.sql) für beide.        
Die Datentabelle folgt diesem Schema:      

	CREATE TABLE `Data` (
 		`Timestamp` datetime NOT NULL,
 		`Name` varchar(64) COLLATE ascii_bin NOT NULL,
 		`ID` varchar(64) COLLATE ascii_bin NOT NULL,
 		`Angle` double NOT NULL,
 		`Temperature` double NOT NULL,
 		`Battery` double NOT NULL,
		`Gravity` double NOT NULL DEFAULT 0,
 	PRIMARY KEY (`Timestamp`,`Name`,`ID`)
	) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin COMMENT='iSpindle Data'

(Im Feld ID wird die Hardware ID abgelegt, welche wir zum Hinterlegen der Kalibrierung benötigen.)     

#### Benutzer anlegen und berechtigen (und ihm ein eigenes Passwort geben):

	CREATE USER 'iSpindle' IDENTIFIED BY 'xxxxxxxxxx';
	GRANT USAGE ON *.* TO 'iSpindle';
	GRANT ALL PRIVILEGES ON `iSpindle`.* TO 'iSpindle' WITH GRANT OPTION;

Ab sofort steht die MySQL Datenbank für die iSpindel zur Verfügung.        
Das Server Skript hierzu wie im [README](./README.md) beschrieben anpassen und neu starten.


### Optional: Samba installieren (empfohlen):

	sudo apt-get install samba samba-common-bin

#### Home Verzeichnis im Netzwerk freigeben:

/etc/samba/smb.conf:

	[global]
 	server string = RASPBIAN
 	guest ok = yes
 	security = user
 	socket options = TCP_NODELAY SO_RCVBUF=65535 SO_SNDBUF=65535
 	registry shares = yes
 	syslog = 0
 	map to guest = bad user
 	workgroup = WORKGROUP
 	bind interfaces only = No
 	encrypt passwords = true
 	log level = 0
	# smb ports = 445
 	unix extensions = No
 	wide links = yes

 	include = /etc/samba/user.conf
 	include = /etc/samba/shares.conf


/etc/samba/shares.conf:

	[pi-home]
    	path = /home/pi
    	guest ok = yes
    	read only = no
    	force user = pi
    	browseable = yes

	[system-logs]
    	path = /var/log
    	guest ok = yes
    	read only = yes
    	force user = root
    	browseable = yes

#### Samba daemon (smbd) starten

	sudo insserv smbd
	sudo service smbd start

Das pi Home Verzeichnis ist nun im Heimnetzwerk freigegeben und Ihr könnt es im Explorer/Finder Eures Computers sehen.

### Das genericTCP Skript installieren
Zunächst das Skript konfigurieren, wie im README beschrieben.    
Die Dateien iSpindle.py und ispindle-srv in das mit Samba freigegebene Verzeichnis kopieren.    
Dann wieder auf dem Raspi im ssh Terminal eingeben:

	sudo mv ~./iSpindle.py /usr/local/bin
	sudo mv ~/ispindle-srv /etc/init.d
	sudo chmod 755 /usr/local/bin/iSpindle.py
	sudo chmod 755 /etc/init.d/ispindle-srv
	cd /etc/init.d
	sudo systemctl deamon-reload
	sudo insserv ispindle-srv
	sudo service ispindle-srv start

Ein guter Zeitpunkt, den Raspi neu zu starten (ist aber nicht nötig):

	sudo reboot

Nach erneuter Verbindung sollte nun "ps -ax | grep iSpindle" einen laufenden iSpindle.py Prozess anzeigen, so in der Art:     

	23826 ?        S      0:00 python2.7 /usr/local/bin/iSpindle.py
	23845 pts/0    R+     0:00 grep iSpindle

Ihr habt jetzt die längstmögliche Batterielaufzeit für die iSpindel und habt Eure Daten auch lokal vorhanden, falls Ubidots mal aussetzen sollte oder Ihr Eure eigenen Visualisierungen machen wollt.

Ein paar (für mich ausreichende) Diagramme habe ich [hier](/web) bereitgestellt.

Euer Tozzi (stephan@sschreiber.de)



