/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
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


void SettingsPage::speedChanged(int newSpeed) {
    mSpeed = newSpeed;
    LEDs->data->setSpeed(mSpeed);
    LEDs->comm->sendSpeed(mSpeed);
}

void SettingsPage::timeoutChanged(int newTimeout) {
    mTimeout = newTimeout;
   LEDs->data->setTimeOut(mSpeed);
   LEDs->comm->sendTimeOut(mTimeout);
}

void SettingsPage::listClicked(QListWidgetItem* item) {
    //qDebug() << item->text() << LEDs->comm->serial->portName();
    // only change serial ports if a new one is clicked
    if (QString::compare(item->text(), LEDs->comm->serial->portName())) {
        // check a mac os x case where it adds a .cu for certain connections
        QString cuCheck = QString("cu.%1").arg(item->text());
        if (!QString::compare(cuCheck, LEDs->comm->serial->portName())) {
            // do nothing
        } else {
            // close the current serial port
            LEDs->comm->closeSerialPort();
            // connect to the selected serial port
            LEDs->comm->connectSerialPort(item->text());
        }
    }
}

void SettingsPage::updateSerialList() {
    // look for updates in the serial port list
    for (QSerialPortInfo port : LEDs->comm->serialList) {
        bool itemFound = false;

        for (int i = 0; i < ui->serialList->count(); i++) {
            QListWidgetItem* item = ui->serialList->item(i);
            if (!QString::compare(item->text(), port.portName())) {
                itemFound = true;
            }

        }
        if (!itemFound) {
            ui->serialList->addItem(port.portName());
        }
    }
}

void SettingsPage::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    // set the sliders to their proper values
    ui->speedSlider->slider->setValue(LEDs->data->getSpeed());
    ui->timeoutSlider->slider->setValue(LEDs->data->getTimeOut());

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
