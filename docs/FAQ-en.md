# Questions and answers FAQ

- ***Do I have to tare the iSpindle with weights?***  
It is indispensable to enable the iSpindle functionality! Add weights such as nuts, lead balls etc. to the bottom  until the inclination is 25° from the vertical. See also example pictures in the documentation. Normally about 20g are sufficient. +/- 5° deviations are often still acceptable.

- ***It doesn't work, I don't see anything***  
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

-  ***What should I do if my iSpindel isn't working properly after flashing it with the firmware?***

For unclear reasons sometimes it looks like flashing of the iSpindel went well while it actually didn’t which could lead to unpredictable behavior.
This behavior could be but is not limited to:
-	the blue led of the wemos stays on permanently
-	parts of configuration is not saved, e.g. offset tilt

The solution to this can be initializing the memory of the wemos before flashing it with the iSpindel firmware.(as if you were formatting a new harddrive before you can use it)

Here is how to do that using NODEMUCU FIRMWARE PROGRAMMER

To initialize:

In the Config Tab
````
		1st column		2nd column
1st Line:	INTERNAL://NODEMCU	0x00000
2nd Line:	INTERNAL://BLANK	0x01000
````
In the Advanced Tab
Set the Baudrate to 115200

In the Operation Tab
Click Flash(E) and wait till the flashing is done

To flash with the iSpindel firmware:

In the Config Tab
````
		1st column		2nd column
1st Line: 	C:\iSpindel-6.0.6.bin 	0x00000
````
Where “C:\iSpindel-6.0.6.bin” is to be replaced by the path to your firmware file
Make sure all the other lines in the Config tab are cleared and only the checkbox in front of your firmware path is checked.

In the Advanced Tab
Set the Baudrate to 115200

In the Operation Tab
Click Flash(E) and wait till the flashing is done

It now should work properly.

Source: https://www.homebrewtalk.com/forum/threads/ispindle-diy-electronic-hydrometer.598187/page-19#post-8366882
