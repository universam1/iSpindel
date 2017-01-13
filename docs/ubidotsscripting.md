# Scripting für Ubidots

## Curl
Curl ist ein open source command line tool. Hiermit können HTML-Nachrichten z.B. nach Ubidots gesendet werden.
- https://curl.haxx.se/

## Ubidots API
Viele Funktionen von Ubidots lassen sich über Script mittels HTML-Nachrichten steuern.
So lassen sich z.B. die Daten einer Variable komplett löchen, wenn sie nicht mehr benötigt werden.
- https://ubidots.com/docs/api/#rest-api-reference

## Curl Script zur Datenlöschung
- Download [ZIP-Datei](https://github.com/universam1/iSpindel/raw/master/tools/delete_all_values_ispindel.zip) 
- Enthaltener Ordner an beliebiger Stelle entpacken 
- Die enthaltene "delete_all_values_ispindel.bat" mit dem Editor bearbeiten, dort dann statt

  `<ID eurer Variable bei Ubidots>`
  
  die ID der Variable, welche ihr bei Ubidots leeren wollt eintragen und statt
  
  `<Token eures Accounts bei Ubidots>`
  
  euer Token.
- Die Zeile könnt ihr natürlich so oft kopieren, wie ihr wollt, wenn ihr mehrer Variablen gleichzeitig leeren wollt.
- Die "delete_all_values_ispindel.bat" starten und die Variable(n) sollten leer sein.

## Curl Script zur Anzeige der letzten Messwerte
- Download [ZIP-Datei](https://github.com/universam1/iSpindel/raw/master/tools/get_five_values_ispindel.zip) 
- Enthaltener Ordner an beliebiger Stelle entpacken 
- Die enthaltene "get_five_values_ispindel.bat" mit dem Editor bearbeiten, dort dann statt

  `<ID eurer Variable bei Ubidots>`
  
  die ID der Variable, welche ihr bei Ubidots auslesen wollt eintragen und statt
  
  `<Token eures Accounts bei Ubidots>`
  
  euer Token.
- Die Zeile könnt ihr natürlich so oft kopieren, wie ihr wollt, wenn ihr mehrer Variablen gleichzeitig auslesen wollt.
- Die "get_five_values_ispindel.bat" starten und die letzten 5 Werte der Variable(n) sollten ausgelesen werden.
