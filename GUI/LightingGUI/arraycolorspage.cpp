/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "ArrayColorsPage.h"
#include "ui_ArrayColorsPage.h"
#include "icondata.h"

#include <QSignalMapper>

ArrayColorsPage::ArrayColorsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArrayColorsPage) {
    ui->setupUi(this);

    mPageButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(6, nullptr));
    (*mPageButtons.get())[0] = ui->glimmerButton;
    (*mPageButtons.get())[1] = ui->randomIndividualButton;
    (*mPageButtons.get())[2] = ui->randomSolidButton;
    (*mPageButtons.get())[3] = ui->fadeButton;
    (*mPageButtons.get())[4] = ui->barsSolidButton;
    (*mPageButtons.get())[5] = ui->barsMovingButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++)
    {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->glimmerButton, (int)ELightingMode::eLightingModeSavedGlimmer);
    signalMapper->setMapping(ui->randomIndividualButton, (int)ELightingMode::eLightingModeSavedRandomIndividual);
    signalMapper->setMapping(ui->randomSolidButton, (int)ELightingMode::eLightingModeSavedRandomSolid);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingMode::eLightingModeSavedFade);
    signalMapper->setMapping(ui->barsSolidButton, (int)ELightingMode::eLightingModeSavedBarsSolid);
    signalMapper->setMapping(ui->barsMovingButton, (int)ELightingMode::eLightingModeSavedBarsMoving);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));

    connect(ui->arraySlider, SIGNAL(valueChanged(int)), this, SLOT(colorCountChanged(int)));
    ui->arraySlider->setSliderColorBackground({0,255,0});
    ui->arraySlider->slider->setMaximum(50);
    ui->arraySlider->slider->setTickInterval(10);
    ui->arraySlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->arraySlider->setSnapToNearestTick(true);
    ui->arraySlider->setMinimumPossible(true, 20);
}

ArrayColorsPage::~ArrayColorsPage() {
    delete ui;
}


void ArrayColorsPage::highlightButton(ELightingMode lightingMode) {

    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (lightingMode == ELightingMode::eLightingModeSavedGlimmer) {
        ui->glimmerButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSavedRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSavedRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSavedFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSavedBarsSolid) {
        ui->barsSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSavedBarsMoving) {
        ui->barsMovingButton->setChecked(true);
    }
}

// ----------------------------
// Slots
// ----------------------------

void ArrayColorsPage::colorCountChanged(int newCount) {
    int count = newCount / 10;
    ELightingMode mode = mData->currentMode();
    if (count != mData->colorCount()) {
        mData->colorCount(count);
        updateIcons();

        if (mode == ELightingMode::eLightingModeSavedBarsMoving
            || mode == ELightingMode::eLightingModeSavedBarsSolid
            || mode == ELightingMode::eLightingModeSavedFade
            || mode == ELightingMode::eLightingModeSavedGlimmer
            || mode == ELightingMode::eLightingModeSavedRandomIndividual
            || mode == ELightingMode::eLightingModeSavedRandomSolid) {
            mData->currentMode(mode);
            mComm->sendArrayModeChange(mode, count);
        }

    }
}


void ArrayColorsPage::modeChanged(int newMode) {
    mData->currentMode((ELightingMode)newMode);
    mComm->sendArrayModeChange(mData->currentMode(), mData->colorCount());
    highlightButton(mData->currentMode());
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void ArrayColorsPage::showEvent(QShowEvent *event) {
  Q_UNUSED(event);

  highlightButton(mData->currentMode());
  if (ui->arraySlider->slider->value() < 20) {
    ui->arraySlider->slider->setValue(20);
  }
  updateIcons();
}


// ----------------------------
// Private
// ----------------------------

void ArrayColorsPage::updateIcons() {
    IconData buttonData = IconData(ui->glimmerButton->iconSize().width(),
                                   ui->glimmerButton->iconSize().height(),
                                   mData);
    buttonData.setArrayGlimmer();
    ui->glimmerButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->fadeButton->iconSize().width(),
                          ui->fadeButton->iconSize().height(),
                          mData);
    buttonData.setArrayFade();
    ui->fadeButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomSolidButton->iconSize().width(),
                          ui->randomSolidButton->iconSize().height(),
                          mData);
    buttonData.setArrayRandomSolid();
    ui->randomSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomIndividualButton->iconSize().width(),
                          ui->randomIndividualButton->iconSize().height(),
                          mData);
    buttonData.setArrayRandomIndividual();
    ui->randomIndividualButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsSolidButton->iconSize().width(),
                          ui->barsSolidButton->iconSize().height(),
                          mData);
    buttonData.setArrayBarsSolid();
    ui->barsSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsMovingButton->iconSize().width(),
                          ui->barsMovingButton->iconSize().height(),
                          mData);
    buttonData.setArrayBarsMoving();
    ui->barsMovingButton->setIcon(buttonData.renderAsQPixmap());
}

