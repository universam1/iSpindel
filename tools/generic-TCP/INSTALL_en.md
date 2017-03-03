#Installation Guide for Raspbian 
###Step-by-Step

###Preliminary Remarks:

These recommended software requirements might seem like overkill, but this solution is highly flexible and wide open for future enhancements.    
Most Raspberry owners will use Raspbian, but you should be able to get this to work just fine under any other Linux based distribution.    
Of course you can use slimmer solutions for the database and the web server.    
I am using MySQL and Apache because they're quite standard and there's a lot of potential for future development.    
The whole install will use roughly 5 GB on the sd card, so a 8 GB one should suffice, 16 GB being just perfect.
If you don't need phpmyadmin and the visualization charts you can safely omit both mySql and Apache, of course.

###Prepare Raspbian 
- raspi-config: activate ssh, establish network connection (via Ethernet or WiFi). You'll probably need a keyboard and an HDMI capable display for this step.      
- connect via putty (Windows) or Terminal and SSH (Mac OS X, Linux):
```           
	   ssh pi@[ip-address or hostname] 
	   Password: raspberry (change this)
```
Update your system:

	sudo apt-get update
	sudo apt-get dist-upgrade

###MySQL, Apache2 and phpMyAdmin database GUI 

####Install:

	sudo apt-get install apache2 mysql-server mysql-client php5-mysql python-mysql.connector

When asked, choose a password for the database admin account.

	sudo apt-get install phpmyadmin

When asked, choose Apache2 as web server, and enter the database root user's password (as above).
MySQL is now managable through http://[myraspi]/phpmyadmin.

You can now complete the following steps either by using phpmyadmin (logging in as root, then click on the "SQL" tab) or directly in mysql via command line:

	mysql -u root -p

You'll have to enter the root user's password again.    
Now you should see a **mysql>** prompt.

####Create and Select the Database:
	CREATE DATABASE iSpindle;
	USE iSpindle;

####Create Tables:

If you wish to use the charts with density calibration, you should create both tables now.    
[Here](./MySQL_CreateTables.sql) you can find scripts with both definitions.
Otherwise, the main data table will suffice:

	CREATE TABLE `Data` (
 		`Timestamp` datetime NOT NULL,
 		`Name` varchar(64) COLLATE ascii_bin NOT NULL,
 		`ID` varchar(64) COLLATE ascii_bin NOT NULL,
 		`Angle` double NOT NULL,
 		`Temperature` double NOT NULL,
 		`Battery` double NOT NULL,
 	PRIMARY KEY (`Timestamp`,`Name`,`ID`)
	) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin COMMENT='iSpindle Data'

The field "ID" stores the iSpindle's unique hardware ID, which we'll need in order to use calibration.

####Create a Database User, Grant Permissions, Set Password):

	CREATE USER 'iSpindle' IDENTIFIED BY 'password'
	GRANT USAGE ON *.* TO 'iSpindle';
	GRANT ALL PRIVILEGES ON `iSpindle`.* TO 'iSpindle' WITH GRANT OPTION;

Now the database is accessible by the Python server script.    
Configure it as explained here: [README](./README_en.md).
See section below on how to install it.


###Optional: Install Samba (Recommended):

	sudo apt-get install samba samba-common-bin

####Share pi User's Home Directory and Log Files:

/etc/samba/smb.conf:

	[global]
 	server string = RASPBIAN
 	guest ok = yes
 	security = user
 	socket options = TCP_NODELAY SO_RCVBUF=65535 SO_SNDBUF=65535
 	registry shares = yes
 	syslog = 0
 	map to guest = bad user
 	workgroup = WORKGROUP
 	bind interfaces only = No
 	encrypt passwords = true
 	log level = 0
	# smb ports = 445
 	unix extensions = No
 	wide links = yes

 	include = /etc/samba/user.conf
 	include = /etc/samba/shares.conf


/etc/samba/shares.conf:

	[pi-home]
    	path = /home/pi
    	guest ok = yes
    	read only = no
    	force user = pi
    	browseable = yes

	[system-logs]
    	path = /var/log
    	guest ok = yes
    	read only = yes
    	force user = root
    	browseable = yes

####Start Samba Daemon:

	sudo insserv smbd
	sudo service smbd start

The pi user's home and system log directories are now being shared and you should be able to see them in Explorer/Finder.

###Install the Python Server Script for genericTCP:
Configure the script as explained here: [README](./README_en.md).
If you're not too familiar with Unix and the shell, you could follow this guide below:

Copy both iSpindle.py and ispindle-serv to the pi home directory.
Then, within the SSH terminal session, type:

    cd /home/pi
    sudo mv ./iSpindle.py /usr/local/bin
	sudo mv ./ispindle-srv /etc/init.d
	sudo chmod 755 /usr/local/bin/iSpindle.py
	sudo chmod 755 /etc/init.d/ispindle-srv
	cd /etc/init.d
	sudo systemctl deamon-reload
	sudo insserv ispindle-srv
	sudo service ispindle-srv start

Now would be a good time to reboot the Raspi (should not be required, though).    
You should be able to see the script running now:

    ps -ax | grep iSpindle

Done.
If everything is configured correctly, the database should receive the iSpindle data and your device(s) should show up in Ubidots, if you have enabled forwarding.    
[Here](web/README_en.md) are some charts I made for visualization I found essential.    

Have fun!

Yours,
Tozzi (stephan@sschreiber.de)



