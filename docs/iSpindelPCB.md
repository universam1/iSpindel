# Aufbau Platine mit / ohne Schlitten

## Variante B: nur Platine

### Bilder
01-Schalterbruecke ![01-Schalterbruecke.JPG](/pics/PCB/01-Schalterbruecke.JPG)
02-durchtrennt ![02-durchtrennt.JPG](/pics/PCB/02-durchtrennt.JPG)
03-Schaltervarianten ![03-Schaltervarianten.JPG](/pics/PCB/03-Schaltervarianten.JPG)
04-SMDReed-Oeffner ![04-SMDReed-Oeffner.JPG](/pics/PCB/04-SMDReed-Oeffner.JPG)
05-Schiebeschalter ![05-Schiebeschalter.JPG](/pics/PCB/05-Schiebeschalter.JPG)
06-Jumper ![06-Jumper.JPG](/pics/PCB/06-Jumper.JPG)
07-Jumper ![07-Jumper.JPG](/pics/PCB/07-Jumper.JPG)
08-Jumper ![08-Jumper.JPG](/pics/PCB/08-Jumper.JPG)
09-Jumper ![09-Jumper.JPG](/pics/PCB/09-Jumper.JPG)
10-Widerstaende ![10-Widerstaende.JPG](/pics/PCB/10-Widerstaende.JPG)
11-SMD-Widerstaende ![11-SMD-Widerstaende.JPG](/pics/PCB/11-SMD-Widerstaende.JPG)
12-LED-beim-GY521-entfernt ![12-LED-beim-GY521-entfernt.JPG](/pics/PCB/12-LED-beim-GY521-entfernt.JPG)
13-GYVerschraubung ![13-GYVerschraubung.JPG](/pics/PCB/13-GYVerschraubung.JPG)
14-GYPinleiste ![14-GYPinleiste.JPG](/pics/PCB/14-GYPinleiste.JPG)
15-WemosVorbereitung ![15-WemosVorbereitung.JPG](/pics/PCB/15-WemosVorbereitung.JPG)
16-Wemos-USB-Litze ![16-Wemos-USB-Litze.JPG](/pics/PCB/16-Wemos-USB-Litze.JPG)
17-Jumper ![17-Jumper.JPG](/pics/PCB/17-Jumper.JPG)
18-Loetbild-nurnoetigePinsloeten ![18-Loetbild-nurnoetigePinsloeten.JPG](/pics/PCB/18-Loetbild-nurnoetigePinsloeten.JPG)
19-Akkuposition ![19-Akkuposition.JPG](/pics/PCB/19-Akkuposition.JPG)
20-Akkuposition ![20-Akkuposition.JPG](/pics/PCB/20-Akkuposition.JPG)
21-Laderegler ![21-Laderegler.JPG](/pics/PCB/21-Laderegler.JPG)
22-Laderegler ![22-Laderegler.JPG](/pics/PCB/22-Laderegler.JPG)
23-Zentrierschraube ![23-Zentrierschraube.JPG](/pics/PCB/23-Zentrierschraube.JPG)
24-Zentrierschraube ![24-Zentrierschraube.JPG](/pics/PCB/24-Zentrierschraube.JPG)
25-Pinkuerzung ![25-Pinkuerzung.JPG](/pics/PCB/25-Pinkuerzung.JPG)
26-Pinkuerzung ![26-Pinkuerzung.JPG](/pics/PCB/26-Pinkuerzung.JPG)
27-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_A_Bildgroesseaendern ![27-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_A_Bildgroesseaendern.jpg](/pics/PCB/27-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_A_Bildgroesseaendern.jpg)
28-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_B_Bildgroesseaendern ![28-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_B_Bildgroesseaendern.jpg](/pics/PCB/28-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_B_Bildgroesseaendern.jpg)
29-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_C_Bildgroesseaendern ![29-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_C_Bildgroesseaendern.jpg](/pics/PCB/29-Kunststoff-BearbeitungamMinuspoldesAkkuhalters_C_Bildgroesseaendern.jpg)
30-Akkuhalterposition ![30-Akkuhalterposition.JPG](/pics/PCB/30-Akkuhalterposition.JPG)
31-Halterverschraubung ![31-Halterverschraubung.JPG](/pics/PCB/31-Halterverschraubung.JPG)
32-Pluspol ![32-Pluspol.JPG](/pics/PCB/32-Pluspol.JPG)
33-VarianteSMD-Reed ![33-VarianteSMD-Reed.JPG](/pics/PCB/33-VarianteSMD-Reed.JPG)
42-MagnetA25mm ![42-MagnetA25mm.JPG](/pics/PCB/42-MagnetA25mm.JPG)
43-MasgnetAmittigPlatine ![43-MasgnetAmittigPlatine.JPG](/pics/PCB/43-MasgnetAmittigPlatine.JPG)
44-MagnetB16x11x35 ![44-MagnetB16x11x35.JPG](/pics/PCB/44-MagnetB16x11x35.JPG)
45-MagnetBmittigReed ![45-MagnetBmittigReed.JPG](/pics/PCB/45-MagnetBmittigReed.JPG)
46-MagnetC30x7 ![46-MagnetC30x7.JPG](/pics/PCB/46-MagnetC30x7.JPG)


## Variante C: Platine & Schlitten

Um die Platine mit dem Schlitten zu verwenden, folgende Dinge berücksichtigen:
- Die Platine hat eine vorgesehene Schnittkante, in Höhe des Lochs der Schraube, an dieser abtrennen.
- Der DS18B20 wird folglich neben dem GY-521 angelötet
- Der Akku wird an BAT+ und BAT- des TP4056 angelötet

Die Platine in den Falz einschieben, so dass der Wemos sich auf der Akku abgewandten Seite befindet (im schwimmenden Zustand oben).

### Variante C - Step by Step   

#### 1. Platine an der Trennline durchtrennen und die "Ohren" entfernen    
![I M G 9255](/pics/PCB+Tray/IMG_9255.JPG)

#### 2. GY-521 LED auslöten und 4 Pins einer Stiftleiste am GY-521 anlöten (VCC, GND, SCL, SDA).    
Die Stifte zeigen dabei nach unten (zur Seite ohne Bauelemente).    
![I M G 9256](/pics/PCB+Tray/LED.png)
![I M G 9256](/pics/PCB+Tray/IMG_9256.JPG)

#### 3. Abstandhalter der Stiftleiste entfernen   
![I M G 9259](/pics/PCB+Tray/IMG_9259.JPG)
![I M G 9260](/pics/PCB+Tray/IMG_9260.JPG)


#### 4. Heißklebepistole vorbereiten und den GY-521 so nah wie möglich auf die Platine kleben. Je dichter der GY auf der Platine sitzt, desto besser. Sollte die Klebeschicht zu dick sein, dann müssen später ggf. Stege aus dem Schlitten entfernt werden (s.unten).    
![I M G 9262](/pics/PCB+Tray/IMG_9262.JPG)
![I M G 9263](/pics/PCB+Tray/IMG_9263.JPG)
![I M G 9264](/pics/PCB+Tray/IMG_9264.JPG)

#### 5. Pins des GY-521 verlöten und einkürzen    
![I M G 9265](/pics/PCB+Tray/IMG_9265.JPG)

#### 6. Wemos Stiftleisten in die Platine löten und Wemos Sockelleisten auf den Wemos löten    
![I M G 9266](/pics/PCB+Tray/IMG_9266.JPG)

#### 7. Wemos Eingangsdiode auslöten   
![I M G 9266](/pics/PCB+Tray/Diode.png)

#### 8. Stifte für das Lademodul "kupferseitig" auf die Platine löten   
Das Einlöten geht recht leicht, wenn man die Stifte am Kunststoff festhält, "falsch herum" einlötet und den Kunststoffabstandhalter nach dem Einlöten zur Platine drückt.
Wichtig: Die Stifte sollen auf der anderen Seite der Platine nicht herausstehen!
![I M G 9272](/pics/PCB+Tray/IMG_9272.JPG)

#### 9. Pins des Schalters ggf. passend zum Rastermaß der Platine leicht zurechtbiegen   
![I M G 9273](/pics/PCB+Tray/IMG_9273.JPG)

#### 10. Den, mit dem Pfeil gekennzeichneten, Steg (zwischen den Schalterkontakten der Platine) durchtrennen.
![I M G 9279](/pics/PCB+Tray/IMG_9279.JPG)

#### 11. Schalter einsetzen und einlöten     
![I M G 9282](/pics/PCB+Tray/IMG_9282.JPG) 
![I M G 9281](/pics/PCB+Tray/IMG_9281.JPG)

#### 12. Widerstände einsetzen und einlöten
![I M G 9283](/pics/PCB+Tray/IMG_9283.JPG)
![I M G 9284](/pics/PCB+Tray/IMG_9284.JPG)

#### 13. Temperatursensor (DS18B20) "kupferseitig" einsetzen und einlöten    
ACHTUNG! Polarität beachten!  Die flache Seite des DS18B20 zeigt beim kupferseitigen Auflöten zur Außenkante der Platine!
![I M G 9288](/pics/PCB+Tray/IMG_9288.JPG)

#### 14. Akku Lademodul aufsetzen, verlöten und die Stifte einkürzen   
Lademodul ggf. mit etwas Heißkleber "stabilisieren".
![I M G 9290](/pics/PCB+Tray/IMG_9290.JPG)
![I M G 9291](/pics/PCB+Tray/IMG_9291.JPG)

#### 15. fertige Ansicht der Platine    
![I M G 9292](/pics/PCB+Tray/IMG_9292.JPG)
![I M G 9293](/pics/PCB+Tray/IMG_9293.JPG)

#### 16. Platine in den Schlitten schieben   
Falls die Klebeschicht zwischen Platine und GY-521 zu dick war, dann müssen ggf. ein paar Stege aus dem Schlitten entfernt werden   
![I M G 9294](/pics/PCB+Tray/IMG_9294.JPG)
![I M G 9296](/pics/PCB+Tray/IMG_9296.JPG)

#### 17. Kabel an den Akku löten und Akku auf dem Schlitten fixiern  
SEHR VORSICHTIG! Akku beim Löten nicht überhitzen! Brand- und Explosionsgefahr!
![I M G 9297](/pics/PCB+Tray/IMG_9297.JPG)
![I M G 9298](/pics/PCB+Tray/IMG_9298.JPG)

#### 18. Kabel des Akkus am Lademodul anlöten und lose Kabel mit Heißkleber fixieren
![I M G 9299](/pics/PCB+Tray/IMG_9299.JPG)

#### 19. Spindel trimmen    

Vorschlag für etwa 25° Neigung in Leitungswasser
![I M G 9300](/pics/PCB+Tray/IMG_9300.JPG)     

Vorschlag für etwa 15° Neigung in Leitungswasser    
![I M G 9301](/pics/PCB+Tray/IMG_9301.JPG)

#### 20. Fertiger Schlitten mit Platine
![I M G 9303](/pics/PCB+Tray/IMG_9303.JPG)

#### 21. fertige iSpindel
![I M G 9304](/pics/PCB+Tray/IMG_9304.JPG)




