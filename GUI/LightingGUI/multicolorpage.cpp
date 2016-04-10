/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "multicolorpage.h"
#include "ui_multicolorpage.h"
#include <QDebug>
#include <QSignalMapper>
#include "icondata.h"

MultiColorPage::MultiColorPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiColorPage) {
    ui->setupUi(this);

    mPageButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(3, nullptr));
    (*mPageButtons.get())[0] = ui->randomSolidButton;
    (*mPageButtons.get())[1] = ui->randomIndividualButton;
    (*mPageButtons.get())[2] = ui->fadeAllButton;

    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->randomSolidButton, (int)ELightingMode::eLightingModeMultiRandomSolid);
    signalMapper->setMapping(ui->randomIndividualButton, (int)ELightingMode::eLightingModeMultiRandomIndividual);
    signalMapper->setMapping(ui->fadeAllButton, (int)ELightingMode::eLightingModeMultiFade);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));

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

MultiColorPage::~MultiColorPage() {
    delete ui;
}

void MultiColorPage::highlightModeButton(ELightingMode lightingMode) {
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    // set the new mode to true
    if (lightingMode == ELightingMode::eLightingModeMultiFade) {
        ui->fadeAllButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeMultiRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeMultiRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    }
}


// ----------------------------
// Slots
// ----------------------------

void MultiColorPage::modeChanged(int newMode) {
    mData->currentMode((ELightingMode)newMode);
    mComm->sendModeChange(mData->currentMode());
    highlightModeButton(mData->currentMode());
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void MultiColorPage::showEvent(QShowEvent *) {
    highlightModeButton(mData->currentMode());
}

