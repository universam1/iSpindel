*AWS IoT Conectivity*
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
