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

TARGET = LightingGUI
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    multicolorpage.cpp \
    singlecolorpage.cpp \
    settingspage.cpp \
    datalayer.cpp \
    commlayer.cpp \
    icondata.cpp \
    lightsslider.cpp \
    arraycolorspage.cpp \
    colorpicker.cpp

HEADERS  += mainwindow.h \
    multicolorpage.h \
    singlecolorpage.h \
    settingspage.h \
    datalayer.h \
    commlayer.h \
    icondata.h \
    lightsslider.h \
    arraycolorspage.h \
    lightingpage.h \
    colorpicker.h

FORMS    += mainwindow.ui \
    multicolorpage.ui \
    singlecolorpage.ui \
    settingspage.ui \
    arraycolorspage.ui

RESOURCES     = qdarkstyle/style.qrc \
    extraresources.qrc

CONFIG += c++11

