#!/usr/bin/env python2.7

# Version: 0.2.1 - experimental (dynamically add fields to mysql data table)
# Generic TCP Server for iSpindel (https://github.com/universam1/iSpindel)
# Receives iSpindel data as JSON via TCP socket and writes it to a CSV file, Database and/or Ubidots
# This is my first Python script ever, so please bear with me for now.
# Stephan Schreiber <stephan@sschreiber.de>, 2017-02-02

from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
from datetime import datetime
import thread
import json

# CONFIG Start

# General
DEBUG = 1                               # Set to 1 to enable debug output on console
PORT = 9501                             # TCP Port to listen to
HOST = '0.0.0.0'                        # Allowed IP range. Leave at 0.0.0.0 to allow connections from anywhere

# CSV
CSV = 0                                 # Set to 1 if you want CSV (text file) output
OUTPATH = '/home/pi/iSpindle'		# CSV output file path; filename will be name_id.csv
DELIMITER = ';'                         # CSV delimiter (normally use ; for Excel)
NEWLINE='\r\n'                          # newline (\r\n for windows clients)
DATETIME = 1                            # Leave this at 1 to include Excel compatible timestamp in CSV

# MySQL
SQL = 0                                 # 1 to enable output to MySQL database
SQL_HOST = '127.0.0.1'                  # Database host name (default: localhost - 127.0.0.1 loopback interface)
SQL_DB = 'iSpindle'                     # Database name
SQL_TABLE = 'Data'                      # Table name
SQL_USER = 'iSpindle'                   # DB user
SQL_PASSWORD = 'xxxxxx'                 # DB user's password (change this)

# Ubidots (using existing account)
UBIDOTS = 1                                     # 1 to enable output to ubidots
UBI_TOKEN = 'xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'    # ubidots token (change this)

# ADVANCED
ENABLE_ADDCOLS = 0                              # Enable dynamic columns (configure pre-defined in lines 128-129)

# CONFIG End

ACK = chr(6)            # ASCII ACK (Acknowledge)
NAK = chr(21)           # ASCII NAK (Not Acknowledged)
BUFF = 1024             # Buffer Size (greatly exaggerated for now)

def dbgprint(s):
    if DEBUG == 1: print(s)

def handler(clientsock,addr):
    inpstr = ''
    success = False
    spindle_name = ''
    spindle_id = ''
    angle = 0.0
    temperature = 0.0
    battery = 0.0
    while 1:
        data = clientsock.recv(BUFF)
        if not data: break  # client closed connection
        dbgprint(repr(addr) + ' received:' + repr(data))
        if "close" == data.rstrip():
            clientsock.send(ACK)
            dbgprint(repr(addr) + ' ACK sent. Closing.')
            break   # close connection
        try:
            inpstr += str(data.rstrip())
            if inpstr[0] != "{" :
                clientsock.send(NAK)
                dbgprint(repr(addr) + ' Not JSON.')
                break # close connection
            dbgprint(repr(addr) + ' Input Str is now:' + inpstr)
            if inpstr.find("}") != -1 :
                jinput = json.loads(inpstr)
                spindle_name = jinput['name']
                spindle_id = jinput['ID']
                angle = jinput['angle']
                temperature = jinput['temperature']
                battery = jinput['battery']
                # looks like everything went well :)
                clientsock.send(ACK)
                success = True
                dbgprint(repr(addr) + ' ' + spindle_name + ' (ID:' + spindle_id + ') : Data received OK.')
                break # close connection
        except Exception as e:
            # something went wrong
            # traceback.print_exc() # this would be too verbose, so let's do this instead:
            dbgprint(repr(addr) + ' Error: ' + str(e))
            clientsock.send(NAK)
            dbgprint(repr(addr) + ' NAK sent.')
            break # close connection server side after non-success
    clientsock.close()
    dbgprint(repr(addr) + " - closed connection") #log on console

    if success :
        # We have the complete spindle data now, so let's make it available
        if CSV == 1:
            dbgprint(repr(addr) + ' - writing CSV')
            try:
                filename = OUTPATH + spindle_name + '_' + spindle_id + '.csv'
                with open(filename, 'a') as csv_file:
                    # this would sort output. But we do not want that...
                    # import csv
                    # csvw = csv.writer(csv_file, delimiter=DELIMITER)
                    # csvw.writerow(jinput.values())
                    outstr = ''
                    outstr += spindle_name + DELIMITER
                    outstr += spindle_id + DELIMITER
                    outstr += str(angle) + DELIMITER
                    outstr += str(temperature) + DELIMITER
                    outstr += str(battery)
                    if DATETIME == 1:
                        cdt = datetime.now()
                        outstr += DELIMITER + cdt.strftime('%x %X')
                    outstr += NEWLINE
                    csv_file.writelines(outstr)
                    dbgprint(repr(addr) + ' - CSV data written.')
            except Exception as e:
                dbgprint(repr(addr) + ' CSV Error: ' + str(e))

        if SQL == 1:
            try:
                import mysql.connector
                dbgprint(repr(addr) + ' - writing to database')
                # standard field definitions:
                fieldlist = ['timestamp', 'name', 'ID', 'angle', 'temperature', 'battery']
                valuelist = [datetime.now(), spindle_name, spindle_id, angle, temperature, battery]
                # establish database connection
                cnx = mysql.connector.connect(user=SQL_USER, password=SQL_PASSWORD, host=SQL_HOST, database=SQL_DB)
                cur = cnx.cursor()
                # add extra columns dynamically?
                # this is kinda ugly; if new columns should persist, make sure you add them to the lists above...
                # for testing purposes it allows to introduce new values of raw data without having to fiddle around.
                if ENABLE_ADDCOLS == 1:
                    jinput = json.loads(inpstr)
                    for key in jinput:
                        if not key in fieldlist:
                            dbgprint(repr(addr) + ' - key \'' + key + ' is not yet listed, adding it now...')
                            fieldlist.append(key)
                            value = jinput[key]
                            valuelist.append(value)
                            # crude way to check if it's numeric or a string (we'll handle strings and doubles only)
                            vartype = 'double'
                            try:
                                dummy = float(value)
                            except:
                                vartype = 'varchar(64)'
                            # check if the field exists, if not, add it
                            try:
                                dbgprint(repr(addr) + ' - key \'' + key + '\': adding to database.')
                                sql = 'ALTER TABLE ' + SQL_TABLE + ' ADD ' + key + ' ' + vartype
                                cur.execute(sql)
                            except Exception as e:
                                if e[0] == 1060:
                                    dbgprint(repr(addr) + ' - key \'' + key + '\': exists. Consider adding it to defaults list if you want to keep it.')
                                else:
                                    dbgprint(repr(addr) + ' - key \'' + key + '\': Error: ' + str(e))
                # gather the data now and send it to the database
                fieldstr = ', '.join(fieldlist)
                valuestr = ', '.join(['%s' for x in valuelist])
                add_sql = 'INSERT INTO Data (' + fieldstr + ')'
                add_sql += ' VALUES (' + valuestr + ')'
                cur.execute(add_sql, valuelist)
                cnx.commit()
                cur.close()
                cnx.close()
                dbgprint(repr(addr) + ' - DB data written.')
            except Exception as e:
                dbgprint(repr(addr) + ' Database Error: ' + str(e))

        if UBIDOTS == 1:
            try:
                dbgprint(repr(addr) + ' - sending to ubidots')
                import urllib2
                outdata = {
                    'tilt' : angle,
                    'temperature' : temperature,
                    'battery' : battery
                }
                out = json.dumps(outdata)
                dbgprint(repr(addr) + ' - sending: ' + out)
                url = 'http://things.ubidots.com/api/v1.6/devices/' + spindle_name + '?token=' + UBI_TOKEN
                req = urllib2.Request(url)
                req.add_header('Content-Type', 'application/json')
                req.add_header('User-Agent', spindle_name)
                response = urllib2.urlopen(req, out)
                dbgprint(repr(addr) + ' - received: ' + str(response))
            except Exception as e:
                dbgprint(repr(addr) + ' Ubidots Error: ' + str(e))


def main():
    ADDR = (HOST, PORT)
    serversock = socket(AF_INET, SOCK_STREAM)
    serversock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    serversock.bind(ADDR)
    serversock.listen(5)
    while 1:
        dbgprint('waiting for connection... listening on port: ' + str(PORT))
        clientsock, addr = serversock.accept()
        dbgprint('...connected from: ' + str(addr))
        thread.start_new_thread(handler, (clientsock, addr))

if __name__ == "__main__":
    main()
