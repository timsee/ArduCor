#!/usr/bin/python

#------------------------------------------------------------
# UDPtoSerial.py
#------------------------------------------------------------
# Version 1.0
# January 16, 2018
# MIT License (in root of git repo)
# by Tim Seemann
#
#
# Takes UDP datagram packets at its UDP_PORT as input and
# echoes them over serial. It also takes serial packets and echoes
# them over UDP.
#
# This script was originally designed to use a Raspberry
# Pi as a wireless adapter for an arduino running lighting routines,
# although it is generic enough that you can probably modify its
# behavior to work with arduino projects other than RGB-LED-Routines.
# You will likely need to change the delimiters for the serial packets
# from ";" to whatever is used by your arduino project if you are
# modifying this script for other purposes.
#
#------------------------------------------------------------

#-----
# imports

import socket
import time
import serial
import sys

#-----
# Check for valid script usage
if (len(sys.argv) != 2):
    print "Incorrect format, please add the serial connection as an argument to your python call Example: python UDPtoSerial.py /dev/ttyUSB0"
    sys.exit()

#-----
#  Serial Setup
# configure the serial connections (the parameters differs on the device you are connecting to)
print "Setup Serial..."
# setup serial
ser = serial.Serial(sys.argv[1], 9600, timeout=0.0) # a timeout of 0 makes serial.read nonblocking


#-----

print "Setup the UDP Socket..."
# port for the UDP connection to bind to
UDP_PORT = 10008
# set up UDP server.
sock = socket.socket(socket.AF_INET,    # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind(("", UDP_PORT))
sock.settimeout(0.02)

#-----
# loop

#repeats ad nauseam
while True:
    # waits to receive data, timeout
    try:
        udp_data, addr = sock.recvfrom(512)
        #print "UDP: %r from %r" % (udp_data, addr)
        if udp_data:
            # add delimiter to messages
            udp_data += ";"
            # write over serial
            ser.write(udp_data)

        # check for serial data
        message = ''
        charReturned = 'junk'
        while (charReturned != ''):
            # read one char at a time
            charReturned = ''
            charReturned = ser.read(1)
            if (charReturned != ''):
                message += charReturned
        if message != '' and addr:
            # strip off the delimiting character
            message = message.rstrip()[:-1]
            #print "ARDUINO:  %r " % (message)
            # send over UDP
            sock.sendto(message, (addr[0], UDP_PORT))
    except socket.timeout as err:
        pass
