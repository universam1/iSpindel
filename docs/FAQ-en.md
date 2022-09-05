# Questions and answers FAQ

- ***Do I have to tare the iSpindle with weights?***  
It is indispensable to enable the iSpindle functionality! Add weights such as nuts, lead balls etc. to the bottom  until the inclination is 25° from the vertical. See also example pictures in the documentation. Normally about 20g are sufficient. +/- 5° deviations are often still acceptable.
If you're too close to 30deg, it will be too close to 90 deg at high sg measurements (your starting sg won't be accurate).
If you're too close to 15deg, it will be too close to 0 deg at low sg measurements (you won't be able to read sg's below .95).

- ***It doesn't work, I don't see anything***  
The best way to find out what the iSpindle is doing  is to read its log. This can be done via a serial monitor like HTerm or Arduino IDE. Connect the spindle via Micro USB, then open the £COM Port with the serial monitor above and read it. Set baud rate to `115200`. It brings light into the dark.

Here is a youtube how-to (in German): https://youtu.be/6xDW5GbIbUw

- ***How get to the iSpindel - the portal***  
Switch it on, press the reset button 3-4 times which sets up an access point. That means you will see a new Wifi network "iSpindel_######" where you connect to with your computer, smartphone or tablet. Now the browser should automatically redirect you to the page [http://192.168.4.1](http://192.168.4.1). If not, navigate there yourself.
If you are now on the portal page, under `Configuration` you can enter your token and the wifi password that you need for your network. Then click on `save` and it will send the data to Ubidots. This means that the **iSpindel is not reachable in normal operation**, until you manually start the portal again.

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
  - Use a passive antenna. Use a shielded cable that is stripped from both ends (i. e. Lambda/4), where one side ends in the pot, exactly 32mm. This is transmitted passively from the inside to the outside and vice versa. A good example can be found here: http://hobbybrauer.de/forum/viewtopic.php?p=208782#p208782
  - Install a wifi range extender next to the barrel to increase the signal level.

-  ***What should I do if my iSpindel isn't working properly after flashing it with the firmware?***

For unclear reasons sometimes it looks like flashing of the iSpindel went well while it actually didn’t which could lead to unpredictable behavior.
This behavior could be but is not limited to:
-	the blue LED of the wemos stays on permanently
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

-  ***Improvements that have issues***
These are "frequently suggested improvements" that have issues that aren't obvious at first; feel free to try them, but
keep watch for problems:

1. "Let's seal it and use inductive charging!"
  a)The Qi chargers are larger than the PETling so they have to be bent.  Some Qi chargers get very unhappy about 
    that (overheating/fire), while others work but are *very* picky about placement on the Qi charging pad.
    There are inductive chargers that fit inside the cap [not Qi] (eg: https://amzn.to/3wslaLO , but they haven't 
    been tried yet).
    *Try it, but watch for heat!*

  b)The seal for the PETling is at the top/inside; yeast and bacteria  will get in the "threaded area" and will be 
    impossible to wash out.  Right now, it's recommended to take the cap off and clean/sanitize the cap and the 
    PETling separately.

2. Doesn't krausen/CO2 affect the readings?
  It certainly does with the commercial version; if you think it's settled prematurely, rock/stir your wort and 
  see what happens.
  
3. Can the iSpindel be used in a pressurized fermenter?
  You can try... the default PETling seal is "on the inside" so that it makes a better seal as the pressure inside 
  increases (sodas are carbonated), which is the wrong direction, but the screw theads do make a seal, so...

- ***Why do I get a connection error with HTTPS POST?***  
Unlike HTTP, the HTTPS POST method requires using the full URL to the server for the Server Address setting. e.g. `https://example.com` instead of just the hostname `example.com`.

## AWS IoT Conectivity

ADVANCED USERS ONLY

Recently we have started to add support for AWS IOT Core using the Secure MQTT option.

This option currently requires you to manually input your keys and certs into the secrets.h file, compile and upload yourself.
We are looking to add the ability to upload a certificate in future.

0. Configure AWS IoT core including creating your thing, certificates, policies and download your certs and keys
1. Download the Repo
2. Edit secrets.h to supply AWS Root Cert, Device Private Cert, Device Private Key
3. Compile and upload to your iSpindel
4. Configure using the config menu as normal with the AWSIOTMQTT option

Note: Secure MQTT (required by AWS) using a lot more battery as the device has to be on longer during each send. It needs to sync to an NTP server and establish and encrypted connection - both of which require processing time and hence battery use on the ESP8266.
