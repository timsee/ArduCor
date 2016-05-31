#-------------------------------------------------
#
# LightingGUI
# Copyright (C) 2015 - 2016. All Rights MIT Licensed.
#
# Project created by QtCreator 2015-12-26T19:10:52
#
#-------------------------------------------------

TARGET = LightingGUI
TEMPLATE = app

#----------
# Minimum Requirements Check
#----------

# check for Qt5
!greaterThan(QT_MAJOR_VERSION, 4){
#todo handle the case of 5.2 or greater, or 6.0 or greater
    error(ERROR: Qt5 is not installed. This project uses QT5.2 or later)
}

#todo: check for C++11
CONFIG += c++11 #adds C++11 support

#----------
# Qt Linking
#----------

QT   += core gui widgets network

# Does not set up the qt serial port on mobile devices
# since they can't support it.
!android:!ios {
  QT +=  serialport
}

# MOBILE_BUILD is a flag that gets set for only MOBILE targets.
# This is useful for things liek QSerialPort, which don't translate
# well to a mobile device and are not supported by Qt.
android {
   DEFINES += MOBILE_BUILD=1
   # Android Manifests are the top level global xml for things like
   # app name, icons, screen orientations, etc.
   OTHER_FILES += android-sources/AndroidManifest.xml
   ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
}

ios {
   DEFINES += MOBILE_BUILD=1
   # Info.plist is the top level global configuration file for iOS
   # for things like app name, icons, screen orientations, etc.
   QMAKE_INFO_PLIST = ios-sources/Info.plist
   # adds the icon files to the iOS application
   ios_icon.files = $$files($$PWD/ios-sources/icon/AppIcon*.png)
   QMAKE_BUNDLE_DATA += ios_icon
}

#----------
# Sources
#----------

SOURCES += main.cpp\
    mainwindow.cpp \
    singlecolorpage.cpp \
    settingspage.cpp \
    datalayer.cpp \
    commlayer.cpp \
    icondata.cpp \
    lightsslider.cpp \
    colorpicker.cpp \
    lightsbutton.cpp \
    customcolorspage.cpp \
    presetcolorspage.cpp \
    commserial.cpp \
    commhttp.cpp \
    commudp.cpp \
    commtype.cpp

HEADERS  += mainwindow.h \
    singlecolorpage.h \
    settingspage.h \
    datalayer.h \
    commlayer.h \
    icondata.h \
    lightsslider.h \
    lightingpage.h \
    colorpicker.h \
    lightsbutton.h \
    lightingprotocols.h \
    customcolorspage.h \
    presetcolorspage.h \
    commtype.h \
    commserial.h \
    commhttp.h \
    commudp.h

FORMS    += mainwindow.ui \
    singlecolorpage.ui \
    settingspage.ui \
    customcolorspage.ui \
    presetcolorspage.ui

#----------
# Resources
#----------

RESOURCES     = qdarkstyle/style.qrc \
    extraresources.qrc

RC_ICONS = images/icon.ico # Windows icon
ICON = images/icon.icns    # Mac OS X icon




