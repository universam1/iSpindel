iSpindle.py - Version 0.0.1

This very simple Python Script (Python 2.7) is listening to a TCP port and writing the JSON data it receives from the iSpindle to an Excel compatble csv file.
It can easily be adapted or used as a core for own generic TCP implementations.
For my own purposes, it's OK as-is, but things like database connectivity should be easy to implement as well.
You're cordially invited to improve it, but we expect that you'll share your code with us here.

Config is easy, just edit the script to suit your needs:

DEBUG = 1                               # Set to 1 to enable debug output on console
PORT = 9501                             # TCP Port to listen to
OUTPATH = '/media/usb0/iSpindel/csv/'   # CSV output file path
DELIMITER = ';'                         # CSV delimiter (use ; for Excel)
NEWLINE='\r\n'                          # newline (\r\n for windows clients)
DATETIME = 1                            # Set to 1 to enable appending of Excel compatible timestamp to CSV file

Other than the path to the output csv file, you would normally not have to change anything there.
After testing, you might want to stop the spam to console, in order to do this, set DEBUG=0.

The output filename is SpindleName_SpindleID.csv by default. You can easily change that by editing the script, too.
Each spindle connected will write to its own separate file.
For the output path, it makes sense to choose something that's being shared over the network with your desktop, in order to simplify import to Excel or whatever tool you choose to visualize your results.

Start the script by making it executable (chown 755 iSpindle.py under Unix/Mac OS) or just do "python iSpindle.py".
It should work under Linux, Mac OS or Windows, without any modifications, if Python is present.

When configuring the iSpindle, use "TCP" and enter the IP address of the server, and the port number (9501 by the script' default).
All other parameters (update interval etc.) as usual.
Generic TCP should offer an advantage on battery usage over the other protocols.

Be aware that this code is still experimental (it was tested and works OK, here, though).
Use at your own peril.

Have fun and let us know how it works for you!

Cheers,
Stephan (stephan@sschreiber.de)
