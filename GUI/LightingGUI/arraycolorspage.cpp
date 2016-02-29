/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "ArrayColorsPage.h"
#include "ui_ArrayColorsPage.h"
#include "icondata.h"

ArrayColorsPage::ArrayColorsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ArrayColorsPage) {
    ui->setupUi(this);

    // set up the menu buttons to change the widgets
    connect(ui->glimmerButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToGlimmer()));
    connect(ui->randomIndividualButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToRandomIndividual()));
    connect(ui->randomSolidButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToRandomSolid()));
    connect(ui->fadeButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToFade()));
    connect(ui->barsSolidButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToBarsSolid()));
    connect(ui->barsMovingButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToBarsMoving()));

    ui->glimmerButton->setCheckable(true);
    ui->randomIndividualButton->setCheckable(true);
    ui->randomSolidButton->setCheckable(true);
    ui->fadeButton->setCheckable(true);
    ui->barsSolidButton->setCheckable(true);
    ui->barsMovingButton->setCheckable(true);

    connect(ui->arraySlider, SIGNAL(valueChanged(int)),
            this, SLOT(colorCountChanged(int)));
    ui->arraySlider->setSliderColorBackground({0,255,0});
    ui->arraySlider->slider->setMaximum(50);
    ui->arraySlider->slider->setTickInterval(10);
    ui->arraySlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->arraySlider->setSnapToNearestTick(true);
    ui->arraySlider->setMinimumPossible(true, 20);
}


void ArrayColorsPage::updateIcons() {
    IconData buttonData = IconData(ui->glimmerButton->iconSize().width(),
                                   ui->glimmerButton->iconSize().height(),
                                   LEDs->data);
    buttonData.setArrayGlimmer();
    ui->glimmerButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->fadeButton->iconSize().width(),
                          ui->fadeButton->iconSize().height(),
                          LEDs->data);
    buttonData.setArrayFade();
    ui->fadeButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomSolidButton->iconSize().width(),
                          ui->randomSolidButton->iconSize().height(),
                          LEDs->data);
    buttonData.setArrayRandomSolid();
    ui->randomSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->randomIndividualButton->iconSize().width(),
                          ui->randomIndividualButton->iconSize().height(),
                          LEDs->data);
    buttonData.setArrayRandomIndividual();
    ui->randomIndividualButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsSolidButton->iconSize().width(),
                          ui->barsSolidButton->iconSize().height(),
                          LEDs->data);
    buttonData.setArrayBarsSolid();
    ui->barsSolidButton->setIcon(buttonData.renderAsQPixmap());

    buttonData = IconData(ui->barsMovingButton->iconSize().width(),
                          ui->barsMovingButton->iconSize().height(),
                          LEDs->data);
    buttonData.setArrayBarsMoving();
    ui->barsMovingButton->setIcon(buttonData.renderAsQPixmap());
}


ArrayColorsPage::~ArrayColorsPage() {
    delete ui;
}

void ArrayColorsPage::highlightButton(DataLayer::ELightingMode lightingMode) {
    ui->glimmerButton->setChecked(false);
    ui->randomIndividualButton->setChecked(false);
    ui->randomSolidButton->setChecked(false);
    ui->fadeButton->setChecked(false);
    ui->barsSolidButton->setChecked(false);
    ui->barsMovingButton->setChecked(false);

    if (lightingMode == DataLayer::eLightingModeSavedGlimmer) {
        ui->glimmerButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSavedRandomIndividual) {
        ui->randomIndividualButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSavedRandomSolid) {
        ui->randomSolidButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSavedFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSavedBarsSolid) {
        ui->barsSolidButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSavedBarsMoving) {
        ui->barsMovingButton->setChecked(true);
    }
}


void ArrayColorsPage::showEvent(QShowEvent *event) {
  Q_UNUSED(event);

  mCurrentMode = LEDs->data->getCurrentMode();
  highlightButton(mCurrentMode);
  if (ui->arraySlider->slider->value() < 20) {
    ui->arraySlider->slider->setValue(20);
  }
  updateIcons();
}


void ArrayColorsPage::colorCountChanged(int newCount) {
    int count = newCount / 10;
    DataLayer::ELightingMode mode = LEDs->data->getCurrentMode();
    if (count != LEDs->data->getColorCount()) {
        LEDs->data->setColorCount(count);
        updateIcons();

        if (mode == DataLayer::ELightingMode::eLightingModeSavedBarsMoving
            || mode == DataLayer::ELightingMode::eLightingModeSavedBarsSolid
            || mode == DataLayer::ELightingMode::eLightingModeSavedFade
            || mode == DataLayer::ELightingMode::eLightingModeSavedGlimmer
            || mode == DataLayer::ELightingMode::eLightingModeSavedRandomIndividual
            || mode == DataLayer::ELightingMode::eLightingModeSavedRandomSolid) {
            LEDs->data->setCurrentMode(mode);
            LEDs->comm->sendArrayModeChange(mode, count);
        }

    }
}


void ArrayColorsPage::changeToGlimmer() {
    mCurrentMode = DataLayer::eLightingModeSavedGlimmer;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void ArrayColorsPage::changeToRandomIndividual() {
    mCurrentMode = DataLayer::eLightingModeSavedRandomIndividual;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void ArrayColorsPage::changeToRandomSolid() {
    mCurrentMode = DataLayer::eLightingModeSavedRandomSolid;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void ArrayColorsPage::changeToFade() {
    mCurrentMode = DataLayer::eLightingModeSavedFade;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void ArrayColorsPage::changeToBarsSolid() {
    mCurrentMode = DataLayer::eLightingModeSavedBarsSolid;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

void ArrayColorsPage::changeToBarsMoving() {
    mCurrentMode = DataLayer::eLightingModeSavedBarsMoving;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendArrayModeChange(mCurrentMode, LEDs->data->getColorCount());
    highlightButton(mCurrentMode);
    emit updateMainIcons();
}

