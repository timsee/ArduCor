/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "CustomColorsPage.h"
#include "ui_CustomColorsPage.h"
#include "icondata.h"

#include <QSignalMapper>

CustomColorsPage::CustomColorsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomColorsPage) {
    ui->setupUi(this);

    // --------------
    // Setup Array Count Slider
    // --------------

    const int COLOR_ARRAY_PRESET = 10;
    connect(ui->arraySlider, SIGNAL(valueChanged(int)), this, SLOT(customColorCountChanged(int)));
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
    mIconData = IconData(80,80);

    QSignalMapper *arrayButtonsMapper = new QSignalMapper(this);
    for (int i = 0; i < COLOR_ARRAY_PRESET; ++i) {
        (*mArrayColorsButtons.get())[i] = new QToolButton;
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
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
    // Setup Routine Buttons
    // --------------

    mRoutineButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(6, nullptr));
    (*mRoutineButtons.get())[0] = ui->glimmerButton;
    (*mRoutineButtons.get())[1] = ui->randomIndividualButton;
    (*mRoutineButtons.get())[2] = ui->randomSolidButton;
    (*mRoutineButtons.get())[3] = ui->fadeButton;
    (*mRoutineButtons.get())[4] = ui->barsSolidButton;
    (*mRoutineButtons.get())[5] = ui->barsMovingButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mRoutineButtons->size(); i++) {
        QToolButton* button =(*mRoutineButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->glimmerButton, (int)ELightingRoutine::eMultiGlimmer);
    signalMapper->setMapping(ui->randomIndividualButton, (int)ELightingRoutine::eMultiRandomIndividual);
    signalMapper->setMapping(ui->randomSolidButton, (int)ELightingRoutine::eMultiRandomSolid);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingRoutine::eMultiFade);
    signalMapper->setMapping(ui->barsSolidButton, (int)ELightingRoutine::eMultiBarsSolid);
    signalMapper->setMapping(ui->barsMovingButton, (int)ELightingRoutine::eMultiBarsMoving);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));
}

CustomColorsPage::~CustomColorsPage() {
    delete ui;
}


void CustomColorsPage::highlightRoutineButton(ELightingRoutine routine) {

    for (uint i = 0; i < mRoutineButtons->size(); i++) {
        QToolButton* button =(*mRoutineButtons.get())[i];
        button->setChecked(false);
    }
    if (mData->currentColorGroup() == EColorGroup::eCustom) {
        for (int i = (int)ELightingRoutine::eSingleGlimmer + 1; i < (int)ELightingRoutine::eLightingRoutine_MAX; i++) {
            mIconData.setLightingRoutine((ELightingRoutine)i, EColorGroup::eCustom);
            (*mRoutineButtons.get())[i - (int)ELightingRoutine::eSingleGlimmer - 1]->setIcon(mIconData.renderAsQPixmap());
        }
    }
}

void CustomColorsPage::selectArrayColor(int index) {
    mCurrentColorPickerIndex = index;
    ui->colorPicker->chooseColor(mData->colorGroup(EColorGroup::eCustom)[mCurrentColorPickerIndex], false);
    updateColorArray();
    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        (*mArrayColorsButtons.get())[i]->setChecked(false);
        (*mArrayColorsButtons.get())[i]->setStyleSheet("border:none");
    }

    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setChecked(true);
    (*mArrayColorsButtons.get())[mCurrentColorPickerIndex]->setStyleSheet("border: 2px solid white");
}

void CustomColorsPage::updateColorArray() {
    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        mIconData.setSolidColor(mData->colorGroup(EColorGroup::eCustom)[i]);
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
    }

    for (int i = mData->groupSize(EColorGroup::eCustom); i < mData->maxColorGroupSize(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }
     ui->arraySlider->setSliderColorBackground(mData->colorsAverage(EColorGroup::eCustom));
}


// ----------------------------
// Slots
// ----------------------------

void CustomColorsPage::customColorCountChanged(int newCount) {
    int colorCount = newCount / 10;
    if (colorCount <= mData->groupSize(EColorGroup::eCustom)) {
        mData->customColorsUsed(colorCount);
        updateColorArray();
        updateIcons();
        mComm->sendCustomArrayCount(mData->groupSize(EColorGroup::eCustom));
        emit updateMainIcons();
    }
}


void CustomColorsPage::modeChanged(int newMode) {
    mData->currentRoutine((ELightingRoutine)newMode);
    mData->currentColorGroup(EColorGroup::eCustom);
    mComm->sendRoutineChange(mData->currentRoutine(), (int)EColorGroup::eCustom);
    highlightRoutineButton(mData->currentRoutine());
    emit updateMainIcons();
}

void CustomColorsPage::colorChanged(QColor color) {
    mData->colorGroup(EColorGroup::eCustom)[mCurrentColorPickerIndex] = color;

    mComm->sendArrayColorChange(mCurrentColorPickerIndex, color);
    ui->arraySlider->setSliderColorBackground(mData->colorsAverage(EColorGroup::eCustom));

    updateIcons();
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void CustomColorsPage::showEvent(QShowEvent *event) {
  Q_UNUSED(event);
  // update all the mode icons to the current colors.
  updateIcons();

  // highlight the current mode, if its in this page
  highlightRoutineButton(mData->currentRoutine());

  // slider can't be set until mData is set up,
  // TODO: remove this dependency. root cause is probably
  // a signal sent out down the line.
  if (ui->arraySlider->slider->value() < 20) {
    ui->arraySlider->slider->setValue(20);
  }

  selectArrayColor(mCurrentColorPickerIndex);
}

// ----------------------------
// Private
// ----------------------------

void CustomColorsPage::updateIcons() {

    for (int i = 0; i < mData->groupSize(EColorGroup::eCustom); ++i) {
        mIconData.setSolidColor(mData->colorGroup(EColorGroup::eCustom)[i]);
        (*mArrayColorsButtons.get())[i]->setEnabled(true);
        (*mArrayColorsButtons.get())[i]->setIcon(mIconData.renderAsQPixmap());
    }

    for (int i = mData->groupSize(EColorGroup::eCustom); i < mData->maxColorGroupSize(); ++i) {
        (*mArrayColorsButtons.get())[i]->setIcon(mGreyIcon.renderAsQPixmap());
        (*mArrayColorsButtons.get())[i]->setEnabled(false);
    }

    mIconData = IconData(80, 80, mData);
    for (int i = (int)ELightingRoutine::eSingleGlimmer + 1; i < (int)ELightingRoutine::eLightingRoutine_MAX; i++) {
        mIconData.setLightingRoutine((ELightingRoutine)i, EColorGroup::eCustom);
        (*mRoutineButtons.get())[i - (int)ELightingRoutine::eSingleGlimmer - 1]->setIcon(mIconData.renderAsQPixmap());
    }
}

