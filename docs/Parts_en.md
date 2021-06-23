# Parts

Following parts are required:

## Plastic cylinder

- plastic cylinder `Petling`. These containers are available at eBay as PET-Preforms.

***

## Drawer

- plastic drawer ([download](https://github.com/universam1/iSpindel/raw/master/drawer/))

![Drawer](/pics/Schlitten_cad.jpg)

Only needed if you are not using a printed circuit board.

***

## Battery pack

- `18650 LiIo cell` (e.g. `Panasonic NCR18650B`)

***

## Components

- [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- `GY-521` Gyro & Acceleration Sensor (MPU-6050 on suitable breakout board)
- [`DS18B20` Temperature Sensor](https://www.maximintegrated.com/en/products/analog/sensors-and-sensor-interface/DS18B20.html)
- Square grid 3x4 cm or a printed circuit board
  - [iSpindel PCB 4.0](https://www.pcbway.com/project/shareproject/iSpndel_4_0.html)
  - [iSpindel The Jeffrey 2.69](https://www.pcbway.com/project/shareproject/iSpindel_The_Jeffrey_2_69___Open_Source_Distilling.html)
- Resistors
  - 4k7 ohms
  - 220k ohms
  - 330 ohms or BAT43 diode
- [Microswitches](http://www.reichelt.de/Schiebeschalter/SS-ESP201/3/index.html) or at the PCB [Reed Wechsler](http://www.reichelt.de/Reedrelais/KSK-1C90/3/index.html)
- Lipo charger `TP4056` (6 pin recommended)

***

# Additional information

## Shopping lists:
- [rechelt.de](https://www.reichelt.de/my/1848159) (Note: select the correct battery holder depending on the used board)

## GY-521 (Gyro & Acceleration Sensor)
There are several clone sensors available that can be of poor quality. So make sure to buy them from a reliable source, e.g. the one linked in the above liked Wemos documentation.

Other then that, it can be helpful to order not just one sensor and test them before soldering them onto the board.

Here you find 2 options how to proceed with testing gyro sensors:
- [Issue 429 (comment)](https://github.com/universam1/iSpindel/issues/429#issuecomment-766389383) or [Issue 512 (comment)](https://github.com/universam1/iSpindel/issues/512#issuecomment-864923729)
- [Issue 512 (comment)](https://github.com/universam1/iSpindel/issues/512)
