/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "singlecolorpage.h"
#include "ui_singlecolorpage.h"
#include <QColorDialog>
#include <QDebug>
#include <QSignalMapper>

SingleColorPage::SingleColorPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleColorPage) {
    ui->setupUi(this);

    mSolidData = IconData(ui->solidButton->iconSize().width(), ui->solidButton->iconSize().height());
    mFadeData = IconData(ui->fadeButton->iconSize().width(), ui->fadeButton->iconSize().height());
    mGlimmerData = IconData(ui->glimmerButton->iconSize().width(), ui->glimmerButton->iconSize().height());
    mBlinkData = IconData(ui->blinkButton->iconSize().width(), ui->blinkButton->iconSize().height());

    mPageButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(4, nullptr));
    (*mPageButtons.get())[0] = ui->solidButton;
    (*mPageButtons.get())[1] = ui->blinkButton;
    (*mPageButtons.get())[2] = ui->fadeButton;
    (*mPageButtons.get())[3] = ui->glimmerButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->solidButton, (int)ELightingMode::eSingleSolid);
    signalMapper->setMapping(ui->blinkButton, (int)ELightingMode::eSingleBlink);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingMode::eSingleFade);
    signalMapper->setMapping(ui->glimmerButton, (int)ELightingMode::eSingleGlimmer);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));

    ui->colorPicker->chooseLayout(ELayoutColorPicker::eFullLayout);
    connect(ui->colorPicker, SIGNAL(colorUpdate(QColor)), this, SLOT(colorChanged(QColor)));
}


SingleColorPage::~SingleColorPage() {
    delete ui;
}

void SingleColorPage::chooseColor(QColor color) {
  ui->colorPicker->chooseColor(color);
}

void SingleColorPage::highlightModeButton(ELightingMode lightingMode) {
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (lightingMode == ELightingMode::eSingleSolid) {
        ui->solidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eSingleBlink) {
        ui->blinkButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eSingleFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eSingleGlimmer) {
        ui->glimmerButton->setChecked(true);
    }
}

// ----------------------------
// Slots
// ----------------------------

void SingleColorPage::modeChanged(int newMode) {
    mData->currentMode((ELightingMode)newMode);
    mComm->sendModeChange((ELightingMode)newMode);
    highlightModeButton((ELightingMode)newMode);
}

void SingleColorPage::colorChanged(QColor color) {
    mData->mainColor(color);

    mComm->sendMainColorChange(mData->mainColor());
    //TODO: is this right?
    if (!(mData->currentMode() == ELightingMode::eSingleBlink
            || mData->currentMode() == ELightingMode::eSingleSolid
            || mData->currentMode() == ELightingMode::eSingleFade
            || mData->currentMode() == ELightingMode::eSingleGlimmer)) {
        mData->currentMode(ELightingMode::eSingleGlimmer);
        mComm->sendModeChange(ELightingMode::eSingleGlimmer);
    }

    mSolidData.setSolidColor(color);
    ui->solidButton->setIcon(mSolidData.renderAsQPixmap());

    mFadeData.setSolidColor(color);
    mFadeData.addFade();
    ui->fadeButton->setIcon(mFadeData.renderAsQPixmap());

    mGlimmerData.setSolidColor(color);
    mGlimmerData.addGlimmer();
    ui->glimmerButton->setIcon(mGlimmerData.renderAsQPixmap());

    mBlinkData.setSolidColor(color);
    mBlinkData.addBlink();
    ui->blinkButton->setIcon(mBlinkData.renderAsQPixmap());

    emit updateMainIcons();
}

// ----------------------------
// Protected
// ----------------------------


void SingleColorPage::showEvent(QShowEvent *) {
  highlightModeButton(mData->currentMode());
}

