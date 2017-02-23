# Kalibreren

>Om de hellingshoek van de iSpindle van graden (º) in ºplato, SG of andere waarden om te rekenen is het nodig om eerst een referentiegrafiek te maken. De gevonden referentiewaarden worden daarbij eenvoudig door middel van een Excel tabel in een functie omgezet. Omdat iedere zelfgebouwde meter een andere waarde zal geven moet deze referentiemeting eenmalig voor iedere meter uitgevoerd worden. De referentiemeting kan in graden Plato of als SG met een hydrometer uitgevoerd worden.

***

# Inhoud
- [Vereenvoudigde methode (I)](#vereenvoudigde-methode-(I))
- [Precieze methode via snelvergisting (II)](#precieze-methode-via-snelvergisting-(II))
- [Formule Excel tabel](#formule)

***

## Vereenvoudigde methode (I)


Voor het aflezen van de hellingshoek wordt aanbevolen om de iSpindel via Ubidots te verbinden en de iSpindel in te stellen op een korte meetinterval (20 seconden). Daardoor kan de helling van de sensor eenvoudig via een webpagina gevolgd worden.

1. iSpindel in schoon water leggen (0ºPlato, SG 1.000).   
> ***Noteer de hellingshoek***

1. Suikerwater maken met de voor de toekomst verwachtte hoogste meetwaarde. Als je bijvoorbeeld erg sterke bieren brouwt (hoge begin SG) dan zal je ook in dit hogere bereik een referentiewaarde moeten hebben. Voor de meeste brouwers zal suikerwater met ca. 20ºPlato of SG 1.083 voldoen.      
> *Voorbeeld suikeroplossing: Oplossing van 400 ml water en 100 g kristalsuiker.*     
> ***Meet je gemaakte oplossing (in SG of ºPlato) en noteer deze waarde met de bijbehorende hellingshoek van de iSpindel***

1. De oplossing van punt 2 met water verdunnen tot circa 15ºPlato of SG 1.061.      
>*Voorbeeld: oplossing van punt 2  verdunnen met 166 ml water.*     
>***Meet de gemaakte oplossing (SG of ºPlato) en noteer deze waarde met de bijbehorende hellingshoek van de iSpindel.***        

1. De oplossing van punt 3 met water verdunnen tot circa 10 graden Plato, SG 1,040 of 10 graden Brix.       
>*Voorbeeld: Oplossing van punt 3 met 333 ml water.*        
>***Meet de gemaakte oplossing (SG of Plato) en noteer met de bijbehorende hoek van de iSpindel.***     

1. De oplossing van punt 4 met water verdunnen tot circa 7,5 graden Plato, SG 1,030 of 7,5 graden Brix.
>*Voorbeeld: Oplossing van punt 4 met 333 ml water.*        
>***Meet de gemaakte oplossing (SG of Plato) en noteer met de bijbehorende hoek van de iSpindel.***     

1. De oplossing van punt 5 met water verdunnen tot circa 5 graden Plato, SG 1,020 of 5 graden Brix
>*Voorbeeld: Oplossing van punt 5 met 666 ml water.*        
>***Meet de gemaakte oplossing (SG of Plato) en noteer met de bijbehorende hoek van de iSpindel.***     

1. De oplossing van punt 6 met water verdunnen tot circa 2,5 graden Plato, SG 1,010 of 2,5 graden Brix.     
>*Voorbeeld: Oplossing van punt 6  met 2000 ml water.*      
>***Meet de gemaakte oplossing (SG of Plato) en noteer met de bijbehorende hoek van de iSpindel.        
>De gemeten waarden invoeren in het Excel blad. Hieruit volgt direct de formule van de referentiegrafiek.***

***
 
## Precieze methode via snelvergisting (Variant B)

Deze variant is preciezer, omdat er rekening wordt gehouden met de invloed van CO2 gedurende de vergisting.

Voor het aflezen van de hellingshoek wordt aanbevolen om de iSpindel  via Ubidots te verbinden en de iSpindel in te stellen op een korte meetinval (20 seconden). Daardoor kan de helling van de sensor eenvoudig via een webpagina gevolgd worden.
1. Ispindel in schoon water leggen (0 º Plato of SG 1,000)      
>***Noteer de hoek***

1. Suikerwater maken met het hoogste voor de toekomst verwachtte bereik. Voor de meeste brouwers zal 20 ºplato of  SG 1,083 voldoende zijn.     
>*Voorbeeld: 800 ml water + 200 g kristalsuiker.*

1. Gist toevoegen en de iSpinel in de vloeistof leggen.         
>***Meet je gemaakte oplossing (SG of Plato) en noteer met de bijbehorende hoek van de iSpindel.***

1. Tot het onderste bereik van de meetrange is bereikt regelmatig de vloeistof meten en de bijbehorende hoek noteren.       
>***De gemeten waarden invoeren in het Excel blad. Hieruit volgt direct de formule van de referentiegrafiek.***     
>
>****Opletten: bij een meting in Brix moet wel de correctie meegenomen worden.**** 

***

## Formule

De meetwaarden kunnen aan de hand van de Excel tabel in een formule gezet worden die het verloop bij benadering kan berekenen. Deze formule wordt in Ubidots ingevoerd om de weergaven in graden Plato of SG te verkrijgen.
Andere weergaven, zoals bijvoorbeeld de SVG of alcohol % zijn op vergelijkbare manier via een **Derived variabele** te berekenen.

1. De hierboven verkregen meetwaarden invoeren in de Excel tabel.       
[Download Excel tabel](https://github.com/universam1/iSpindel/blob/master/docs/Kalibrierung.xlsm)
2. Macro **Formule update** starten door op de button te klikken. De formule wordt dan bijgewerkt.      
![Excelcalc](/pics/Excelcalc.jpg)
3. In je account van Ubidots een **Derived Variabele** maken. Dat is een variabele die van andere gegevens afhankelijk is. Volg hiervoor de onderstaande stappen:
4. Ga in Ubidots naar Sources
5. Kies de iSpindel als source (bron).
6. (+) Add variable kiezen
7. Derived kiezen.      
![Ubiderived](/pics/Ubiderived.jpg)
8. Hier kan de Excel formule ingevoerd worden. De onderdelen ***(tilt)*** die uit Excel komen moet je hier vervangen door op ***insert Variable*** te klikken en tilt te kiezen. Geef de variabele een logische naam (Bijvoorbeeld SG of Plato)     
![Ubiplato](/pics/Ubiplato.jpg)
9. Op het Dashboard kan je dan een nieuwe grafiek maken met als data bron je zojuist gemaakte variabele SG of Plato.

