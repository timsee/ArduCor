/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "settingspage.h"
#include "ui_settingspage.h"

#include <QDebug>

SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage) {
    ui->setupUi(this);

    // setup sliders
    ui->speedSlider->slider->setRange(1, 1000);
    ui->speedSlider->slider->setValue(300);
    ui->speedSlider->setSliderHeight(0.6f);
    ui->timeoutSlider->slider->setRange(0,240);
    ui->timeoutSlider->slider->setValue(120);
    ui->timeoutSlider->setSliderHeight(0.6f);

    // setup the signals and slots
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedChanged(int)));
    connect(ui->timeoutSlider, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));

    connect(ui->serialList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listClicked(QListWidgetItem*)));
}

SettingsPage::~SettingsPage() {
    delete ui;
}

// ----------------------------
// Slots
// ----------------------------

void SettingsPage::speedChanged(int newSpeed) {
    mData->speed(newSpeed);
    mComm->sendSpeed(mData->speed());
}

void SettingsPage::timeoutChanged(int newTimeout) {
   mData->timeOut(newTimeout);
   mComm->sendTimeOut(mData->timeOut());
}

//TODO:COMM LISTS
void SettingsPage::listClicked(QListWidgetItem* item) {
    // only change connection if a new one is clicked
    if (QString::compare(item->text(), mComm->comm()->name())) {
        // check a mac os x case where it adds a .cu for certain connections
        QString cuCheck = QString("cu.%1").arg(item->text());
        if (!QString::compare(cuCheck, mComm->comm()->name())) {
            // do nothing
        } else {
            // close the current connection
            //mComm->comm()->closeConnection();
            // connect to the selected connection
           // mComm->mSerial->connectSerialPort(item->text());
        }
    }
}


// ----------------------------
// Protected
// ----------------------------


void SettingsPage::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    // set the sliders to their proper values
    ui->speedSlider->slider->setValue(mData->speed());
    ui->timeoutSlider->slider->setValue(mData->timeOut());

    // default the settings bars to the current colors
    if (mData->currentRoutine() <= ELightingRoutine::eSingleGlimmer) {
        ui->speedSlider->setSliderColorBackground(mData->mainColor());
        ui->timeoutSlider->setSliderColorBackground(mData->mainColor());
    } else {
        ui->speedSlider->setSliderColorBackground(mData->colorsAverage(mData->currentColorGroup()));
        ui->timeoutSlider->setSliderColorBackground(mData->colorsAverage(mData->currentColorGroup()));
    }
#ifndef MOBILE_BUILD

    // update the serial list, if needed
    updateSerialList();

    // select the QSerialPort portname that matches the currently connected port
    for (int i = 0; i < ui->serialList->count(); i++) {
        QListWidgetItem *item = ui->serialList->item(i);
        if (!QString::compare(item->text(), mComm->comm()->name())) {
            item->setSelected(true);
        } else {
            item->setSelected(false);
        }
    }
#endif //MOBILE_BUILD
}


// ----------------------------
// Private
// ----------------------------

//TODO:COMM LISTS
void SettingsPage::updateSerialList() {
//    // look for updates in the serial port list
//    for (QSerialPortInfo port : mComm->mSerial->serialList) {
//        bool itemFound = false;

//        for (int i = 0; i < ui->serialList->count(); i++) {
//            QListWidgetItem *item = ui->serialList->item(i);
//            if (!QString::compare(item->text(), port.portName())) {
//                itemFound = true;
//            }

//        }
//        if (!itemFound) {
//            ui->serialList->addItem(port.portName());
//        }
//    }
}
