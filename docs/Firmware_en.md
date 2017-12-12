# Firmware

## Flashing via Web Interface

- download the latest .bin firmware from this repository 
### https://github.com/universam1/iSpindel/releases

- in `Maintenance` menu select this file and press update

![flash web](/pics/maintenance.png)

## Flashing via USB (Windows)

- download and run: [NodeMCU-Flasher](https://github.com/nodemcu/nodemcu-flasher/raw/master/Win32/Release/ESP8266Flasher.exe)
- download the latest firmaware Release from the [/bin Folder](https://github.com/universam1/iSpindel/raw/master/bin/)
> Important: select the ```Download``` button to download the file correctly
- Select the COM port for the Wemos
- Use the ```Config``` tab to select the first ```.bin``` file
- Clicking the ```Flashen``` tab shows a progress bar
- After a successful Flash, press the ```RESET``` key

> ```Config Mode``` is active while the LED is blinking at 1s intervals. It can be exited via the Link in the Portal or by waiting for the 5min timeout.

## Flashing via USB (Linux)

- download and unpack the latest version of [esptool](https://github.com/igrr/esptool-ck/releases). Then change directory into the newly unpacked directory, using terminal commands such as:
```
    wget https://github.com/igrr/esptool-ck/releases/download/0.4.12/esptool-0.4.12-linux64.tar.gz
    tar xf esptool-0.4.12-linux64.tar.gz
    cd esptool-0.4.12-linux64
```
    
- download the latest firmware from the [iSpindel Releases page](https://github.com/universam1/iSpindel/releases) e.g.
```
    wget https://github.com/universam1/iSpindel/releases/download/5.6.1/firmware.bin
```

- determine the iSpindel's device path. This may be found by listing all serial USB devices before and after plugging in the iSpindel e.g. before iSpinndel is plugged in:
```
ls -l /dev/ttyUSB*
ls: cannot access '/dev/ttyUSB*': No such file or directory
```

now after iSpindel is plugged in:
```
ls -l /dev/ttyUSB*
crw-rw-rw- 1 root dialout 188, 0 Dec 11 23:20 /dev/ttyUSB0
```
showing that the iSpindel device name is /dev/ttyUSB0


- upload the firmware by running the esptool command with the discovered iSpindel device name (/dev/ttyUSB0 in this case):

```./esptool -vv -cd nodemcu -cb 921600 -cp /dev/ttyUSB0 -ca 0x00000 -cf ./firmware.bin```

