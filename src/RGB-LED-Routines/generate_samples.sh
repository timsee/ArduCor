#!/bin/bash

#------------------------------------------------------------------------------
# A script used to generate hardware-specific sample projects off of the 
# main sample project. Since most users will only have one type of RGB LED 
# array that they are working with, this simplifies the setup since they just
# need to run the proper project. 
#
# If you are committing changes to the sample, the changes should be made on 
# the main sample project that is used to generate all other samples.
#
#
#
# Script Version: 1.1
# Github repository: http://www.github.com/timsee/RGB-LED-Routines
# License: MIT-License, LICENSE provided in root of git repo
#
#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------


#----------------------------------------
# Variables
#----------------------------------------

#project names
MASTER_PROJ="RGB-LED-Routines"
RAINBOWDUINO_PROJ="Rainbowduino-Routines-Sample"
NEOPIXELS_PROJ="NeoPixels-Routines-Sample"
SINGLE_LED_PROJ="Single-LED-Routines-Sample"
CUSTOM_PROJ="Custom-LED-Routines-Sample"

#paths
PROJ_PATH="$MASTER_PROJ.ino"
SAMPLES_PATH=../../samples

# set up identifiers for the hacky preprocessor system
RAINBOWDUINO_FLAG="#if IS_RAINBOWDUINO"
NEOPIXELS_FLAG="#if IS_NEOPIXEL"
SINGLE_LED_FLAG="#if IS_SINGLE_LED"
CUSTOM_FLAG="#if IS_CUSTOM"

YUN_FLAG="#if IS_YUN"
END_FLAG="#endif"

# save possible descriptions
DESCRIPTION_PLACEHOLDER=" * DESCRIPTION_PLACEHOLDER"
RAINBOWDUINO_DESCRIPTION=" * Supports SeeedStudio Rainbowduino projects."
LED_DESCRIPTION=" * Supports a single RGB LED but can be easily hacked to support more."
NEOPIXELS_DESCRIPTION=" * Supports Adafruit NeoPixels products."
CUSTOM_DESCRIPTION=" * Custom Sketch."

COM_PLACEHOLDER=" * COM_PLACEHOLDER"
SERIAL_DESCRIPTION=" * Provides a serial interface to a set of lighting routines generated"
UDP_DESCRIPTION=" * Provides a UDP interface to a set of lighting routines generated"

#----------------------------------------
# Parsing Function
#----------------------------------------

# a function that parses through the preprocessor directives and determines based off
# of them whether or not to include the lines in a hardware specific sample code
function generate_hardware_specific_sample() 
{
    SHOULD_WRITE=1
    cat $PROJ_PATH.temp | while read line
    do
        # check for any flags, don't write the actual line that contains them
        if [ "$line" == "$NEOPIXELS_FLAG" ] \
           || [ "$line" == "$SINGLE_LED_FLAG" ] \
           || [ "$line" == "$RAINBOWDUINO_FLAG" ] \
           || [ "$line" == "$CUSTOM_FLAG" ] \
           || [ "$line" == "$YUN_FLAG" ] \
           || [ "$line" == "$END_FLAG" ];
        then
            SHOULD_WRITE=0
        fi

        # check for placeholder special case
        if [ "$line" == "$DESCRIPTION_PLACEHOLDER" ]
        then
            echo "$3" >> $2
        elif [ "$line" == "$COM_PLACEHOLDER" ]
        then
        	if [ $4 -eq 0 ];
        	then
        	    echo "$SERIAL_DESCRIPTION" >> $2
        	else
        	    echo "$UDP_DESCRIPTION" >> $2
        	fi
        # check if line belongs in sample code
        elif [ $SHOULD_WRITE -eq 1 ];
        then
            echo "$line" >> $2
        fi
        
        # write future lines if out of a preprocessor or in the proper preprocessor
        if  [ "$line" == "$1" ] || [ "$line" == "$END_FLAG" ];
        then
            SHOULD_WRITE=1
        fi
        
        # if its a yun sample, include yun code
        if [ $4 -eq 1 ] && [ "$line" == "$YUN_FLAG" ];
        then
            SHOULD_WRITE=1
        fi
    done
}

#----------------------------------------
# Script
#----------------------------------------

# remove old sample code
rm -rf $SAMPLES_PATH/$RAINBOWDUINO_PROJ
rm -rf $SAMPLES_PATH/$NEOPIXELS_PROJ
rm -rf $SAMPLES_PATH/$SINGLE_LED_PROJ
rm -rf $SAMPLES_PATH/$CUSTOM_PROJ

rm -rf $SAMPLES_PATH/yun/$RAINBOWDUINO_PROJ
rm -rf $SAMPLES_PATH/yun/$NEOPIXELS_PROJ
rm -rf $SAMPLES_PATH/yun/$SINGLE_LED_PROJ
rm -rf $SAMPLES_PATH/$CUSTOM_PROJ
rm -rf $SAMPLES_PATH/yun


# make new directories
mkdir $SAMPLES_PATH/$RAINBOWDUINO_PROJ
mkdir $SAMPLES_PATH/$NEOPIXELS_PROJ
mkdir $SAMPLES_PATH/$SINGLE_LED_PROJ
mkdir $SAMPLES_PATH/$CUSTOM_PROJ

mkdir $SAMPLES_PATH/yun
mkdir $SAMPLES_PATH/yun/$RAINBOWDUINO_PROJ
mkdir $SAMPLES_PATH/yun/$NEOPIXELS_PROJ
mkdir $SAMPLES_PATH/yun/$SINGLE_LED_PROJ
mkdir $SAMPLES_PATH/yun/$CUSTOM_PROJ

IFS='' # preserves the leading whitespace

# remove defines for debugging in IDE
sed '/#define/d' $PROJ_PATH > $PROJ_PATH.temp

# create Rainbowduino sample sketch
echo "Generating Rainbowduino sketches..."
generate_hardware_specific_sample $RAINBOWDUINO_FLAG $SAMPLES_PATH/$RAINBOWDUINO_PROJ/$RAINBOWDUINO_PROJ.ino $RAINBOWDUINO_DESCRIPTION 0
# create yun sample
generate_hardware_specific_sample $RAINBOWDUINO_FLAG $SAMPLES_PATH/yun/$RAINBOWDUINO_PROJ/$RAINBOWDUINO_PROJ.ino $RAINBOWDUINO_DESCRIPTION 1

# create NeoPixel sample project
echo "Generating Neopixels sketches..."
generate_hardware_specific_sample $NEOPIXELS_FLAG $SAMPLES_PATH/$NEOPIXELS_PROJ/$NEOPIXELS_PROJ.ino $NEOPIXELS_DESCRIPTION 0
# create yun sample
generate_hardware_specific_sample $NEOPIXELS_FLAG $SAMPLES_PATH/yun/$NEOPIXELS_PROJ/$NEOPIXELS_PROJ.ino $NEOPIXELS_DESCRIPTION 1

# create Single LED sample project
echo "Generating Single LED sketches..."
generate_hardware_specific_sample $SINGLE_LED_FLAG $SAMPLES_PATH/$SINGLE_LED_PROJ/$SINGLE_LED_PROJ.ino $LED_DESCRIPTION 0
# create yun sample
generate_hardware_specific_sample $SINGLE_LED_FLAG $SAMPLES_PATH/yun/$SINGLE_LED_PROJ/$SINGLE_LED_PROJ.ino $LED_DESCRIPTION 1


# This option allows you to set up your own light configuration into the build scripts
# It is currently set up for a test system that uses two neopixel libraries and repeats
# the commands it receives over serial.
#
# It is commented out and not distributed with the sample projects.
#
#echo "Generating custom sketches..."
#generate_hardware_specific_sample $CUSTOM_FLAG $SAMPLES_PATH/$CUSTOM_PROJ/$CUSTOM_PROJ.ino $CUSTOM_DESCRIPTION 0
# create yun version of custom
#generate_hardware_specific_sample $CUSTOM_FLAG $SAMPLES_PATH/yun/$CUSTOM_PROJ/$CUSTOM_PROJ.ino $CUSTOM_DESCRIPTION 1

unset IFS # always unset IFS after you're done using it!

#----------------------------------------
# cleanup
#----------------------------------------

# remove temporary data
rm -rf $PROJ_PATH.temp

# Not really necessary, but useful for consistency. Reverts any testing done 
# on the main project and reset it so that its always set to NeoPixels
sed "1s/.*/#define IS_RAINBOWDUINO 0/" $MASTER_PROJ.ino > temp.txt ; mv temp.txt $MASTER_PROJ.ino
sed "2s/.*/#define IS_NEOPIXEL 1/" $MASTER_PROJ.ino > temp.txt ; mv temp.txt $MASTER_PROJ.ino
sed "3s/.*/#define IS_SINGLE_LED 0/" $MASTER_PROJ.ino > temp.txt ; mv temp.txt $MASTER_PROJ.ino
sed "4s/.*/#define IS_CUSTOM 0/" $MASTER_PROJ.ino > temp.txt ; mv temp.txt $MASTER_PROJ.ino


