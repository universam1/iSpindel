#Installationsanleitung für Raspberry Pi (Raspbian)
###Schritt-für-Schritt

###Vorbemerkung:
Es mag so aussehen, als würde hier viel zu viel Ballast installiert, das geht alles auch schlanker.
Stimmt.
Aber die meisten werden ja Raspbian verwenden, und das ist schon so aufgebläht dass ich da kein schlechtes Gewissen habe, noch Apache, Samba und MySQL mit draufzupacken.
Der Raspi 3 schafft das sowieso problemlos.
Wer auf phpmyadmin verzichten will, also auf die Möglichkeit, die Datenbank mit einer html Oberfläche zu administrieren, kann den Apache2 und phpmyadmin problemlos auch weglassen.

###Raspbian vorbereiten
raspi-config: ssh einschalten     
Verbinden mit putty (Windows) oder Terminal (Mac OS X, Linux) und ssh:

	ssh pi@[ip-adresse oder hostname] 
	Passwort: raspberry (ändern)

System auf neuesten Stand bringen:

	sudo apt-get update
	sudo apt-get dist-upgrade

###Optional: MySQL

####Installieren:

	sudo apt-get install apache2 mysql-server mysql-client php5-mysql python-mysql.connector

Passwort für Datenbank root Benutzer eingeben

	sudo apt-get install phpmyadmin

pache2 als Webserver auswählen, Datenbank root Passwort wieder eingeben.
MySQL kann nun über http://[meinraspi]/phpmyadmin erreicht werden.

Die folgenden Schritte lassen sich über phpadmin im Browser erledigen (dazu als root einloggen und oben den Reiter "SQL" anklicken) oder über die Kommandozeile machen, dazu einfach aus dem Terminal aufrufen:

	mysql -u root -p

Ihr werdet aufgefordert, das obige Passwort wieder einzugeben.
Danach landet Ihr auf einem **mysql>** Prompt.

####Datenbank erstellen und auswählen:
	CREATE DATABASE iSpindle;
	USE iSpindle;

####Tabelle anlegen:

	CREATE TABLE `Data` (
 		`Timestamp` datetime NOT NULL,
 		`Name` varchar(64) COLLATE ascii_bin NOT NULL,
 		`ID` varchar(64) COLLATE ascii_bin NOT NULL,
 		`Angle` double NOT NULL,
 		`Temperature` double NOT NULL,
 		`Battery` double NOT NULL,
 	PRIMARY KEY (`Timestamp`,`Name`,`ID`)
	) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin COMMENT='iSpindle Data'


####Benutzer anlegen und berechtigen (und ihm ein eigenes Passwort geben):

	CREATE USER 'iSpindle' IDENTIFIED BY 'xxxxxxxxxx'
	GRANT USAGE ON *.* TO 'iSpindle';
	GRANT ALL PRIVILEGES ON `iSpindle`.* TO 'iSpindle' WITH GRANT OPTION;

Ab sofort steht die MySQL Datenbank für die iSpindel zur Verfügung.
Das Server Skript hierzu wie in README beschrieben anpassen und neu starten.


###Optional: Samba installieren (empfohlen):

	sudo apt-get samba samba-common-bin

####Home Verzeichnis im Netzwerk freigeben:

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

####Samba daemon (smbd) starten

	sudo insserv smbd
	sudo service smbd start

Das pi Home Verzeichnis ist nun im Heimnetzwerk freigegeben und Ihr könnt es im Explorer/Finder Eures Computers sehen.

###Das genericTCP Skript installieren
Zunächst das Skript konfigurieren, wie im README beschrieben.    
Die Dateien iSpindle.py und ispindle-srv in das mit Samba freigegebene Verzeichnis kopieren.    
Dann wieder auf dem Raspi im ssh Terminal eingeben:

	sudo mv ~./iSpindle.py /usr/local/bin
	sudo mv ~/ispindle-srv /etc/init.d
	sudo insserv ispindle-srv
	sudo service ispindle-srv start

Ein guter Zeitpunkt, den Raspi neu zu starten (ist aber nicht nötig):

	sudo reboot

Nach erneuter Verbindung sollte nun "ps -ax | grep iSpindle" einen laufenden iSpindle.py Prozess anzeigen.

Ihr habt jetzt die längstmögliche Batterielaufzeit für die iSpindel und habt Eure Daten auch lokal vorhanden, falls Ubidots mal aussetzen sollte oder Ihr Eure eigenen Visualisierungen machen wollt.

Auch von mir wird da sicher noch was kommen.

Euer Tozzi (stephan@sschreiber.de)



