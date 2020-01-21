#!/bin/bash

#------------------------------------------------------------------------------
# A script used to generate hardware-specific sample projects off of the 
# main sample project. Since most users will only have one type of RGB LED 
# array that they are working with, this simplifies the setup since they just
# need to run the proper project.
#
#
# Script Version: 1.5
# Github repository: http://www.github.com/timsee/ArduCor
# License: MIT-License, LICENSE provided in root of git repo
#
#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------

#----------------------------------------
# Helper Functions
#----------------------------------------

# taken from http://stackoverflow.com/a/33248547 to trim leading
# and trailing whitespace
trim() {
  local s2 s="$*"
  # note: the brackets in each of the following two lines contain one space
  # and one tab
  until s2="${s#[   ]}"; [ "$s2" = "$s" ]; do s="$s2"; done
  until s2="${s%[   ]}"; [ "$s2" = "$s" ]; do s="$s2"; done
  echo "$s"
}

#----------------------------------------
# Project Name Setup
#----------------------------------------
PROJECT_NAME=(
    "Rainbowduino"
    "Neopixels"
    "Single-LED"
)
# this array gets filled automatically based off of the
# PROJECT_NAME array
PROJECT_FLAG=()
for project_id in "${PROJECT_NAME[@]}"
do
    project_id=`echo ${project_id} | tr [a-z] [A-Z] | tr '-' '_'`
    PROJECT_FLAG["${#PROJECT_FLAG[@]}"]="#if IS_${project_id}"
done

# Give the line that would overwrite the sketch's PLACEHOLDER_DESCRIPTION
# the order should match the order in PROJECT_NAME, ie if the first PROJECT_NAME
# variable is Rainbowduino, then the first description here should be RAINBOWDUINO.
PROJECT_DESCRIPTION=(
    " * Supports SeeedStudio Rainbowduino projects."
    " * Supports Adafruit NeoPixels products."
    " * Supports a single RGB LED but can be easily hacked to support more."
)

#----------------------------------------
# Miscellaneous Setup
#----------------------------------------

# placeholders used as special cases. These take a line from the
# PROJECT_DESCRIPTION instead of using
# what is in the sample code.
DESCRIPTION_PLACEHOLDER=" * DESCRIPTION_PLACEHOLDER"

#paths
PROJ_PATH="SimpleSample.ino"
SAMPLES_PATH=../../samples/Simple

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
    FLAG="${PROJECT_FLAG[${2}]}"
    DESCRIPTION="${PROJECT_DESCRIPTION[${2}]}"

    WRITE_PROJ="${1}-ArduCor-Sample"
    WRITE_PATH_DIR="${SAMPLES_PATH}/${WRITE_PROJ}"

    if [ "$WRITE_PATH_DIR" != "" ]
    then

    WRITE_PATH=${WRITE_PATH_DIR}/${WRITE_PROJ}.ino
    if [ -d $WRITE_PATH_DIR ]
    then
        rm -r $WRITE_PATH_DIR               # remove the old version of the sketch
    fi
    mkdir $WRITE_PATH_DIR                   # remake the directory
    SHOULD_WRITE=1
    cat $PROJ_PATH.temp | while read line   # parse the master project line by line
    do
        #trims leading and trailing whitespace, used for some edge cases.
        trimmed=$(trim "$line")

        #---------------------
        # Set write flag, if needed
        #---------------------
        # check for any flags, don't write the actual line that contains them
        # first it checks if line exists in the PROJECT_FLAG array, then it checks
        # our special case variables.
        if  [[ "${PROJECT_FLAG[@]}" =~ "$line" ]]   \
            || [ "$trimmed" == "${END_FLAG}" ];
        then
            # handle edge case in certain environments.
            if [ "${line}" != "" ]
            then
                SHOULD_WRITE=0
            fi
        fi

        #---------------------
        # Write Line, if write flag is set
        #---------------------
        # check for description placeholder special case
        if [ "$line" == "$DESCRIPTION_PLACEHOLDER" ]
        then
            echo "${DESCRIPTION}" >> $WRITE_PATH
        # DEFAULT: check if line belongs in the current sample
        elif [ "${SHOULD_WRITE}" -eq 1 ]
        then
            echo "$line" >> "$WRITE_PATH"
        fi
        #---------------------
        # Reset write flag, if needed
        #---------------------
        # write future lines if out of a preprocessor or in the proper preprocessor
        if  [ "$trimmed" == "$FLAG" ]        \
            || [ "$trimmed" == "$END_FLAG" ];
        then
            SHOULD_WRITE=1
        fi
    done
    fi
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
# on the main project and reset it so that its always set to NeoPixels and serial
line_num=1
for project_id in "${PROJECT_NAME[@]}"
do
    # setup project name in preprocessor style
    PROJECT=`echo ${project_id} | tr [a-z] [A-Z] | tr '-' '_'`
    sed "${line_num}s/.*/#define IS_${PROJECT} 0 /" $PROJ_PATH > temp.txt
    mv temp.txt $PROJ_PATH
    line_num=$(($line_num+1))
done

# reset the second line to be #define IS_NEOPIXELS 1 since thats the default testing environment
sed "2s/.*/#define IS_NEOPIXELS 1 /" $PROJ_PATH > temp.txt ; mv temp.txt $PROJ_PATH

# remove temporary data
rm -r $PROJ_PATH.temp



