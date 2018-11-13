# Kalibrierung

>Zur Umrechnung von Winkel ° in °Plato, SG, %mas o.Ä. ist es initial nötig eine Referenzkurve zu kalibrieren. Die gewonnenen Referenzen können dann vereinfacht über eine Excel Tabelle in eine Funktion überführt werden, die zur Darstellung hinterlegt wird. Da jede selbstgebaute Spindel andere Messwerte ergeben wird, muss man diesen Prozess einmalig durchführen.

>Stelle sicher dass die iSpindel im normalen Wasser ca. 25° Neigung von der Vertikalen aufweist. Gegebenenfalls noch weiter austarieren. Meistens sind ca. 20g an zusätzlichem Gewicht nötig.

***

# Inhalt

- [Kalibrierung](#kalibrierung)
- [Inhalt](#inhalt)
  - [Variante A (vereinfacht)](#variante-a-vereinfacht)
  - [Variante B Schnellvergärung (genauer)](#variante-b-schnellverg%C3%A4rung-genauer)
  - [Variante C Referenzen ziehen (empfohlen)](#variante-c-referenzen-ziehen-empfohlen)
- [Formel](#formel)
- [Speichern](#speichern)
  - [in iSpindel hinterlegen](#in-ispindel-hinterlegen)
  - [Alternativ: in Ubidots nach-berechnen](#alternativ-in-ubidots-nach-berechnen)

***

## Variante A (vereinfacht)

> Zum Ablessen des Neigungswinkels empfiehlt es sich, die iSpindel mit Ubidots zu verbinden und einen sehr kurzen Intervall (20 Sekunden) zu setzen. Damit kann die Neigung über die Webseite verfolgt werden.  
>  
>  Alternativ: Im Konfigurationsmodus kann die ```Info``` Seite aufgerufen werden und dort den Winkel abgelesen werden.

1. iSpindel in Reinwasser 0 °Plato legen, ***Winkel notieren (sollte ca. 25° Neigung aus der Vertikalen sein)***
2. Zuckerwasser mit ca. 20 °Plato herstellen, oder höchsten Wert den man voraussichtlich erreichen wird. ***Plato messen und Winkel notieren***  
*Beispiel 400ml Wasser + 100g Kristallzucker auflösen*
1. Wasser weiter verdünnen auf ca. 15 °Plato, ***Plato messen und Winkel notieren***   
*Beispiel +166ml Wasser hinzufügen*
1. Wasser weiter verdünnen auf ca. 10 °Plato, ***Plato messen und Winkel notieren***   
*Beispiel +333ml Wasser hinzufügen*
1. Wasser weiter verdünnen auf ca. 7,5 °Plato, ***Plato messen und Winkel notieren***   
*Beispiel +333ml Wasser hinzufügen*
1. Wasser weiter verdünnen auf ca. 5 °Plato, ***Plato messen und Winkel notieren***   
*Beispiel +666ml Wasser hinzufügen*
1. Wasser weiter verdünnen auf ca. 2,5 °Plato, ***Plato messen und Winkel notieren***   
*Beispiel +2000ml Wasser hinzufügen*

**gemessene Werte werden in das [Excel Sheet](#formel) eingetragen** 

***

## Variante B Schnellvergärung (genauer)

**Diese Variante ist genauer, da der CO2 Auftriebsfehler mit ein - kalibriert wird**

> Zum Ablessen des Neigungswinkels empfiehlt es sich, die iSpindel mit Ubidots zu verbinden und einen sehr kurzen Intervall (20 Sekunden) zu setzen. Damit kann die Neigung über die Webseite verfolgt werden.  

1. iSpindel in Reinwasser 0 °Plato legen, ***Winkel notieren (sollte ca. 25° Neigung aus der Vertikalen sein)***
1. Zuckerwasser mit ca. 20 °Plato herstellen, oder höchsten Wert den man voraussichtlich erreichen wird.  
*Beispiel 800ml Wasser + 200g Kristallzucker auflösen*
1. Hefe hinzufügen und Spindel einlegen. ***Plato messen und Winkel notieren*** 
1. bis x. :  Nun nimmt man regelmässig Plato Messwerte und den jeweilig korrelierenden Winkel und trägt dies in die Excel Tabelle ein. 
> Achtung: Sollte man hier einen Refraktometer verwenden darf man die Korrektur dessen nicht vergessen: [Refraktometerrechner](http://www.maischemalzundmehr.de/index.php?inhaltmitte=toolsrefraktorechner)

## Variante C Referenzen ziehen (empfohlen)

> Die einfachste und vermeintlich genaueste Variante ist sie in eine Gärung zu bringen und einige Referenzen zu ziehen. Man notiert sich die Stammwürze und den entsprechenden Winkel den man zB. bei Ubidots zu diesem Zeitpunkt sieht. Im Verlauf der Gärung nimmt man weiter 3 oder mehr solcher Referenzen ( gespindelte Stammwürze / Winkel ) notiert diese bis zum Gärende. Damit erhält man praxisnahe Werte die übrigens den CO2 Einfluss beinhalten bzw. berücksichtigen.
Danach kann man mit dem errechnen der Formel fortfahren, und diesen fest in der iSpindel dann hinterlegen.


***

# Formel
Folgende Varianten existieren um das Polynome leicht zu errechnen:

- ## [Online Gravity Calibration tool](http://www.ispindel.de/tools/calibration/calibration.htm)

Die Messwerte werden in das Feld `Plato` oder `Brix` eingetragen mit dem korrespondierendem Neigungswinkel unter `Tilt`. Daraus errechnet sich dann ein Polynom 2. oder 3. Grades welches dann in der Firmware hinterlegt wird.

***

- ## Excel Tabelle

> Obige Meßwerte können nun anhand dieser Excel Tabelle in eine annähernde Formel überführt werden.  
Diese hinterlegt man in Ubidots um die Anzeige des °Plato zu erhalten.  
> Andere Darstellung wie EVG, %mas, SG, Alc. werden nach gleichem Prinzip über eine ```Derived``` Variable aus vorhandenen Werten errechnet

1. Die obigen Meßwerte in die Excel Tabelle eintragen  
  [Download Excel Tabelle](Kalibrierung.xlsm)
1. ```Formel update``` Macro durch den Button starten, stellt die Trendlinie als Formal dar  
![Formel update](/pics/Excelcalc.jpg)


# Speichern

## in iSpindel hinterlegen

> Das Polynome kann nun in der iSpindel gespeichert werden, welcher in Laufzeit evaluiert und berechnet wird. Damit liefert die iSpindel nun die Dichte direkt als Datensatz.

![Plato](/pics/configuration.png)

## Alternativ: in Ubidots nach-berechnen

1. Ubidots eine ```Derived``` Variable erstellen, d.h. eine berechnete die auf anderen Daten aufbaut.  
1. Bei Ubidots auf Sources gehen
1. Source ISpindel wählen
1. (+) Add variable wählen
1. Derived wählen ![](/pics/Ubiderived.jpg)
1. Dort kann dann die Excel Formel eingetragen werden.  
```tilt``` ersetzt man durch ```instert Variable``` und wählt dort ```tilt``` aus 
![Plato](/pics/Ubiplato.jpg)
1. Im ```Dashboard``` erstellt man dann einen neuen Graphen mit den Datenquelle ```Plato```
