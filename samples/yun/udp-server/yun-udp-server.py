#!/usr/bin/python

#------------------------------------------------------------
# Arduino Yun UDP Echo Server
#------------------------------------------------------------
# Version 1.0
# May 09, 2016
# MIT License (in root of git repo)
# by Tim Seemann
#
#
# Takes UDP datagram packets at its UDP_PORT as input and
# echoes them to the Arduino Yun's ATmega32U4 processor over
# the bridge library.
#
#
# [Check here for setup instructions](https://github.com/timsee/RGB-LED-Routines/tree/master/samples/yun )
#
#------------------------------------------------------------

#-----
# imports

import socket
import array

import sys
# Adds a yun specific-library to the sys path
sys.path.insert(0, '/usr/lib/python2.7/bridge')
# imports the yun specific library
from bridgeclient import BridgeClient

#-----
# config

# port for the UDP connection to bind to
UDP_PORT = 10008

#-----
# bridge setup

# set up the serial port for communication with
# the yun's microprocessor
print "Setup the Arduino Bridge..."
value = BridgeClient()

#-----
# udp setup

print "Setup the UDP Socket..."
# set up UDP server. Python can do a simple server
# in just a few lines..
sock = socket.socket(socket.AF_INET,    # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind(("", UDP_PORT))

#-----
# loop

#repeats ad nauseam
while True:
    # waits until it receives data
    data, addr = sock.recvfrom(512)
    #print "received message:", data
    # puts data on arduino bridge
    value.put('udp', data)
