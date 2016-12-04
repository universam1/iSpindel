# Calibration

>The inclination of the iSpindle floating in the wort can be converted into degrees Plato (ºPlato), density (SG) or other units. To be able to do so a reference should be made between the degrees of angle of the iSpindle making with the wort or young beer and the chosen unit. Reference values will be measured and with the use of an Excel table a graph is made. Excel will calculate a function with the relation between the angle of the iSpindle and  Plato degrees, density or others. As each self built iSpindle will show little differences, the reference measurement is needed once for each individual iSpindle. 

***

# Content
- [Easy method (I)](#easy-method-(I))
- [Improved method using fast fermenting (II)](#improved-method-using-fast-fermenting-(II))
- [Formula Excel tabel](#formula)

***

## Easy method (I)

You are advised to connect the iSpindel with Ubidots for easy reading of the measured angles. Furthermore it is wise to change settings of the iSpindel so it sent every 20 seconds new readings. With such a short interval the measurements can be easil followed on the Ubidots website.

1. Put the iSpindel in clean water (0 degrees Plato, SG 1.000).   
> ***Write down the inclination of the iSpindel***

1. Make a solution of sugar in water with the highest for the future forseeable density. If you are brewing strong beers with a high starting density you should have reference measurements in this high range. For most brewers a solution with a density around 1.085 or around 20 degrees Plato will do.      
> *Example for sugar soluion: 400 ml with 100 g sugar*     
> ***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***

1. Dillute the solution above with water until it reaches 15 degrees Plato or density 1.061       
>*Example for sugar solution: dillute the solution of point 2 with 166 ml of water*     
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***        

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
![](https://github.com/universam1/iSpindel/blob/master/docs/Excelcalc.jpg)
3. In je account van Ubidots een **Derived Variabele** maken. Dat is een variabele die van andere gegevens afhankelijk is. Volg hiervoor de onderstaande stappen:
4. Ga in Ubidots naar Sources
5. Kies de iSpindel als source (bron).
6. (+) Add variable kiezen
7. Derived kiezen.      
![](https://github.com/universam1/iSpindel/blob/master/docs/Ubiderived.jpg)
8. Hier kan de Excel formule ingevoerd worden. De onderdelen ***(tilt)*** die uit Excel komen moet je hier vervangen door op ***insert Variable*** te klikken en tilt te kiezen. Geef de variabele een logische naam (Bijvoorbeeld SG of Plato)     
![](https://github.com/universam1/iSpindel/blob/master/docs/Ubiplato.jpg)
9. Op het Dashboard kan je dan een nieuwe grafiek maken met als data bron je zojuist gemaakte variabele SG of Plato.

