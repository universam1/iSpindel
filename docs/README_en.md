# iSpindle Documentation

***Translation is work in progress, feel free to contribute!
This translation might be out of sync with the original german one, please double check***


iSpindle (iSpindel) Documentation
===================

**DIY electronic Hydrometer**
***Please consider supporting this project***  

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)


![iSpindle in clean water](/pics/floating.jpg)
![Dashboard](/pics/Dashboard.jpg)


## Table of Contents


- [FAQ](FAQ-en.md)
- [Parts](Parts_en.md)
- [Calibration](Calibration_en.md)
- [Circuit Diagram](circuit_diagram_en.md)
- [Upload FHEM](upload-FHEM_en.md)
- [Ubidots scripting](ubidotsscripting_en.md)


- [License](#license)
- [Principle](#principle)
  - [Metacentric Height](#metacentric-height)
- [Construction](#construction)
  - [Components](#components)
  - [Circuit Diagram](#circuit-diagram)
  - [Sled](#sled)
- [Configuration](#configuration)
  - [Ubidots](#ubidots)
  - [Portal](#portal)
- [Graphical User Interface](#graphical-user-interface)
  - [Calibrating the Spindle](#calibration)
  - [Ubidots Graphen](#ubidots-graphen)
  - [CraftBeerPi](#craftbeerpi)
- [Software](#software)


***

## License


> All rights reserverd, any commercial use is hereby prohibited and will violate applicable patents.

***

## Principle

Powered by the thread [Alternative to the Spindle](http://hobbybrauer.de/forum/viewtopic.php?f=7&t=11157&view=unread#p170499), the idea was born to reproduce the commercially available electronic tildting spindle using low-cost components.

The system is based around the use of a heeling (or tilting) cylinder, an ingenious and easy concept - you do not need any external reference (except for gravity) and the cylinder is extremely easy to keep clean. The inclination angle changes in relation to the buoyancy and thus directly in relation to the sugar content. There is an angle formed between the center of mass and the center of bouyancy depending on the density of the fluid.


![Kränung](/pics/kraengung.jpg)

Therefore the idea  is to place a Wifi-enable IoT device with an accelerometer and temperature sensor in a floating cylinder. The system will measure the sensors and every x-minutes it will connect to the Wifi and sends its tilt angle, temperature and battery voltage to a cloud service like Ubidots.com to store the data.

With an update interval of 30min it was possible to achive a battery lifetime of almost 3 months!

### *Metacentric Height*

Actually, this is the "metacentre", the cylinder will tilt as the liquid density changes in relation to its center of mass and center of bouyancy. The angle of tilt can then be measured. 

It is possible to trim the cylinder by adding a few grams on the bottom so that the cylinder is more upright, or on the lid, so that it is more tilted.

The software calculates the Euler angle for X and Y from the XYZ acceleration values and forms the absolute angle. We compute these with the calibrated parameters to ° Plato, SG or similar by correlating to measured references.


***

## Construction

### see [Parts](Parts_en.md)

### see [Sourcing](Parts_en.md)

### see [Circuit Diagram](circuit_diagram_en.md)

### see [iSpindel Breabboard Mounting](iSpindelbreadboard_en.md)

***

### Drawer
- A 3D Printed drawer is used to secure the electronics and battery inside the plastic housing as shown below. The 3D model can be found in the repository.
![Sled](/pics/Schlitten_cad.jpg)
![Assembled](/pics/assembled2.jpg)
![Assembled](/pics/assembled.jpg)

<a href="http://www.youtube.com/watch?feature=player_embedded&v=gpVarh8BxhQ" target="_blank"><img src="http://img.youtube.com/vi/gpVarh8BxhQ/0.jpg" 
alt="Druck" width="240" height="180" border="10" /></a>



***

## Configuration

### Ubidots

- To start, you must create a free account at [Ubidots.com](https://ubidots.com)
- Next, you must go to the menu  ```API Credentials``` to get a ```Token``` to be used by the iSpindle to authorize writing data to the Ubidots account.
***Write this down.***  
![Token](/pics/UbiToken.jpg)  

### Portal

By pressing the  ```Reset Button``` the Wemos creates an access point, which allows you to make the necessary settings to configure the device. **In `operation mode` this portal is not active or accessible because the principle of this design is based on shorted possible acitve time. Basically it will wake up, send its data and deep sleep again. This takes now less than 3s which is directly related to its long life run time.

> The ```iSpindel``` signalises the `config mode` by blinking the LED in a 1s interval.  
By saving your settings or waiting timeout of 5min it will end the Portal thus AccessPoint and try to go into `operation mode`.


   ![Setup](/pics/configuration.png)

   ![AccessPoint](/pics/AP.png)![Portal](/pics/Portal.png)

  ![Info](/pics/info.png)

> In Ubidots you can monitor the update of data unders ```Sources``` where the iSpindel will create a new device itself.  
In the ```Dashboard``` now you can create your nice graphs.

***
## Graphical User Interface

### Calibration

> In order to convert the measured iSpindle angle to degrees Plato (°Plato), density (SG), or other units, it is necessary to first calibrate the sensor by making several reference measurements of sugar water of known gravities. These reference measurements can then be converted to a mathematical function which is stored for later measurements and display. Since each self-assembled iSpindle will  have different measured values, each device must be individually calibrated after assembly or reassembly to yield accurate measurements. A detailed procedure is below.

[Calibration Procedure](Calibration_en.md)

### Ubidots Graphen

- [Plato Formula](Calibration_en.md#forumla)

### CraftBeerPi

[CraftBeerPi](https://github.com/universam1/iSpindel/issues/3)
***

## Software 

### Firmware flashing

[Firmware flashing](Firmware_en.md)

***if you like it, let me know***  :beers:

[![Donate](https://www.paypalobjects.com/de_DE/DE/i/btn/btn_donate_LG.gif)](https://www.paypal.me/universam)
