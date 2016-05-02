/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "CustomArrayPage.h"
#include "ui_CustomArrayPage.h"
#include "icondata.h"

#include <QSignalMapper>

CustomArrayPage::CustomArrayPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomArrayPage) {
    ui->setupUi(this);

    // --------------
    // Setup Array Count Slider
    // --------------

    const int COLOR_ARRAY_PRESET = 10;
    connect(ui->arraySlider, SIGNAL(valueChanged(int)), this, SLOT(colorsUsedChanged(int)));
    ui->arraySlider->setSliderColorBackground(QColor(0,255,0));
    ui->arraySlider->slider->setMaximum(COLOR_ARRAY_PRESET * 10);
    ui->arraySlider->slider->setTickInterval(10);
    ui->arraySlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->arraySlider->setSnapToNearestTick(true);
    ui->arraySlider->setMinimumPossible(true, 20);


    // --------------
    // Setup Color Array Array (array array  ... array array...)
    // --------------

    mArrayColorsButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(COLOR_ARRAY_PRESET, nullptr));
    mArrayColorsIconData = std::shared_ptr<std::vector<IconData> >(new std::vector<IconData>(COLOR_ARRAY_PRESET));
    QSignalMapper *arrayButtonsMapper = new QSignalMapper(this);
    for (int i = 0; i < COLOR_ARRAY_PRESET; ++i) {
        (*mArrayColorsButtons.get())[i] = new QToolButton;
        (*mArrayColorsIconData.get())[i] = IconData(80,80);
        (*mArrayColorsButtons.get())[i]->setIcon((*mArrayColorsIconData.get())[i].renderAsQPixmap());
        ui->arrayColorsLayout->addWidget((*mArrayColorsButtons.get())[i], 0 , i);
        connect((*mArrayColorsButtons.get())[i], SIGNAL(clicked(bool)), arrayButtonsMapper, SLOT(map()));
        arrayButtonsMapper->setMapping((*mArrayColorsButtons.get())[i], i);
    }
    connect(arrayButtonsMapper, SIGNAL(mapped(int)), this, SLOT(selectArrayColor(int)));

    mGreyIcon = IconData(80,80);
    mGreyIcon.setSolidColor(QColor(140,140,140));

    // --------------
    // Setup Color Picker
    // --------------

    ui->colorPicker->chooseLayout(ELayoutColorPicker::eCondensedLayout);
    connect(ui->colorPicker, SIGNAL(colorUpdate(QColor)), this, SLOT(colorChanged(QColor)));
    mCurrentColorPickerIndex = 0;

    // --------------
    // Setup Mode Buttons
    // --------------

    mPageButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(6, nullptr));
    (*mPageButtons.get())[0] = ui->glimmerButton;
    (*mPageButtons.get())[1] = ui->randomIndividualButton;
    (*mPageButtons.get())[2] = ui->randomSolidButton;
    (*mPageButtons.get())[3] = ui->fadeButton;
    (*mPageButtons.get())[4] = ui->barsSolidButton;
    (*mPageButtons.get())[5] = ui->barsMovingButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->glimmerButton, (int)ELightingMode::eMultiGlimmer);
    signalMapper->setMapping(ui->randomIndividualButton, (int)ELightingMode::eMultiRandomIndividual);
    signalMapper->setMapping(ui->randomSolidButton, (int)ELightingMode::eMultiRandomSolid);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingMode::eMultiFade);
    signalMapper->setMapping(ui->barsSolidButton, (int)ELightingMode::eMultiBarsSolid);
    signalMapper->setMapping(ui->barsMovingButton, (int)ELightingMode::eMultiBarsMoving);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));
}

CustomArrayPage::~CustomArrayPage() {
    delete ui;
}


void CustomArrayPage::highlightModeButton(ELightingMode lightingMode) {

    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (lightingMode == ELightingMode::eMultiGlimmer) {
        ui->glimmerButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eMultiRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eMultiRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eMultiFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eMultiBarsSolid) {
        ui->barsSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eMultiBarsMoving) {
        ui->barsMovingButton->setChecked(true);
    }
}

void CustomArrayPage::selectArrayColor(int index) {
    mCurrentColorPickerIndex = index;
    ui->colorPicker->chooseColor(mData->customArray()[mCurrentColorPickerIndex], false);
    updateColorArray();
    for (int i = 0; i < mData->customColorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setChecked(false);
        (*mArrayColorsButtons.get())[i]->setStyleSheet("border:none");
    }

    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setChecked(true);
    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setStyleSheet("border: 2px solid white");
}

void CustomArrayPage::updateColorArray() {
    for (int i = 0; i < mData->customColorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon((*mArrayColorsIconData.get())[i].renderAsQPixmap());
    }

    for (int i = mData->colorsUsed(); i < mData->customColorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }
     ui->arraySlider->setSliderColorBackground(mData->colorsAverage());
}


// ----------------------------
// Slots
// ----------------------------

void CustomArrayPage::colorsUsedChanged(int newColorsUsed) {
    int totalColorsUsed = newColorsUsed / 10;
    ELightingMode mode = mData->currentMode();
    if (totalColorsUsed != mData->colorsUsed()) {
        mData->colorsUsed(totalColorsUsed);
        updateColorArray();
        updateIcons();
        mComm->sendCustomArrayCount(mData->colorsUsed());
    }
}


void CustomArrayPage::modeChanged(int newMode) {
    mData->currentMode((ELightingMode)newMode);
    mComm->sendArrayModeChange(mData->currentMode());
    highlightModeButton(mData->currentMode());
    emit updateMainIcons();
}

void CustomArrayPage::colorChanged(QColor color) {
    mData->customArray()[mCurrentColorPickerIndex] = color;

    mComm->sendArrayColorChange(mCurrentColorPickerIndex, color);
    ui->arraySlider->setSliderColorBackground(mData->colorsAverage());

    updateIcons();
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void CustomArrayPage::showEvent(QShowEvent *event) {
  Q_UNUSED(event);
  // update all the mode icons to the current colors.
  updateIcons();

  // highlight the current mode, if its in this page
  highlightModeButton(mData->currentMode());
  // fix an edge case
  // TODO: revmoe this edge case
  if (ui->arraySlider->slider->value() < 20) {
    ui->arraySlider->slider->setValue(20);
  }

  selectArrayColor(mCurrentColorPickerIndex);
}

// ----------------------------
// Private
// ----------------------------

void CustomArrayPage::updateIcons() {

    for (int i = 0; i < mData->colorsUsed(); ++i) {
        (*mArrayColorsIconData.get())[i].setSolidColor(mData->customArray()[i]);
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon((*mArrayColorsIconData.get())[i].renderAsQPixmap());
    }

    for (int i = mData->colorsUsed(); i < mData->customColorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }

    IconData buttonData = IconData(ui->glimmerButton->iconSize().width(),
                                   ui->glimmerButton->iconSize().height(),
                                   mData);
    buttonData.setArrayGlimmer(EColorPreset::eCustom);
    ui->glimmerButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->fadeButton->iconSize().width(),
                          ui->fadeButton->iconSize().height(),
                          mData);
    buttonData.setArrayFade(EColorPreset::eCustom);
    ui->fadeButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomSolidButton->iconSize().width(),
                          ui->randomSolidButton->iconSize().height(),
                          mData);
    buttonData.setArrayRandomSolid(EColorPreset::eCustom);
    ui->randomSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomIndividualButton->iconSize().width(),
                          ui->randomIndividualButton->iconSize().height(),
                          mData);
    buttonData.setArrayRandomIndividual(EColorPreset::eCustom);
    ui->randomIndividualButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsSolidButton->iconSize().width(),
                          ui->barsSolidButton->iconSize().height(),
                          mData);
    buttonData.setArrayBarsSolid(EColorPreset::eCustom);
    ui->barsSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsMovingButton->iconSize().width(),
                          ui->barsMovingButton->iconSize().height(),
                          mData);
    buttonData.setArrayBarsMoving(EColorPreset::eCustom);
    ui->barsMovingButton->setIcon(buttonData.renderAsQPixmap());
}

