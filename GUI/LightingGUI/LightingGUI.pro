#-------------------------------------------------
#
# LightingGUI
# Copyright (C) 2015 - 2016. All Rights MIT Licensed.
#
# Project created by QtCreator 2015-12-26T19:10:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets serialport

# windows icon
RC_ICONS = images/icon.ico
# Mac OS X icon
ICON = images/icon.icns

TARGET = LightingGUI
TEMPLATE = app

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
    presetcolorspage.cpp

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
    presetcolorspage.h

FORMS    += mainwindow.ui \
    singlecolorpage.ui \
    settingspage.ui \
    customcolorspage.ui \
    presetcolorspage.ui

RESOURCES     = qdarkstyle/style.qrc \
    extraresources.qrc

CONFIG += c++11

