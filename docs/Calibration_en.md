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

1. Make a solution of sugar in water with the highest in the future forseeable density. If you are brewing strong beers with a high starting density you should have reference measurements in this high range. For most brewers a solution with a density around 1.085 or around 20 degrees Plato will do.      
> *Example for sugar soluion: 400 ml with 100 g sugar*     
> ***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***

1. Dillute the solution above with water until it reaches 15 degrees Plato or density 1.061       
>*Example for sugar solution: dillute the solution of point 2 with 166 ml of water*     
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***        

1. Dillute the solution from the point above till it reaches approximately 10 degrees Plato, SG 1.040 of 10 degrees Brix.       
>*Example: Dillute the solution from point 3 with 333 ml of water.*        
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***     

1. Dillute the solution from the point above till it reaches approximately 7,5 degrees Plato, SG 1.030 or 7.5 degrees Brix.
>*Example: Dillute the solution from point 4 with 333 ml of water.*        
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***     

1. Dillute the solution from the point above till it reaches approximately 5 degrees Plato, SG 1.020 or 5 degrees Brix
>*Example: Dillute the solution from point 5 with 333 ml of water*        
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***   

1. Dillute the solution from the point above till it reaches approximately 2.5 degrees Plato, SG 1.010 or 2.5 degrees Brix.     
>*Example: Dillute the solution from point 6 with 2000 ml of water.*      
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel        
>Enter the measured points in the Excel sheet (see below). This Excel sheet will calculate the formula for the reference***

***
 
## Improved method using fast fermenting (II)

This method is more precize since it will take the influence of CO2 formed during fermenting into account.

It is advised to connect the iSpindel with Ubidots in order te easily read the measurement of the inclination of the iSpindel. Set the iSpindel with 20 seconds between readings on a short interval of measurement. The inclination can now be very easily found on the webpage of your Ubidots account.

1. Put the iSpindel in clean (tap) water (0 º Plato or SG 1.000)      
>***Write down the measured inclination of the iSpindel***

1. Make a solution of sugar in water with the highest in the future forseeable density. If you are brewing strong beers with a high starting density you should have reference measurements in this high range. For most brewers a solution with a density around 1.085 or around 20 degrees Plato will do.     
>*Example: 800 ml of water + 200 g crystal sugar.*

1. Add yeast to the sugar solution and let the iSpindel float in the solution         
>***Measure the density or the degrees Plato and write it down with the inclination of the iSpindel***

1. Measure the density of the fluid and the corresponding inclination of the iSpindle at regular intervals till the end of fermentation has been reached.      
>***Enter the measured values in the Excel sheet. This sheet will show the formula of the calibration curve.***     
>
>****Caution: if you measure in Brix value with a refractometer you have to recalculate the measured value to correct for the alcohol reading*** 

***

## Formula

The measured values can be entered in the Excel sheet. This sheet will calculate a formula that will predict the density or the degrees of Plato given an inclination value. This formula has to be entered in Ubidots to be able to get readings in density or degrees Plato.
Other reading, such as apparent attenuation or alcohol % can be obtained in similiar manners with the use of a '''Derived Variable'''.

1. First enter the measured values in Excel:    
[Download Excel tabel](https://github.com/universam1/iSpindel/blob/master/docs/Kalibrierung.xlsm)
2. Start the macro **Formule update** by clicking on the button. The formula will be updated.     
![](https://github.com/universam1/iSpindel/blob/master/docs/Excelcalc.jpg)
3. In the account of Ubidots a **Derived Variable** has to be added. This is a variable that is calculated with other variables as source. This can be done by following the next steps.
4. In Ubidots go to **Source**
5. Choose iSpindel as source
6. Click on **(+) Add variable** 
7. Choose **Derived**      
![](https://github.com/universam1/iSpindel/blob/master/docs/Ubiderived.jpg)
8. Now you can fill in the formula from Excel. De parts in the formula shown as ***(tilt)*** has to be replaced. To do so click on ***insert variable*** and choose **tilt**. Give the newly created variable a name that does make sense, i.e. Density or Plato     
![](https://github.com/universam1/iSpindel/blob/master/docs/Ubiplato.jpg)
9. Under **Dashboard** you can create graphical presentations of your data.
