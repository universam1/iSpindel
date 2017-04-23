# Datenupload nach Loxone


iSpindel is able to transmit its data to a [Loxone](http://loxone.com) Miniserver. That way iSpindel data can be visualized within the Loxone apps and web interface. Functionality may be extended by further computation of the data – e.g. for regulating the fermentation temperature or notification of the completed fermentation.

[Loxone visualization](/pics/loxone-app-screenshot.png)


## The way it works

iSpindel submits its data as text using Http Get with authentification to a Virtual Textinput in the Loxone Miniserver. A PicoC program splits the text into the different data values and outputs to TQ1 (iSpindel name), AQ1 (tilt), AQ2 (calculated plato value), AQ3 (temperature) and AQ4 (battery voltage).


## Setup in Loxone

Find an example project for Loxone Config in /tools/loxone. Copy the components with copy and paste to your own project. Unfortunately the Virtual Textinput will be broken and had to be added on your own. Simply add a new Virtual Textinput and connect it to the iSpindel program on input TI1. Last, but not least you have to change the plato calculation in the PicoC script according to your calibration results (the code line is marked in the PicoC script). Default update time for the script is 5 seconds – change the line "sleep(5000);" to different values for testing purposes or to minimize server load.

[Loxone setup](/pics/loxone-Miniserver-configuration.png)


## Setup iSpindel

In the configuration set the service type to "Loxone" and enter the server address, port number and Loxone login credentials. The server URL has to be the address of the Virtual Textinput; "/dev/sps/io/<name of the Virtual Textinput>/". For example: "/dev/sps/io/VTI1/".

[iSpindel setup](/pics/loxone-iSpindel-configuration.jpg)


## Safety disclaimer

Login credentials are being saved and transmitted by the iSpindel in plain text without any encryption. Everybody in the same network could easily read this information by analysing network traffic. For safety reasons only use it if you're accessing the Loxone Miniserver locally or via VPN.
