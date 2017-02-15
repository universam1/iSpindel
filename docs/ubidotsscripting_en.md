# Scripting for Ubidots

## Curl
Curl is an open source command line tool. For example, HTML messages can be To Ubidots.- https://curl.haxx.se/

## Ubidots API
Many functions of Ubidots can be controlled via Script via HTML-messages.
For example, Completely delete the data of a variable when it is no longer required.
- https://ubidots.com/docs/api/#rest-api-reference

## Curl Script for data erasure
- Download [ZIP file](https://github.com/universam1/iSpindel/raw/master/tools/delete_all_values_ispindel.zip) 
- Unpack the folder at any point
- Edit the contained "delete_all_values_ispindel.bat" with the editor, then take place instead

  `<ID of your variable for Ubidots>`
  
  The ID of the variable, which you want to empty with Ubidots and instead
  
  `<Token of your account at Ubidots>`
  
  your token.
- You can, of course, copy the line as many times as you want, if you want to empty several variables at the same time.
- Start "delete_all_values_ispindel.bat" and the variable (s) should be empty.

## Curl Script to display the last measured values
- Download [ZIP file](https://github.com/universam1/iSpindel/raw/master/tools/get_five_values_ispindel.zip) 
- Unpack the folder at any point
- Edit the contained "get_five_values_ispindel.bat" with the editor, then take place instead

  `<ID of your variable for Ubidots>`
  
  The ID of the variable, which you want to read out at Ubidots and instead
  
  `<Token of your account at Ubidots>`
  
  Your token.
- You can, of course, copy the line as many times as you want, if you want to read multiple variables at the same time.
- Start the "get_five_values_ispindel.bat" and read the last 5 values ​​of the variable (s).
