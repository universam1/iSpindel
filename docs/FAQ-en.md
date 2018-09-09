# Questions and answers FAQ

- ***It doesn't work, I don't see anything****  
The best way to find out what the iSpindle is doing as information is to read it's log. This can be done via a serial monitor like HTerm or Arduino IDE. Connect the spindle via Micro USB, then open the Com Port with the serial monitor above and read it. Set baud rate to `115200`. It brings light into the dark.

Here is a youtube howto: https://youtu.be/6xDW5GbIbUw

- ***How get to the iSpindel - the portal***  
Switch it on, press the reset button 3-4 times which sets up an access point. That means you will see a new Wifi network "iSpindel" where you connect to with your computer, smartphone or tablet. Now the browser should automatically redirect you to the page[http://192.168.4.1](http://192.168.4.1). If not, navigate there yourself.
If you are now on the portal page, under `Configuration` you can enter your token and the wifi password that you need for your network. Then click on `safe` and it will send the data to Ubidots. This means that the **iSpindel is not reachable in normal operation**, until you manually start the portal again.

- ***Why can't I access the iSpindle in normal operation***  
The critical part of the battery life time is that the iSpindle is active as short as possible and maximum time being in`deep sleep`! The Wemos in active operation requires more than 300x of energy, so this trick is indispensable to achieve the runtime.

- ***The Micro USB port is for program updates?***
Correct, see Firmware Update

- ***How can I calibrate the voltage display?***
From firmware 3.0 on you can adjust the conversion factor. By default, factor 191.8 is used. You can adjust by this formula:
````
(displayed voltage / measured voltage) * current factor (191.8) = new factor

The new factor should be between current factor more or less 20% (191.8*0.8 - 191.8*1.2).
````
- ***Can I operate it in a stainless steel container? Is it possible to get through the faradic cage?***
To achieve a sufficient WLAN signal strength you can do the following things:

  - Break open the faradic cage. This is possible by electrically separating the lid from the pot. Can be achieved by applying a silicone sealant between the two parts.
  - Use a passive antenna. Use a shielded cable that is stripped from both ends (i. e. Lambda/4), where one side ends in the pot, exactly 3.2mm. This is transmitted passively from the inside to the outside and vice versa. A good example can be found here: http://hobbybrauer.de/forum/viewtopic.php?p=208782#p208782
  - Install a wifi range extender next to the barrel to increase the signal level.
