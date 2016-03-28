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
# Script Version: 1.2
# Github repository: http://www.github.com/timsee/RGB-LED-Routines
# License: MIT-License, LICENSE provided in root of git repo
#
#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------

# Currently a variable set to 0 because we haven't implemented the yun samples yet
# Eventually, we'll remove this variable.
GENERATE_YUN_SAMPLES=0

#----------------------------------------
# Project Setup
#----------------------------------------
# When adding a new project, make sure that you maintain the order of these arrays.
# This script is quick and dirty and assumes that all project's are in the same
# order in each array.
PROJECT_NAME=(
    "Rainbowduino"
    "Neopixels"
    "Single-LED"
    "Custom"
)

# Give the line that would overwrite the sketch's PLACEHOLDER_DESCRIPTION
PROJECT_DESCRIPTION=(
    " * Supports SeeedStudio Rainbowduino projects."
    " * Supports Adafruit NeoPixels products."
    " * Supports a single RGB LED but can be easily hacked to support more."
    " * Custom Sketch."
)

# communication descriptions
SERIAL_COMM_DESCRIPTION=" * Provides a serial interface to a set of lighting routines generated"
UDP_COMM_DESCRIPTION=" * Provides a UDP interface to a set of lighting routines generated"

# this array gets filled automatically based off of the
# PROJECT_NAME array
PROJECT_FLAG=()
for project_id in "${PROJECT_NAME[@]}"
do
    project_id=`echo ${project_id} | tr [a-z] [A-Z] | tr '-' '_'`
    PROJECT_FLAG["${#PROJECT_FLAG[@]}"]="#if IS_${project_id}"
done

#----------------------------------------
# Variable Setup
#----------------------------------------

# save possible descriptions
DESCRIPTION_PLACEHOLDER=" * DESCRIPTION_PLACEHOLDER"
COM_PLACEHOLDER=" * COM_PLACEHOLDER"

#paths
PROJ_PATH="RGB-LED-Routines.ino"
SAMPLES_PATH=../../samples

# set up identifiers for the hacky preprocessor system
YUN_FLAG="#if IS_YUN"
END_FLAG="#endif"


#----------------------------------------
# Parsing Function
#----------------------------------------
# a function that parses through the preprocessor directives and determines based off
# of them whether or not to include the lines in a hardware specific sample code
# This function creates potentially two samples: a standard sample that uses serial
# comunication and a a yun-specific sample that uses UDP communication.
function generate_hardware_specific_sample() 
{
    # set the project-specific variables
    WRITE_PROJ="${1}-Routines-Sample"
    FLAG="${PROJECT_FLAG[${2}]}"
    DESCRIPTION="${PROJECT_DESCRIPTION[${2}]}"

    # creates two sets of hardware samples and repeat the sketch
    # generation twice: once for serial communication and once for UDP
    for (( i=1; i<=2; i++ )) ;
    do
        # determines what flags to use when generating the sample
        WRITE_PATH_DIR=""
        if [ $i -eq 1 ]
        then
            # setup the serial communication variables
            COMM_DESCRIPTION="$SERIAL_COMM_DESCRIPTION"
            WRITE_PATH_DIR="${SAMPLES_PATH}/${WRITE_PROJ}"
        elif [ $i -eq 2 ] && [ $GENERATE_YUN_SAMPLES -eq 1 ]
        then
            # setup the UDP communication variables
            COMM_DESCRIPTION="$UDP_COMM_DESCRIPTION"
            WRITE_PATH_DIR="${SAMPLES_PATH}/yun/${WRITE_PROJ}"
        fi

        if [ "$WRITE_PATH_DIR" != "" ]
        then

            WRITE_PATH=${WRITE_PATH_DIR}/${WRITE_PROJ}.ino
            rm -rf $WRITE_PATH_DIR                  # remove the old version of the sketch
            mkdir $WRITE_PATH_DIR                   # remake the directory
            SHOULD_WRITE=1
            cat $PROJ_PATH.temp | while read line   # parse the master project line by line
            do
                #---------------------
                # Set write flag, if needed
                #---------------------
                # check for any flags, don't write the actual line that contains them
                # first it checks if line exists in the PROJECT_FLAG array, then it checks
                # our special case variables.
                if  [[ "${PROJECT_FLAG[@]}" =~ "$line" ]]  \
                    || [ "$line" == "${YUN_FLAG}" ] \
                    || [ "$line" == "${END_FLAG}" ];
                then
                    SHOULD_WRITE=0
                fi

                #---------------------
                # Write Line, if write flag is set
                #---------------------
                # check for placeholder special case
                if [ "$line" == "$DESCRIPTION_PLACEHOLDER" ]
                then
                    echo "${DESCRIPTION}" >> $WRITE_PATH
                elif [ "$line" == "$COM_PLACEHOLDER" ]
                then
                    echo "$COMM_DESCRIPTION" >> $WRITE_PATH
                # check if line belongs in sample code
                elif [ "${SHOULD_WRITE}" -eq 1 ]
                then
                    echo "$line" >> $WRITE_PATH
                fi

                #---------------------
                # Reset write flag, if needed
                #---------------------
                # write future lines if out of a preprocessor or in the proper preprocessor
                if  [ "$line" == "$FLAG" ] || [ "$line" == "$END_FLAG" ];
                then
                    SHOULD_WRITE=1
                fi

                # if its a yun sample, include yun code
                if [ "$COMM_DESCRIPTION" == "$UDP_COMM_DESCRIPTION" ] && [ "$line" == "$YUN_FLAG" ];
                then
                    SHOULD_WRITE=1
                fi
            done
        fi
    done
}


#----------------------------------------
# Script Body
#----------------------------------------
# This is the part of the script where setup is finished, and execution begins.

# remove defines during code generation
sed '/#define/d' $PROJ_PATH > $PROJ_PATH.temp

IFS='' # preserves the leading whitespace
# Generates both the standard and yun versions of the sketches
# by calling the generate_hardware_specific_sample function
project_num=0
for project_id in "${PROJECT_NAME[@]}"
do
    echo "Generating ${project_id} sketches..."
    generate_hardware_specific_sample $project_id $project_num
    project_num=$(($project_num+1))
done
unset IFS # always unset IFS after you're done using it!


#----------------------------------------
# Cleanup
#----------------------------------------

# Not really necessary, but useful for consistency. Reverts any testing done
# on the main project and reset it so that its always set to NeoPixels
line_num=1
for project_id in "${PROJECT_NAME[@]}"
do
    # setup project name in preprocessor style
    PROJECT=`echo ${project_id} | tr [a-z] [A-Z] | tr '-' '_'`
    sed "${line_num}s/.*/#define IS_${PROJECT} 0 /" $PROJ_PATH > temp.txt ; mv temp.txt $PROJ_PATH
    line_num=$(($line_num+1))
done

# reset the second line to be #define IS_NEOPIXELS 1 since thats the default testing environment
sed "2s/.*/#define IS_NEOPIXELS 1 /" $PROJ_PATH > temp.txt ; mv temp.txt $PROJ_PATH

# remove temporary data
rm -rf $PROJ_PATH.temp



