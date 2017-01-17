iSpindle.py - Version 0.0.1

Das Python Skript ist sehr einfach gehalten und kann leicht an eigene Bedürfnisse angepasst werden.
Es lauscht auf einem TCP Port und schreibt die Daten dann in eine CSV Datei, die z.B. in Excel eingelesen werden kann.
Für meine Zwecke reicht das erst mal. Meine Devise war erst mal "keep it simple".
Weitere Sachen wie Datenbank Anbindung etc. kann man ja immer noch einbauen.
Jeder der mag ist herzlich eingeladen das Skript zu verbessern.

Zur Konfiguration muss man das Skript in einem Editor öffnen und dann die folgenden Zeilen anpassen:

DEBUG = 1                               # Set to 1 to enable debug output on console
PORT = 9501                             # TCP Port to listen to
OUTPATH = '/media/usb0/iSpindel/csv/'   # CSV output file path
DELIMITER = ';'                         # CSV delimiter (use ; for Excel)
NEWLINE='\r\n'                          # newline (\r\n for windows clients)
DATETIME = 1                            # Set to 1 to enable appending of Excel compatible timestamp to CSV file

Außer dem Pfad zur CSV Datei braucht man da eigentlich nichts zu ändern, normalerweise.
Wenn das Verzeichnis im Netzwerk freigegeben ist, kann man die Daten direkt in Excel etc. importieren.
Der Name der CSV Datei ist SpindelName_SpindelID.csv, d.h. jede Spindel schreibt in eine eigene Datei.

Danach kann man das Skript einfach starten.
Unter Linux am besten einfach ausführbar machen (chmod 755 iSpindle.py).
Ansonsten halt "python iSpindle.py".
Entwickelt wurde das Skript für Python Version 2.7.
Um den Konsolen Spam auszuschalten, DEBUG = 0.

In der Konfiguration der Spindel Firmware gibt es jetzt einen Punkt "TCP".
Dort den Port (9501) und die IP Adresse des Servers eintragen und wie gehabt Update-Intervall etc.

Viel Spaß!
Stephan (stephan@sschreiber.de)
