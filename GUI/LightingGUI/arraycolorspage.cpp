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

    // --------------
    // Setup Array Count Slider
    // --------------

    const int COLOR_ARRAY_PRESET = 10;
    connect(ui->arraySlider, SIGNAL(valueChanged(int)), this, SLOT(colorsUsedChanged(int)));
    ui->arraySlider->setSliderColorBackground({0,255,0});
    ui->arraySlider->slider->setMaximum(COLOR_ARRAY_PRESET * 10);
    ui->arraySlider->slider->setTickInterval(10);
    ui->arraySlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->arraySlider->setSnapToNearestTick(true);
    ui->arraySlider->setMinimumPossible(true, 20);

    // --------------
    // Setup Color Array Array (array array  ... array array...)
    // --------------

    mArrayColorsButtons = std::shared_ptr<std::vector<QPushButton*> >(new std::vector<QPushButton*>(COLOR_ARRAY_PRESET, nullptr));
    mArrayColorsIconData = std::shared_ptr<std::vector<IconData> >(new std::vector<IconData>(COLOR_ARRAY_PRESET));
    QSignalMapper *arrayButtonsMapper = new QSignalMapper(this);
    for (int i = 0; i < COLOR_ARRAY_PRESET; ++i) {
        (*mArrayColorsButtons.get())[i] = new QPushButton;
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
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++)
    {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->glimmerButton, (int)ELightingMode::eLightingModeArrayGlimmer);
    signalMapper->setMapping(ui->randomIndividualButton, (int)ELightingMode::eLightingModeArrayRandomIndividual);
    signalMapper->setMapping(ui->randomSolidButton, (int)ELightingMode::eLightingModeArrayRandomSolid);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingMode::eLightingModeArrayFade);
    signalMapper->setMapping(ui->barsSolidButton, (int)ELightingMode::eLightingModeArrayBarsSolid);
    signalMapper->setMapping(ui->barsMovingButton, (int)ELightingMode::eLightingModeArrayBarsMoving);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));
}

ArrayColorsPage::~ArrayColorsPage() {
    delete ui;
}


void ArrayColorsPage::highlightModeButton(ELightingMode lightingMode) {

    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (lightingMode == ELightingMode::eLightingModeArrayGlimmer) {
        ui->glimmerButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeArrayRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeArrayRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeArrayFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeArrayBarsSolid) {
        ui->barsSolidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeArrayBarsMoving) {
        ui->barsMovingButton->setChecked(true);
    }
}

void ArrayColorsPage::selectArrayColor(int index) {
    mCurrentColorPickerIndex = index;
    ui->colorPicker->chooseColor(mData->colors[mCurrentColorPickerIndex], false);
    updateColorArray();
    for (int i = 0; i < mData->colorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setChecked(false);
        (*mArrayColorsButtons.get())[i]->setStyleSheet("border:none");
    }

    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setChecked(true);
    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setStyleSheet("border: 2px solid white");
}

void ArrayColorsPage::updateColorArray() {
    for (int i = 0; i < mData->colorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon((*mArrayColorsIconData.get())[i].renderAsQPixmap());
    }

    for (int i = mData->colorsUsed(); i < mData->colorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }
     ui->arraySlider->setSliderColorBackground(mData->colorsAverage());
}


// ----------------------------
// Slots
// ----------------------------

void ArrayColorsPage::colorsUsedChanged(int newColorsUsed) {
    int totalColorsUsed = newColorsUsed / 10;
    ELightingMode mode = mData->currentMode();
    if (totalColorsUsed != mData->colorsUsed()) {
        mData->colorsUsed(totalColorsUsed);
        updateColorArray();
        updateIcons();
        if (mode == ELightingMode::eLightingModeArrayBarsMoving
            || mode == ELightingMode::eLightingModeArrayBarsSolid
            || mode == ELightingMode::eLightingModeArrayFade
            || mode == ELightingMode::eLightingModeArrayGlimmer
            || mode == ELightingMode::eLightingModeArrayRandomIndividual
            || mode == ELightingMode::eLightingModeArrayRandomSolid) {
            mData->currentMode(mode);
            mComm->sendArrayModeChange(mode, totalColorsUsed);
        }

    }
}


void ArrayColorsPage::modeChanged(int newMode) {
    mData->currentMode((ELightingMode)newMode);
    mComm->sendArrayModeChange(mData->currentMode(), mData->colorsUsed());
    highlightModeButton(mData->currentMode());
    emit updateMainIcons();
}

void ArrayColorsPage::colorChanged(QColor color) {
    mData->colors[mCurrentColorPickerIndex] = color;

    mComm->sendArrayColorChange(mCurrentColorPickerIndex, color);
    ui->arraySlider->setSliderColorBackground(mData->colorsAverage());

    updateIcons();
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void ArrayColorsPage::showEvent(QShowEvent *event) {
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

void ArrayColorsPage::updateIcons() {

    for (int i = 0; i < mData->colorsUsed(); ++i) {
        (*mArrayColorsIconData.get())[i].setSolidColor(mData->colors[i]);
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon((*mArrayColorsIconData.get())[i].renderAsQPixmap());
    }

    for (int i = mData->colorsUsed(); i < mData->colorCount(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }

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

