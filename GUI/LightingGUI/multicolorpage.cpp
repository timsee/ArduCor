/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "multicolorpage.h"
#include "ui_multicolorpage.h"
#include <QDebug>
#include "icondata.h"

MultiColorPage::MultiColorPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiColorPage) {
    ui->setupUi(this);

    // setup the buttons
    ui->randomIndividualButton->setCheckable(true);
    ui->randomSolidButton->setCheckable(true);
    ui->fadeAllButton->setCheckable(true);

    // setup the signals and slots
    connect(ui->randomSolidButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToRandomSolid()));
    connect(ui->randomIndividualButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToRandomIndividual()));
    connect(ui->fadeAllButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToFadeAll()));

    // setup the icons
    IconData iconData = IconData(ui->randomIndividualButton->iconSize().width(),
                                 ui->randomIndividualButton->iconSize().height());
    iconData.setRandomColors();
    ui->randomIndividualButton->setIcon(iconData.renderAsQPixmap());

    iconData = IconData(ui->randomSolidButton->iconSize().width(),
                        ui->randomSolidButton->iconSize().height());
    iconData.setRandomSolid();
    ui->randomSolidButton->setIcon(iconData.renderAsQPixmap());

    iconData = IconData(ui->fadeAllButton->iconSize().width(),
                        ui->fadeAllButton->iconSize().height());
    iconData.setFadeAllColors();
    ui->fadeAllButton->setIcon(iconData.renderAsQPixmap());
}

void MultiColorPage::highlightButton(DataLayer::ELightingMode lightingMode) {
    // set all to false
    ui->randomSolidButton->setChecked(false);
    ui->randomIndividualButton->setChecked(false);
    ui->fadeAllButton->setChecked(false);
    // set the new mode to true
    if (lightingMode == DataLayer::eLightingModeMultiFade) {
        ui->fadeAllButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeMultiRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeMultiRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    }
}


MultiColorPage::~MultiColorPage() {
    delete ui;
}

void MultiColorPage::showEvent(QShowEvent *) {
    mCurrentMode =  LEDs->data->getCurrentMode();
    highlightButton(mCurrentMode);
}

void MultiColorPage::changeToRandomSolid() {
    mCurrentMode = DataLayer::eLightingModeMultiRandomSolid;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void MultiColorPage::changeToRandomIndividual() {
    mCurrentMode = DataLayer::eLightingModeMultiRandomIndividual;
    highlightButton(mCurrentMode);
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    emit updateMainIcons();
}

void MultiColorPage::changeToFadeAll() {
    mCurrentMode = DataLayer::eLightingModeMultiFade;
    highlightButton(mCurrentMode);
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    emit updateMainIcons();
}
