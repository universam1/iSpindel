# Bauteile

Folgende Bauteile werden benötigt:

## Kunstoff Zylinder

- Kunststoff Zylinder `Petling`. Diese `PET-Preform` sind z.B. in eBay erhältlich.

***

## Schlitten

- Kunststoff Schlitten ([download](https://github.com/universam1/iSpindel/raw/master/drawer/))

![Schlitten](/pics/Schlitten_cad.jpg)

Dieser wird dann benötigt, wenn keine Leiterplatte (PCB) verwendet wird.

***

## Akku

- `18650 LiIo Zelle` (z.B. `Panasonic NCR18650B`)

***

## Elektronische Bauteile

- [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- `GY-521` Gyro & Beschleunigungssensor (MPU-6050 auf passendem Breakout-Board)
- [`DS18B20` Temperatursensor](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html)
- Lochrasterplatine 3x4 cm oder Platine aus Sammelbestellung
  - [iSpindel PCB 4.0](https://www.pcbway.com/project/shareproject/iSpndel_4_0.html)
  - [iSpindel The Jeffrey 2.69](https://www.pcbway.com/project/shareproject/iSpindel_The_Jeffrey_2_69___Open_Source_Distilling.html)
- Widerstände
  - 4k7 Ohm
  - 220 kOhm
  - 330 Ohm oder BAT 43 Diode
- [Mikro Schalter](http://www.reichelt.de/Schiebeschalter/SS-ESP201/3/index.html) oder bei Platine [Reed Wechsler](http://www.reichelt.de/Reedrelais/KSK-1C90/3/index.html)   
- Lipo Lademodul `TP4056` (6 pin recommended)

***

# Zusätzliche Informationen

## Shopping Liste:
- [rechelt.de](https://www.reichelt.de/my/1848159) (Hinweis: Abängig von der Platine einen der beiden Batteriehalter wählen)

## GY-521 (Gyro & Beschleunigungssensor)
Es gibt einige Klone des Sensors auf dem Markt. Diese können von schlechter Qualität sein und funktionieren ggf. nicht korrekt. Daher den Sensor am besten bei einem vertrauenswürdigen Händler kaufen – z.B. über den Link in der oben verlinkten Wemos Dokumentation.

Außerdem kann es hilfreich sein, gleich mehrere Sensoren zu bestellen und diese vor dem Verlöten auf dem Board zu testen.

Hier findest du zwei Möglichkeiten den Gyrosensor zu testen:
- [Issue 429 (comment)](https://github.com/universam1/iSpindel/issues/429#issuecomment-766389383) oder [Issue 512 (comment)](https://github.com/universam1/iSpindel/issues/512#issuecomment-864923729)
- [Issue 512 (comment)](https://github.com/universam1/iSpindel/issues/512)
