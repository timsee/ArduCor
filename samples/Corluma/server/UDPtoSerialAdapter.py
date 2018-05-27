#!/usr/bin/python

#------------------------------------------------------------
# UDPtoSerial.py
#------------------------------------------------------------
# Version 2.8
# May 27, 2018
# MIT License (in root of git repo)
# by Tim Seemann
#
#
# Takes UDP datagram packets at its UDP_PORT as input, parses and sorts it
# based off of its contents, and, if its valid, it echoes them over
# over serial. It also takes serial packets and echoes
# them over UDP.
#
#------------------------------------------------------------


#-----
# imports
import socket
import time
import serial
import sys
from ctypes import c_uint32
from numpy import bitwise_xor, right_shift

# Change this port if it conflicts with another program on your system
UDP_PORT = 10008
# Header values for packets for state updates and custom color updates
stateUpdatePacketHeader = 6
customColorUpdatePacketHeader = 7

#--------------------------------
# Message Parsing Functions
#--------------------------------

#-----
# takes a list of messages, appends them to a single string with message delimiters,
# computes the CRC for this string, appends that, and then returns the full message packet
def convertMessageListToPacket(messageList, serialIndex):
    packet = ""
    if checkIfDiscoveryPacket(messageList[0]):
        packet += messageList[0]
    else:
        for message in messageList:
            # leave room for a CRC and metadata, so only append if we're not exceeding
            # max packet size
            if (len(packet) + len(message) < maxPacketSizeList[serialIndex] - 16):
                packet += message
                packet += "&"
        packet = appendCRC(packet)
    packet += ";"
    return packet

#-----
# a really ugly function that parses a packet as individual messages, and then
# sorts them into a message dictionary where the key is the serial device's index.
def sortMessages(packet):
    # break up the packet into multiple messages
    messageArray = packet.split("&")
    # only if a packet is considered is valid, parse it
    if len(messageArray) > 0:
        for message in messageArray:
            # split each message by its delimiter
            values = message.split(",")
            if len(values) > 0:
                if values[0] != '':
                    if (int(values[0]) in [stateUpdatePacketHeader,customColorUpdatePacketHeader]):
                        multiCastMessage(message)
                    if len(values) > 1:
                        hardwareIndex = int(values[1])
                        if (hardwareIndex == 0):
                            multiCastMessage(message)
                        else:
                            # find serial index by using hardware index
                            index = findSerialIndexForHardwareIndex(hardwareIndex)
                            messageDict[index].append(message)

#-----
# helper to convert a hardware index to a serial index
def findSerialIndexForHardwareIndex(hardwareIndex):
    serialIndex = 0
    retIndex = -1
    for serialDevices in lightHardwareIndices:
        for lightIndex in serialDevices:
            if lightIndex == hardwareIndex:
                retIndex = serialIndex
        serialIndex = serialIndex + 1
    return retIndex

#-----
# Takes a message and puts it in every serial devices message dictionary
def multiCastMessage(message):
    index = 0
    for devices in lightHardwareIndices:
        messageDict[index].append(message)
        index = index + 1

#-----
# takes a message and, if theres more than one serial device and theres a 0
# as hardware index, convert it into multiple packets
def convertMultiCastPackets(message, serialIndex):
    global lightHardwareIndices
    individualMessage = message.split("&")
    values = individualMessage[0].split(",")
    messageArray = []
    headerIndex = 0
    if (len(values) > 0):
        headerIndex = values[0]
        # send the message as is if its a state update or custom color update
        if not(headerIndex == str(stateUpdatePacket) or headerIndex == str (customColorUpdatePacketHeader)):
            return [message]
    if (len(values) > 1):
        headerIndex = values[0]
        hardwareIndex = values[1]
        if (hardwareIndex == "0"):
            hardwareIndexArray = lightHardwareIndices[serialIndex]
            # for each device, create a packet
            for hardwareIndex in hardwareIndexArray:
                values[1] = hardwareIndex
                packet = createPacket(values)
                messageArray.append(packet)
        else:
            packet = createPacket(values)
            messageArray.append(packet)
    return messageArray

#-----
# Takes a message that contains a CRC, splits it and stores the given CRC,
# then it computes a new CRC and checks if they are the same. If they are the
# same, this function return true, otherwise, it returns false.
def checkCRC(message):
    # check for packet with no &
    ampCheckList = message.split("&")
    if (len(ampCheckList) != 1):
        crcSplitArray = message.split("#")
        if len(crcSplitArray) == 2:
            payload = crcSplitArray[0]
            givenCRC = crcSplitArray[1].rstrip()[:-1]
            #print "Given: " + givenCRC
            computedCRC = crcCalculator(payload)
            #print "Computed: " + str(computedCRC)
            if str(computedCRC) == givenCRC:
                return payload, True
            else:
                return payload, False
        else:
            return message, False
    else:
        return message, False

#-----
# create a packet based on an array of integers. This automatically adds a
# CRC but it does not add a ";"
def createPacket(valueArray):
    packet = ""
    # generate the packet again
    for value in valueArray:
        packet += str(value)
        packet += ","
    # remove the last comma
    packet = packet[:-1]
    packet += "&"
    packet = appendCRC(packet)
    return packet

#--------------------------------
# Discovery Functions
#--------------------------------

#-----
# True if a discovery packet, Faxlse if not
def checkIfDiscoveryPacket(message):
   if (message == "DISCOVERY_PACKET"):
       return True
   else:
       return False

#-----
# builds a discovery packet based on the information from the discovery serial loop
def buildDiscoveryPacket():
    discoveryPacket = "DISCOVERY_PACKET,"
    discoveryPacket += str(majorAPILevel) + ","
    discoveryPacket += str(minorAPILevel) + ","
    discoveryPacket += str(useCRC) + ","
    discoveryPacket += str(1) + "," # hardware capabilities flag, 1 for raspberry pi
    discoveryPacket += str(maxPacketSizeServer) + ","
    discoveryPacket += str(deviceCount) + "@"
    for i in range(deviceCount):
        discoveryPacket += nameList[i]
        discoveryPacket += ","
        discoveryPacket += typeList[i]
        discoveryPacket += ","
        discoveryPacket += productList[i]
        if (i != deviceCount - 1):
            discoveryPacket += ","
    discoveryPacket += "&"
    return discoveryPacket

#-----
# append a CRC packet with the correct delimiters based on a packet
def appendCRC(packet):
    if useCRC:
        crc = crcCalculator(packet)
        packet += "#"
        packet += str(crc)
        packet += "&"
    return packet

#-----
# This parses a discovery packet, and, if its valid, sets global variables
# that are used during creating packets and checking packet validity
def parseDiscoveryPacket(packet, serialIndex):
    global useCRC
    global deviceCount
    global maxPacketSizeList
    global majorAPILevel
    global minorAPILevel
    global nameList
    global typeList
    global productList
    hardwareCapabilities = 0  # arduinos will give a 0, server will override with 1
    packetSplitArray = packet.split("&")
    discoveryAndNameSplitArray = packetSplitArray[0].split("@")
    discoverySplitArray = discoveryAndNameSplitArray[0].split(",")
    nameSplitArray = discoveryAndNameSplitArray[1].split(",")
    if len(discoverySplitArray) == 7:
        try:
            majorAPILevel = int(discoverySplitArray[1])
            minorAPILevel = int(discoverySplitArray[2])
            useCRC = int(discoverySplitArray[3])
            hardwareCapabilities = int(discoverySplitArray[4])
            maxPacketSize = int(discoverySplitArray[5])
            count = int(discoverySplitArray[6])
            packetIndex = 0
            for x in range(count * 3):
                if packetIndex == 0:
                    deviceCount = deviceCount + 1
                    nameList.append(nameSplitArray[x])
                    packetIndex = 1
                elif packetIndex == 1:
                    typeList.append(nameSplitArray[x])
                    packetIndex = 2
                elif packetIndex == 2:
                    productList.append(nameSplitArray[x])
                    packetIndex = 0

            if (useCRC == 1 or useCRC == 0) \
                and majorAPILevel == 3 \
                and deviceCount < 20 \
                and maxPacketSize < 500:
                maxPacketSizeList[serialIndex] = maxPacketSize
                return True
        except ValueError:
            pass
    return False


#-----
# Read serial streams looking only for a discovery packet. Return false if anything
# else is returned, return true if and only if a parseable discovery packet is found.
def readForDiscovery(serialPort, serialIndex):
    message = readSerialPort(serialPort)
    if len(message) >= 16:
        subString = message[0:16]
        if checkIfDiscoveryPacket(subString):
            return parseDiscoveryPacket(message, serialIndex)
    return False

#-----
# looks through a state update packet received during discovery and determines
# the hardware indices associated with the serial device. These are then added
# into the lightHardwareIndices list
def parseStateUpdateForHardwareIndices(serialPort, serialIndex):
    global lightHardwareIndices
    packet = readSerialPort(serialPort)
    if len(packet) >= 5:
        packet = packet.rstrip()[:-1]
        packet, passedCRC = checkCRC(packet)
        if passedCRC:
            # break up the packet into multiple messages
            messageArray = packet.split("&")
            # only if a packet is considered is valid, parse it
            if len(messageArray) > 0:
                messageisValid = False
                for message in messageArray:
                    # split each message by its delimiter
                    values = message.split(",")
                    if len(values) == 13:
                        if values[0] == str(stateUpdatePacketHeader):
                            lightHardwareIndices[serialIndex].append(int(values[1]))
                            messageIsValid = True
                return messageIsValid
    return False

#-----
# Builds a packet to request state updates
def stateUpdatePacket():
    packet = str(stateUpdatePacketHeader)
    packet += "&"
    packet = appendCRC(packet)
    packet += ";"
    return packet

#--------------------------------
# CRC Functions
#--------------------------------

# lookup table to speed up the long divsion of a CRC
crcTable = [
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c,
    ]

#-----
# uses the lookup table to compute the changes to the CRC
def crcUpdate(crc, data):
    tableIndex = 0
    tableIndex = bitwise_xor(crc,
                             c_uint32((data >> (0 * 4))))
    crc = bitwise_xor(c_uint32(crcTable[tableIndex & 0x0f]),
                      c_uint32((right_shift(crc, c_uint32(4)))))
    tableIndex = bitwise_xor(crc,
                             c_uint32((data >> (1 * 4))))
    crc = bitwise_xor(c_uint32(crcTable[tableIndex & 0x0f]),
                      c_uint32((right_shift(crc, c_uint32(4)))))
    return crc

#-----
# Takes a message, loops through its characters, and computes a CRC that can
# represent that message
def crcCalculator(message):
    crc = c_uint32(~0L)
    #print "input: " + input
    for c in message:
        crc = crcUpdate(crc, ord(c))
        #print "CRC: " + str(crc)
    try:
        crc = long(~crc)
    except:
        crc = long(0)
    #print "final crc: " + str(crc)
    return crc


#--------------------------------
# Serial Functions
#--------------------------------

#-----
# take messages from the message dictionary, turn them into proper
# serial messages, then send them to the serial device
def writeSerialMessages():
    for x in range(0, numOfSerialDevices):
        if (len(messageDict[x])):
            message = convertMessageListToPacket(messageDict[x], x)
            serialDevices[x].write(message)

#-----
# Takes a serial port as an argument, reads all available
# characters, then echoes them on UDP
def echoSerial(serialPort, serialIndex):
    message = readSerialPort(serialPort)
    if message != '':
        # strip off the delimiting character
        message = message.rstrip()[:-1]
        # split into individual packets
        messageSplitArray = message.split(";")
        for message in messageSplitArray:
            messageNoCRC, passedCRC = checkCRC(message)
            # if serial device count is larger than zero, rewrite hardware index
            if (passedCRC):
                #print "ARDUINO: %r " % (message)
                if (numOfSerialDevices > 1):
                    messageArray = convertMultiCastPackets(message, serialIndex)
                    for multiCastMessage in messageArray:
                        if (len(multiCastMessage) > 1):
                            sock.sendto(multiCastMessage, (addr[0], UDP_PORT))
                else:
                    if (len(message) > 1):
                        sock.sendto(message, (addr[0], UDP_PORT))


#-----
# read a message from a serial port, returning it when the serial port has
# no more bytes to read
def readSerialPort(serialPort):
    message = ''
    charReturned = 'junk'
    while charReturned:
        # reads up to 200 bytes at a time.
        charReturned = serialPort.read(200)
        message += charReturned
    return message

#--------------------------------
# Setup Connections and Variables
#--------------------------------

#-----
# Check for valid script usage
if (len(sys.argv) < 2):
    print "Incorrect format!"
    print "Please provide at least one or more serial addresses as arguments"
    print "Example: python UDPtoSerial.py /dev/ttyUSB0"
    sys.exit()


#-----
#  Serial Setup
# configure the serial connections (the parameters differs on the device you are connecting to)
print "Setup Serial..."
serialDevices = []
for x in range(1, len(sys.argv)):
    try:
        # a timeout of 0 makes serial.read nonblocking
        serialDevices.append(serial.Serial(sys.argv[x], 9600, timeout=0.0))
    except serial.serialutil.SerialException:
        print "ERROR: Could not connect to serial device at: " + str(sys.argv[x])
        sys.exit()
numOfSerialDevices = len(serialDevices)

#-----------------------------
lightHardwareIndices = [[] for i in xrange(numOfSerialDevices)]
messageDict = {k: [] for k in range(numOfSerialDevices)}
#-----------------------------


# Discovery Info
# These are treated as global variables through the main while loop
#-----------------------------
useCRC = None
deviceCount = 0
discoveryPacket = None
majorAPILevel = None
minorAPILevel = None
udp_data = None
addr = None
nameList = []
typeList = []
productList = []
# this list is used to store the max packet size for each serial device.
maxPacketSizeList = [0 for i in xrange(numOfSerialDevices)]
# set this to define the max packet size sent to the server. The server will
# simplify packets and only send relevant information to different arduinos,
# so it can accept a larger packet size.
maxPacketSizeServer = 250
#-----------------------------

#--------------------------------
# Listen for Connection Confirmation
#--------------------------------

# First, check that a serial stream can be successfully used by sending discovery
# packets to the connetctd serial devices
serialDiscoveryFlags = [False for i in xrange(numOfSerialDevices)]
fullyDiscoveredFlags  = [False for i in xrange(numOfSerialDevices)]
index = 0
while not all(fullyDiscoveredFlags):
    # first send discovery packets and find devices
    if not serialDiscoveryFlags[index]:
        serialDevices[index].write("DISCOVERY_PACKET;")
        serialDiscoveryFlags[index] = readForDiscovery(serialDevices[index], index)
    else:
        # once a serial device is discovered, send state update packets and
        # parse responses to find the hardware indices associated with the serial
        # device.
        serialDevices[index].write(stateUpdatePacket())
        fullyDiscoveredFlags[index] = parseStateUpdateForHardwareIndices(serialDevices[index], index)
        if fullyDiscoveredFlags[index]:
            print "Serial Device #" + str(index) + " found with lighting devices " + str(lightHardwareIndices[index])
            index = index + 1 # move on to the next index
    time.sleep(0.1)

# get max hardware index
maxIndex = -1
for serialDeviceNumbers in lightHardwareIndices:
    for lightIndex in serialDeviceNumbers:
        if lightIndex > maxIndex:
            maxIndex = lightIndex
deviceCount = maxIndex

# create a discovery packet
discoveryPacket = buildDiscoveryPacket()

if deviceCount == 1:
    print "Serial stream confirmed with " + str(deviceCount) + " device."
else:
    print "Serial stream confirmed with " + str(deviceCount) + " devices."

#-----
print "Setup the UDP Socket..."
# set up UDP server.
sock = socket.socket(socket.AF_INET,    # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind(("", UDP_PORT))
sock.settimeout(0.02)

receivedUDP = False
while not receivedUDP:
    try:
        udp_data, addr = sock.recvfrom(512)
        if udp_data:
            receivedUDP = True
        time.sleep(0.02)
    except socket.timeout:
        pass

print "UDP packet received!"

#--------------------------------
# Main loop
#--------------------------------

print "Starting main loop..."
# Once a serial stream has been estbalished, repeat this ad nauseam
while True:
    try:
        # waits to receive data
        udp_data, addr = sock.recvfrom(512)
        #print "UDP: %r from %r" % (udp_data, addr)
        if udp_data:
            # discovery packets are special case, the relevant info from
            # discovery packets are stored in this script already, so instead
            # send back over UDP the stored discoveryPacket string
            if checkIfDiscoveryPacket(udp_data):
                sock.sendto(discoveryPacket, (addr[0], UDP_PORT))
            else:
                messageDict = {k: [] for k in range(numOfSerialDevices)}
                # checks a CRC and strips its information out of the packet
                # if it is not using CRC, this function just returns the packet
                # as is, and returns passedCRC as True
                message, passedCRC = checkCRC(udp_data)
                if (passedCRC or checkIfDiscoveryPacket(message)):
                    # sort messages into a dictionary where the serial devices are used as keys
                    sortMessages(message)
                    # parse the dictionary and send messages to the proper serial devices.
                    writeSerialMessages()
        # loop through all serial devices
        for x in range(0, numOfSerialDevices):
            # check for serial packets and echo if needed
            echoSerial(serialDevices[x], x)
    except socket.timeout:
        pass
