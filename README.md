iSpindel
===================

***https://universam1.github.io/iSpindel/***

**DIY elektronische Bierspindel | electronic Hydrometer**

> Die ```iSpindel``` befindet sich zur Zeit mitten in der Entwicklung, siehe dazu [den Hobbybrauer.de Thread](http://hobbybrauer.de/forum/viewtopic.php?f=7&t=11235). Mitthilfe ist herzlich willkommen und Ergänzungen bitte per Pull Request einreichen. Großes Dank an alle die Unterstützung leisten seitens Grundlagen erarbeiten oder passende Hardware zu finden.

![Spindel im Reinwasser](docs/floating.jpg)
![Dashboard](docs/Dashboard.jpg)


## Inhaltverzeichnis
- [Lizenz](#Lizenz)
- [Prinzip](#Prinzip)
  - [Metazentrum](#Metazentrum)
- [Aufbau](#Aufbau)
  - [Bauteile](#Bauteile)
  - [Bauteile](#Bauteile)
  - [Platine](#Platine)
- [Konfiguration](#Konfiguration)
  - [Portal](#Portal)
  - [Ubitdots](#Ubitdots)
- [Software](#Software)


***

## Lizenz
> Jeglicher kommerzieller Nachbau oder Verwendung ist prinzipiell untersagt. Geltende Patente werden verletzt.

> All rights reserverd, any commercial use is hereby prohibited and will violate applicable patents.

***

## Prinzip

Angetrieben von dem Thread [Alternative zu Spindel](http://hobbybrauer.de/forum/viewtopic.php?f=7&t=11157&view=unread#p170499) wurde die Idee geboren die kommerziell erhältliche elektronische Bierspindel nachzubauen.

Die Idee des krängende Zylinders ist genial wie einfach - man braucht keine externe Referenz (außer der Erde) und der Zylinder ist extrem leicht sauber zu halten. Der Neigungswinkel ändert sich im Verhältnis zum Auftrieb und damit im direkten Zusammenhang mit dem Zuckergehalt.
Kein unnötiges Öffnen zum spindeln und evtl. Kontaminieren!

![Kränung](docs/kraengung.jpg)

Daher die Idee, ein IoT device mit Wifi zusammen mit einem Beschleunigungssensor und Temperatursensor in einen schwimmenden Zylinder zu legen. Dort wacht er alle bsp. 5min auf, verbindet sich mit meinem Wlan und sendet seinen Neigungswinkel, Temperatur und Akkuspannung an einen Cloudservice. 

### *Metazentrum*

Eigentlich dreht es sich hier um das "Metazentrum", der Zylinder wird solange rotieren bis das Metazentrum im Lot zum Auftriebspunkt ist. Diesen Wert messen wir.

Man kann insofern trimmen, dass man am Boden ein paar Gramm hinzufügt, damit stellt sich der Zylinder mehr auf, oder am Deckel, damit krängt er mehr. 

In der Software berechnent man aus den XYZ Beschleunigungswerten den Euler Winkel für X und Y und bildet daraus den Absolutwinkel. Diesen verrechnen wir mit den kalibrierten Eckwerten zum °Plato.


***

## Aufbau

>***ACHTUNG: dies entspricht Stand 20.11.2016***

### Bauteile

- [Wemos D1 mini](https://www.wemos.cc/product/d1-mini.html)
- ```GY-521``` Gyro & Beschleunigungssensor (MPU-6050 auf passendem breakout Board)
- [DS18B20 Temperatursensor](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html)
- Lochrasterplatine 3x4cm
- Widerstände
  - 4k7 Ohm
  - 220k Ohm
  - 470 Ohm
- Mikro Schalter
- ```18650 LiIo Zelle``` (z.B. ```Panasonic NCR18650B``` **geschützt** oder ohne **PCB**) ***UNGETESTED***
- Lipo Lademodul ```TP4056``` ***UNGETESTED***
- Kunstoff Schlitten (Vorlage zum 3D drucken im repo)
  - alternativ (Lochraster) Platine ***UNGETESTED***)

- Kunstoff Zylinder ```Petling```

> ## Info
> Der Anbieter [cachers-world.de](http://cachers-world.de/de/Petling-XL) unsterstützt dieses Projekt indem er nachhaltig den passenden Petling liefern möchte und über den Gutschein-Code "```HOBBYBRAUER```"  (Großschreibung!) 20% Rabatt gewährt. 
Dieser [Petling-XL](http://cachers-world.de/de/Petling-XL) passt zu dem 3D gedruckten Schlitten.

***

### Schlitten

![Schlitten](docs/Schlitten_cad.jpg)
![Zusammenbau](docs/assembled2.jpg)
![Zusammenbau](docs/assembled.jpg)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=gpVarh8BxhQ" target="_blank"><img src="http://img.youtube.com/vi/gpVarh8BxhQ/0.jpg" 
alt="Druck" width="240" height="180" border="10" /></a>

### Platine 

***(TODO)***


***

## Konfiguration

### Portal

Durch mehrmaliges Drücken der ```Reset Taste``` erstellt der Wemos einen AccessPoint, mit dem verbunden man die nötigen Einstellugen vornehmen kann:

![AccessPoint](docs/AP.png)![Portal](docs/Portal.png)![Setup](docs/setup.png)![Info](docs/info.png)

### Ubitdots

***(TODO)***

***

## Software 

***(TODO)***

***Gefällt es dir, na dann spende mir halt ein Bier***  :beers:

[![Donate](https://www.paypalobjects.com/de_DE/DE/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)

