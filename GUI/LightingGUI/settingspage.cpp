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
    //TODO: fix edge case during loading that requires loading
    //      the serial list twice. for now, this flag is a workaround.
    mFirstLoad = true;

    // setup sliders
    ui->speedSlider->slider->setRange(1, 1000);
    ui->speedSlider->slider->setValue(300);
    ui->timeoutSlider->slider->setRange(0,240);
    ui->timeoutSlider->slider->setValue(120);

    // setup the signals and slots
    connect(ui->speedSlider, SIGNAL(valueChanged(int)),
            this, SLOT(speedChanged(int)));
    connect(ui->timeoutSlider, SIGNAL(valueChanged(int)),
            this, SLOT(timeoutChanged(int)));

    connect(ui->serialList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(listClicked(QListWidgetItem*)));
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

void SettingsPage::listClicked(QListWidgetItem* item) {
    // only change serial ports if a new one is clicked
    if (QString::compare(item->text(), mComm->serial->portName())) {
        // check a mac os x case where it adds a .cu for certain connections
        QString cuCheck = QString("cu.%1").arg(item->text());
        if (!QString::compare(cuCheck, mComm->serial->portName())) {
            // do nothing
        } else {
            // close the current serial port
            mComm->closeSerialPort();
            // connect to the selected serial port
            mComm->connectSerialPort(item->text());
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

    // update the serial list, if needed
    updateSerialList();

    // run the update twice the first time, so that it can properly
    // select the active serial port.
    //TODO: remove the need for this
    if (mFirstLoad) {
        mFirstLoad = false;
        updateSerialList();
    }
}


// ----------------------------
// Private
// ----------------------------


void SettingsPage::updateSerialList() {
    // look for updates in the serial port list
    for (QSerialPortInfo port : mComm->serialList) {
        bool itemFound = false;

        for (int i = 0; i < ui->serialList->count(); i++) {
            QListWidgetItem *item = ui->serialList->item(i);
            if (!QString::compare(item->text(), port.portName())) {
                itemFound = true;
            }

        }
        if (!itemFound) {
            ui->serialList->addItem(port.portName());
        }
    }
}
