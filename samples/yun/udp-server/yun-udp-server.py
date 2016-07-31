#!/usr/bin/python

#------------------------------------------------------------
# Arduino Yun UDP Echo Server
#------------------------------------------------------------
# Version 1.1
# July 31, 2016
# MIT License (in root of git repo)
# by Tim Seemann
#
#
# Takes UDP datagram packets at its UDP_PORT as input and
# echoes them to the Arduino Yun's ATmega32U4 processor. 
# This script takes a lot of elements from the [Yun's 
# Bridge client](https://github.com/arduino/YunBridge/blob/master/bridge/bridgeclient.py)
# but does not use it directly. Instead, we interact with
# the TSPJSONClient directly and manage our own sockets. 
# This adds a significant speed increase over the Bridge 
# Client's implementation. 
#
# [Check here for setup instructions](https://github.com/timsee/RGB-LED-Routines/tree/master/samples/yun)
#
#------------------------------------------------------------

#-----
# imports

import socket
from time import sleep

import sys
# Adds a yun specific-library to the sys path
sys.path.insert(0, '/usr/lib/python2.7/bridge')
# imports the yun specific library
from bridgeclient import BridgeClient

#-----
# config

# port for the UDP connection to bind to
UDP_PORT = 10008
last_packet = ""

# Echoing back commands slows down the speed that lights update but it gives
# more reliability since you know when a packet is received. Set this to 
# false to increase the light update speed. 
should_echo = True

#-----
# bridge setup

# set up the serial port for communication with
# the yun's microprocessor
print "Setup the Arduino bridge..."
bridge = BridgeClient()
# Very important! Without this comannd, communication will work
# but will be significantly slower, since it will open and close
# sockets in each function.
bridge.begin() 

#-----

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
    # print "received %r from %r" % (data, addr)
    if data == "DISCOVERY_PACKET":
        hardware_count = bridge.get('hardware_count')  
        data += ','
        data += str(hardware_count)
        state_update = bridge.get('state_update') 
        data += ','
        data += state_update
        # sends discovery packet
        sock.sendto(data, (addr[0], UDP_PORT))
    elif data == "7":
        bridge.put('udp', data)
        state_update = bridge.get('state_update')
        sock.sendto(state_update, (addr[0], UDP_PORT)) 
    else: 
        # puts data on arduino bridge
    	bridge.put('udp', data)
        if should_echo and (data != last_packet):
            sock.sendto(data, (addr[0], UDP_PORT))
            last_packet = data

