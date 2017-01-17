#!/usr/bin/env python2.7

# Version: 0.0.1
# Simple Q&D Server for iSpindel (https://github.com/universam1/iSpindel)
# Receives iSpindel data as JSON via TCP socket and writes it to a CSV file
# This is my first Python script ever, so please bear with me for now.
# Stephan Schreiber <stephan@sschreiber.de>, 2017-01-14

from socket import *
import thread
import json
import datetime
# import traceback
# import csv

# CONFIG Start
DEBUG = 1                               # Set to 1 to enable debug output on console
PORT = 9501                             # TCP Port to listen to
OUTPATH = '/media/usb0/iSpindel/csv/'   # CSV output file path
DELIMITER = ';'                         # CSV delimiter (use ; for Excel)
NEWLINE='\r\n'                          # newline (\r\n for windows clients)
DATETIME = 1                            # Set to 1 to enable appending of Excel compatible timestamp to CSV file
# CONFIG End

# HOST = '127.0.0.1'    # Use this to connect only from localhost (testing purposes)
HOST = '0.0.0.0'        # Accept all incoming connections
ACK = chr(6)            # ASCII ACK (Acknowledge)
NAK = chr(21)           # ASCII NAK (Not Acknowledged)
BUFF = 1024             # Buffer Size (greatly exaggerated for now)

def dbgprint(s):
    if DEBUG == 1: print(s)

def handler(clientsock,addr):
    inpstr = ''
    while 1:
        data = clientsock.recv(BUFF)
        if not data: break  # client closed connection
        dbgprint(repr(addr) + ' received:' + repr(data))
        if "close" == data.rstrip():
            clientsock.send(ACK)
            dbgprint(repr(addr) + ' ACK sent. Closing.')
            break
        try:
            inpstr += str(data.rstrip())
            if inpstr[0] != "{":
                clientsock.send(NAK)
                dbgprint(repr(addr) + ' Not JSON.')
                break
            dbgprint(repr(addr) + ' Input Str is now:' + inpstr)
            if inpstr.find("}") != -1 :
                jinput = json.loads(inpstr)
                filename = OUTPATH + jinput['name'] + '_' + jinput['ID'] + '.csv'
                with open(filename,'a') as csv_file:
                    # this would sort output. But we do not want that...
                    # csvw = csv.writer(csv_file, delimiter=DELIMITER)
                    # csvw.writerow(jinput.values())
                    outstr = ''
                    outstr += jinput['name'] + DELIMITER
                    outstr += jinput['ID'] + DELIMITER
                    outstr += str(jinput['angle']) + DELIMITER
                    outstr += str(jinput['temperature']) + DELIMITER
                    outstr += str(jinput['battery'])
                    if DATETIME == 1:
                        cdt = datetime.datetime.now()
                        outstr += DELIMITER + cdt.strftime('%x %X')
                    outstr += NEWLINE
                    csv_file.writelines(outstr)
                clientsock.send(ACK)
                dbgprint(repr(addr) + ' ' + jinput['name'] + ' (ID:'+ jinput['ID'] + ') : Data received OK.')
                break # close connection server side after success
        except Exception as e:
            # something went wrong
            # traceback.print_exc() # this would be too verbose, so let's do this instead:
            dbgprint(repr(addr) + ' Error: ' + str(e))
            clientsock.send(NAK)
            dbgprint(repr(addr) + ' NAK sent. Waiting for Retry.')
            break # close connection server side after non-success
    clientsock.close()
    dbgprint(repr(addr) + " - closed connection") #log on console

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
