#Charts From Local Server 

While exporting CSVs or directly accessing the database via ODBC from, for example, Excel, is fine for data analysis, we'll definitely also want a quick way to take a glance at the current fermentation.
So, here are a few essential charts, developed using [highcharts](http://www.highcharts.com), browser accessible.

We'll need a working [install](../INSTALL_en.md) of the backend, including mySQL and Apache2.

My goal was to implement a solution as simple yet effective as possible.

I've implemented these 4 basic charts:

* angle.php - tilt and temperature over the past x hours
* plato.php - gravity and temperature over the past x hours (calibration record required as explained below)
* battery.php - current battery voltage
* status.php - battery, tilt and temperature of the specified iSpindle

In order to show these charts we pass arguments via GET in order to be able to bookmark the URLs:

* http://raspi/iSpindle/angle.php?name=MySpindle1&hours=24
* http://raspi/iSpindle/status.php?name=MySpindle2

I hope I've built sort of a foundation with templates for lots of future enhancements.
I am aware that there's probably a ton of things I could have solved more elegantly and there's room for improvement galore.
Contributions are by all means welcome. Looking forward!


###A Few Hints Regarding Installation:
####Apache2:
In order for apache to "see" the charts, they'll have to be somewhere in **/var/www/html**.
(This might vary in distributions other than Raspbian).
I achieve that by simply creating a symlink there, pointing towards my work directory.

      cd /var/www/html    
      sudo ln -s ~/iSpindel/tools/genericTCP/web/ iSpindle

####Database Interface:
You'll need to configure the database connection, found in include/common_db.php, so edit this file section:

      // configure your database connection here:
      define('DB_SERVER',"localhost");
      define('DB_NAME',"iSpindle");
      define('DB_USER',"iSpindle");
      define('DB_PASSWORD',"password");

####Calibration (Angle:Density)
Before you can use plato.php to display the calculated density (%w/w) in Plato degrees, you'll need enter the [calibration results](../../../docs/Calibration_en.md) and add them to the database.      
The reference being used is the spindle's unique hardware id, stored as "ID" in the 'Data' table.    
First, if you haven't done that before, you'll need to create a second table now:
     
     CREATE TABLE `Calibration` (
     `ID` varchar(64) COLLATE ascii_bin NOT NULL,
     `const1` double NOT NULL,
     `const2` double NOT NULL,
     `const3` double NOT NULL,
     PRIMARY KEY (`ID`)
     ) 
     ENGINE=InnoDB DEFAULT CHARSET=ascii 
     COLLATE=ascii_bin COMMENT='iSpindle Calibration Data';

ID is the iSpindel's unique hardware ID as shown in the 'Data' table.
const1, 2, 3 are the three coefficients of the polynom you have got as a result of calibrating your iSpindel:

const1 * winkel<sup>2</sup> - const2 * winkel + const3

You could enter these using phpMyAdmin, or on a mysql prompt, you'd do:

    INSERT INTO Calibration (ID, const1, const2, const3)
    VALUES ('123456', 0.013355798, 0.776391729, 11.34675255);

Have Fun,     
Tozzi (stephan@sschreiber.de)
