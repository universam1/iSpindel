# Scripting für Ubidots

## Curl
Curl ist ein open source command line tool. Hiermit können HTML-Nachrichten z.B. nach Ubidots gesendet werden.
- https://curl.haxx.se/

## Ubidots API
Viele Funktionen von Ubidots lassen sich über Script mittels HTML-Nachrichten steuern.
So lassen sich z.B. die Daten einer Variable komplett löchen, wenn sie nicht mehr benötigt werden.
- https://ubidots.com/docs/api/#rest-api-reference

## Curl Script zur Datenlöschung
- Download ![ZIP-Datei](Ubidots/delete_all_values_ispindel.zip) 
- Enthaltener Ordner an beliebiger Stelle entpacken 
- Die enthaltene "delete_all_values_ispindel.bat" mit dem Editor bearbeiten, dort dann statt
  <ID eurer Variable bei Ubidots>
  die ID der Variable, welche ihr bei Ubidots leeren wollt eintragen und statt
  <Token eures Accounts bei Ubidots>
  euer Token.
- Die Zeile könnt ihr natürlich so oft kopieren, wie ihr wollt, wenn ihr mehrer Variablen gleichzeitig leeren wollt.
- Die "delete_all_values_ispindel.bat" starten und die Variable(n) sollten leer sein.
