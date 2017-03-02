#genericTCP
####(iSpindle.py Version 0.2.1)

[Install Instructions](INSTALL_en.md)      
[Charts](web/README_en.md)

This script was written in Python and its purpose is to accept raw data from an iSpindle via a generic TCP connection, usually in a local network environment.
It purposely avoids unneccessary higher-level protocols such as http, in order to maximize the battery lifetime of the iSpindle and generally make things more easy and transparent, also for you fellow developers out there.

The data received can be stored (or forwarded) in three different ways, non exclusively.    
You can enable or disable them separately, one by one.   
One option is to save incoming data to a CSV (comma separated values) file, one per iSpindle.
This might be useful for example to do a quick import in Excel.   
The second one allows you to write it to a MySQL table.   
And finally, in order to get the best out of two worlds and not have to say goodbye to Ubidots, you can configure the script to foward it all transparently, so Ubidots won't even know it's not connected directly to your iSpindle, with the added advantage of being able to also process your data locally, so, for example, you could come up with some new super nice way to calibrate it.   

In addition, the time your iSpindle has to wait for a connection will decrease, further enhancing its battery life.   
But even without Internet access, you'll be able to process the data your iSpindle sends.

The script is completely platform independent and should run on any OS that supports Python.
It has been tested on Mac OS X (Sierra) and Linux (Debian), but it should work under Windows just as well.
If you own or have rented a dedicated or virtual server, or if there is any computer in your home network that is running 24/7, this should work for you.    
A Raspberry Pi will always do the trick.
Just make sure you have Python installed, and if you are using the MySQL feature, don't forget to install the `python-mysql.connector`, too.
Multithreading is implemented, so even if your multiple iSpindles send their data at the same time, there should be no delays or other problems during the transmission.

When configuring your iSpindle, choose **TCP** as protocol, enter the IP address of the server the script is running on, and enter **9501** as TCP port (which is the default port the script will listen to).

Then, configure the script by opening it in a text editor.
Make sure you adjust all the settings according to the descriptions following below.

Finally, copy it to some path on your server. If using a Raspi, good choices would be `/usr/local/bin` or simply `/home/pi`.

Make it executable by typing `chmod 755 iSpindle.py` on the command line inside the path you've chosen.
Then start it by typing `./iSpindle.py`.
Alternatively (or when using Windows), you can start it by typing `python iSpindle.py`.    
Once it all works, set DEBUG to 0, restart it in the background and enjoy.


###Configuration:

####General:

	DEBUG = 0      
	PORT = 9501    
	HOST = '0.0.0.0'

**DEBUG** = 1 allows detailed output on the console.
You'll want this when first configuring the script and your iSpindle.
After that, not so much, probably.   
If TCP **Port** is already in use (unlikely), you can change it here to another value.   
**HOST** determines the IP range clients have to be in in order to be allowed to connect. Leave this at 0.0.0.0 for no restrictions.
Port 9501 is usually not reachable from the (potentially hostile) outside unless you are explicitly forwarding it through your router (firewall settings: port forwarding), so, no worries there, usually.
And if you've read this far, you'll probably know what you're doing, anyway... ;)

####CSV:

	CSV = 0
	OUTPATH = '/home/pi/iSpindel'
	DELIMITER = ';'
	NEWLINE = '\r\n'
	DATETIME = 1    

Set **CSV** to 1 if you want CSV files to be generated.    
**OUTPATH** configures the path CSV files will be stored at. Share it in your local network to allow easy import for Excel or whatever frontend you want to use.    
**DELIMITER** sets the character to be used to separate the data fields. ';' is usually good for Excel. Common choices would be ',' or ';'.    
**NEWLINE** is normally '\n', but if you're using anything made by Microsoft, use '\n\r'.    
**DATETIME** should be left at its default setting of 1, unless for some reason you don't want timestamps being added to the data output.


####MySQL

	SQL = 1
	SQL_HOST = '127.0.0.1'
	SQL_DB = 'iSpindle'
	SQL_TABLE = 'Data'
	SQL_USER = 'iSpindle'
	SQL_PASSWORD = 'xxxxxxxx'


If you want to switch off MySQL connectivity, set **SQL** to 0 and all other settings will be ignored.     
**SQL\_HOST** defines the DB host's IP address. Usually, this will be 'localhost' or 127.0.0.1.    
The remaining fields define the connection to the database.    
By default, we assume the database name and user ID are 'iSpindle', and the table name is 'Data'.

In order to create a table inside your MySQL database accordingly, use this SQL statement:

	CREATE TABLE 'Data' (
		'Timestamp' datetime NOT NULL,
		'Name' varchar(64) COLLATE ascii_bin NOT NULL,
		'ID' varchar(64) COLLATE ascii_bin NOT NULL,
		'Angle' double NOT NULL,
		'Temperature' double NOT NULL,
		'Battery' double NOT NULL,
		PRIMARY KEY ('Timestamp', 'Name', 'ID')
	) 
	ENGINE=InnoDB DEFAULT CHARSET=ascii 
	COLLATE=ascii_bin COMMENT='iSpindle Data';

Of course you could always just log in to the database using your default admin account, but a better solution is to create a dedicated user for the server script:

	CREATE USER 'iSpindle'@'localhost' IDENTIFIED BY 'password';
	GRANT ALL PRIVILEGES ON iSpindle . * TO 'iSpindle'@'localhost';
	FLUSH PRIVILEGES;

The script is able to create additional table columns dynamically from the received JSON dataset.    
This is, however, only recommended if you are developing your own firmware and wish to store some variables not being exported by default.
If your server is reachable from the Internet, make sure **ENABLE\_ADDCOLS** is 0.
In its current version I cannot guarantee the script is not vulnerable to SQL Injection attacks when this is enabled (set to 1).
If unsure, set it to 0.


####Ubidots Forwarding

	UBIDOTS = 1
	UBI_TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'

**UBIDOTS** = 0 will switch off Ubidots Forwarding.    
**UBI\_TOKEN** is where you enter your personal Ubidots Token (see iSpindle docs).

Your data should now show up in Ubidots as usual, plus you have it available locally to fiddle around with.    
Ubidots will not know the difference and even create new devices just as well.

Have Fun!    
Tozzi (stephan@sschreiber.de)
