#!/usr/bin/python

#------------------------------------------------------------
# Arduino Yun UDP Echo Server
#------------------------------------------------------------
# Version 1.5
# March 2, 2018
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
# [Check here for setup instructions](https://github.com/timsee/ArduCor/tree/master/samples/yun)
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

# set by the arduino project, defines maximum size packet it will accept
max_packet_size = 512

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
    header = data[:2]
    # print "received %r from %r" % (data, addr)
    if data == "DISCOVERY_PACKET":
        major_api = bridge.get('major_api')
        minor_api = bridge.get('minor_api')
        using_crc = bridge.get('using_crc')
        hardware_count = bridge.get('hardware_count') 
        max_packet_size = bridge.get('max_packet_size')
        hardware_name = bridge.get('hardware_name')
        hardware_type = bridge.get('hardware_type')
        product_type = bridge.get('product_type')
        data += ','
        data += str(major_api)
        data += ','
        data += str(minor_api)
        data += ','
        data += str(using_crc)
        data += ','
        data += str(max_packet_size)
        data += ','
        data += str(hardware_count)
        data += '@'
        data += hardware_name
        data += ','
        data += hardware_type
        data += ','
        data += product_type
        data += '&'
        # sends discovery packet
        sock.sendto(data, (addr[0], UDP_PORT))
    elif header == "8&":
        bridge.put('udp', data)
        state_update = bridge.get('state_update')
        sock.sendto(state_update, (addr[0], UDP_PORT)) 
    elif header == "9&":
        bridge.put('udp', data)
        custom_array_update = bridge.get('custom_array_update')
        sock.sendto(custom_array_update, (addr[0], UDP_PORT)) 
    elif len(data) <= max_packet_size: 
        # puts data on arduino bridge
    	bridge.put('udp', data)
        if should_echo:
            sock.sendto(data, (addr[0], UDP_PORT))

