/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "settingspage.h"
#include "ui_settingspage.h"

#include <QDebug>
#include <QSignalMapper>

SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage) {
    ui->setupUi(this);

    // setup sliders
    mSliderSpeedValue = 425;
    ui->speedSlider->slider->setRange(1, 1000);
    ui->speedSlider->slider->setValue(mSliderSpeedValue);
    ui->speedSlider->setSliderHeight(0.5f);
    ui->speedSlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->speedSlider->slider->setTickInterval(100);

    ui->timeoutSlider->slider->setRange(0,240);
    ui->timeoutSlider->slider->setValue(120);
    ui->timeoutSlider->setSliderHeight(0.5f);
    ui->timeoutSlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->timeoutSlider->slider->setTickInterval(40);

    QSignalMapper *commTypeMapper = new QSignalMapper(this);
#ifndef MOBILE_BUILD
    ui->serialButton->setCheckable(true);
    connect(ui->serialButton, SIGNAL(clicked(bool)), commTypeMapper, SLOT(map()));
    commTypeMapper->setMapping(ui->serialButton, (int)ECommType::eSerial);
#else
    // hide PC-specific elements
    ui->serialButton->setHidden(true);
    ui->availablePortsLabel->setHidden(true);
#endif //MOBILE_BUILD

    ui->httpButton->setCheckable(true);
    connect(ui->httpButton, SIGNAL(clicked(bool)), commTypeMapper, SLOT(map()));
    commTypeMapper->setMapping(ui->httpButton, (int)ECommType::eHTTP);

    ui->udpButton->setCheckable(true);
    connect(ui->udpButton, SIGNAL(clicked(bool)), commTypeMapper, SLOT(map()));
    commTypeMapper->setMapping(ui->udpButton, (int)ECommType::eUDP);

    connect(commTypeMapper, SIGNAL(mapped(int)), this, SLOT(commTypeSelected(int)));
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedChanged(int)));
    connect(ui->timeoutSlider, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));
    connect(ui->connectionList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listClicked(QListWidgetItem*)));
    connect(ui->plusButton, SIGNAL(clicked(bool)), this, SLOT(plusButtonClicked()));
    connect(ui->minusButton, SIGNAL(clicked(bool)), this, SLOT(minusButtonClicked()));
}

SettingsPage::~SettingsPage() {
    delete ui;
}


// ----------------------------
// Slots
// ----------------------------

void SettingsPage::speedChanged(int newSpeed) {
    mSliderSpeedValue = newSpeed;
    int finalSpeed;
    // first half of slider is going linearly between 20 FPS down to 1 FPS
    if (newSpeed < 500) {
        float percent = newSpeed / 500.0f;
        finalSpeed = (int)((1.0f - percent) * 2000.0f);
    } else {
        // second half maps 1FPS to 0.01FPS
        float percent = newSpeed - 500.0f;
        finalSpeed = (500 - percent) / 5.0f;
        if (finalSpeed < 2.0f) {
            finalSpeed = 2.0f;
        }
    }
    mData->speed((int)finalSpeed);
    mComm->sendSpeed(mData->speed());
}

void SettingsPage::timeoutChanged(int newTimeout) {
   mData->timeOut(newTimeout);
   mComm->sendTimeOut(mData->timeOut());
}

//TODO:COMM LISTS
void SettingsPage::listClicked(QListWidgetItem* item) {
    //qDebug() << item->text() << "vs" << mCurrentListString;
    if (QString::compare(item->text(), mCurrentListString)) {
        mCurrentListString = item->text();
#ifndef MOBILE_BUILD
        // serial connections require a little more effort to properly
        // switch, they need to be closed first and then reconnected
        mComm->closeCurrentConnection();
        mComm->changeSerialPort(mCurrentListString);
#endif //MOBILE_BUILD
        mComm->comm()->selectConnection(mCurrentListString);
        ui->lineEdit->setText(mComm->comm()->currentConnection());
    }
}


void SettingsPage::highlightButton(ECommType currentCommType) {
    ui->serialButton->setChecked(false);
    ui->httpButton->setChecked(false);
    ui->udpButton->setChecked(false);

    if (currentCommType == ECommType::eHTTP) {
        ui->httpButton->setChecked(true);
    } else if (currentCommType == ECommType::eUDP) {
        ui->udpButton->setChecked(true);
    }
#ifndef MOBILE_BUILD
    else if (currentCommType == ECommType::eSerial) {
        ui->serialButton->setChecked(true);
    }
#endif //MOBILE_BUILD
}


void SettingsPage::commTypeSelected(int type) {
    if ((ECommType)type != mComm->currentCommType()) {
        mComm->currentCommType((ECommType)type);
        highlightButton((ECommType)type);
        updateConnectionList();
        ui->connectionList->item(mComm->comm()->selectedIndex())->setSelected(true);
    }
#ifndef MOBILE_BUILD
        if ((ECommType)type == ECommType::eSerial) {
            ui->lineEdit->setHidden(true);
            ui->plusButton->setHidden(true);
            ui->minusButton->setHidden(true);
            ui->availablePortsLabel->setHidden(false);
        } else {
            ui->lineEdit->setHidden(false);
            ui->plusButton->setHidden(false);
            ui->minusButton->setHidden(false);
            ui->availablePortsLabel->setHidden(true);
        }
#endif //MOBILE_BUILD

}


void SettingsPage::plusButtonClicked() {
    bool isSuccessful = mComm->comm()->addConnection(ui->lineEdit->text());
    if (isSuccessful) {
        // adjusts the backend to the connection
        mComm->comm()->selectConnection(ui->lineEdit->text());
        // updates the connection list
        updateConnectionList();
        // selects the top connection on the GUI level
        ui->connectionList->item(0)->setSelected(true);
    }
}


void SettingsPage::minusButtonClicked() {
    bool isSuccessful = mComm->comm()->removeConnection(mCurrentListString);
    if (isSuccessful) {
        mCurrentListString = ui->connectionList->item(0)->text();
        // adjusts the backend to the connection
        mComm->comm()->selectConnection(mCurrentListString);
        // update the line edit text
        ui->lineEdit->setText(mCurrentListString);
        // updates the connection list
        updateConnectionList();
        // selects the top connection on the GUI level
        ui->connectionList->item(0)->setSelected(true);
    }

}

// ----------------------------
// Protected
// ----------------------------


void SettingsPage::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    ui->speedSlider->slider->setValue(mSliderSpeedValue);
    ui->timeoutSlider->slider->setValue(mData->timeOut());
    highlightButton(mComm->currentCommType());
    commTypeSelected((int)mComm->currentCommType());
    // default the settings bars to the current colors
    if (mData->currentRoutine() <= ELightingRoutine::eSingleSineFade) {
        ui->speedSlider->setSliderColorBackground(mData->mainColor());
        ui->timeoutSlider->setSliderColorBackground(mData->mainColor());
    } else {
        ui->speedSlider->setSliderColorBackground(mData->colorsAverage(mData->currentColorGroup()));
        ui->timeoutSlider->setSliderColorBackground(mData->colorsAverage(mData->currentColorGroup()));
    }

    // update the serial list, if needed
    updateConnectionList();
#ifndef MOBILE_BUILD
    if (mComm->currentCommType() != ECommType::eSerial) {
#endif //MOBILE_BUILD
      ui->connectionList->item(mComm->comm()->selectedIndex())->setSelected(true);
      mCurrentListString = ui->connectionList->item(0)->text();
#ifndef MOBILE_BUILD
    } else {
        ui->connectionList->item(mComm->comm()->selectedIndex())->setSelected(true);
    }
#endif //MOBILE_BUILD
}


// ----------------------------
// Private
// ----------------------------

void SettingsPage::updateConnectionList() {
    ui->connectionList->clear();
    for (QString connectionName : (*mComm->comm()->connectionList().get())) {
        bool itemFound = false;
        for (int i = 0; i < ui->connectionList->count(); i++) {
            QListWidgetItem *item = ui->connectionList->item(i);
            if (!QString::compare(item->text(), connectionName)) {
                itemFound = true;
            }
        }
        if (!itemFound && (QString::compare(QString(""), connectionName))) {
            ui->connectionList->addItem(connectionName);
        }
    }
    ui->lineEdit->setText(mComm->comm()->currentConnection());
}
