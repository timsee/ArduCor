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


    QPixmap pixmap = QPixmap(":/images/color_wheel.png");
    ui->colorPicker->setPixmap(pixmap.scaled(175, 175, Qt::KeepAspectRatio));

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

    signalMapper->setMapping(ui->solidButton, (int)ELightingMode::eLightingModeSingleConstant);
    signalMapper->setMapping(ui->blinkButton, (int)ELightingMode::eLightingModeSingleBlink);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingMode::eLightingModeSingleFade);
    signalMapper->setMapping(ui->glimmerButton, (int)ELightingMode::eLightingModeSingleGlimmer);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));

    // set up the sliders
    ui->sliderR->setSliderColorBackground({255, 0, 0});
    ui->sliderG->setSliderColorBackground({0, 255, 0});
    ui->sliderB->setSliderColorBackground({0, 0, 255});

    ui->sliderR->slider->setRange(0, 255);
    ui->sliderG->slider->setRange(0, 255);
    ui->sliderB->slider->setRange(0, 255);

    connect(ui->colorPicker, SIGNAL(colorWheelUpdate(QColor)),
            this, SLOT(colorWheelChanged(QColor)));
    connect(ui->sliderR, SIGNAL(valueChanged(int)),
            this, SLOT(rChanged(int)));
    connect(ui->sliderG, SIGNAL(valueChanged(int)),
            this, SLOT(gChanged(int)));
    connect(ui->sliderB, SIGNAL(valueChanged(int)),
            this, SLOT(bChanged(int)));
}


SingleColorPage::~SingleColorPage() {
    delete ui;
}


void SingleColorPage::updateColorPreview() {
    updateIcons();
    // emit the updates to other UI elements
    emit updateMainIcons();
}


void SingleColorPage::highlightButton(ELightingMode lightingMode) {
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (lightingMode == ELightingMode::eLightingModeSingleConstant) {
        ui->solidButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSingleBlink) {
        ui->blinkButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSingleFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == ELightingMode::eLightingModeSingleGlimmer) {
        ui->glimmerButton->setChecked(true);
    }
}

// ----------------------------
// Slots
// ----------------------------

void SingleColorPage::setNewColor() {
    mComm->sendMainColorChange(mData->color().red(), mData->color().green(), mData->color().blue());
    if (!(mData->currentMode() == ELightingMode::eLightingModeSingleBlink
            || mData->currentMode() == ELightingMode::eLightingModeSingleConstant
            || mData->currentMode() == ELightingMode::eLightingModeSingleFade
            || mData->currentMode() == ELightingMode::eLightingModeSingleGlimmer)) {
        mData->currentMode(ELightingMode::eLightingModeSingleGlimmer);
        mComm->sendModeChange(ELightingMode::eLightingModeSingleGlimmer);
    }
}

void SingleColorPage::showEvent(QShowEvent *) {
  mCurrentMode = mData->currentMode();
  highlightButton(mCurrentMode);
}


void SingleColorPage::modeChanged(int newMode) {
    mCurrentMode = (ELightingMode)newMode;
    mData->currentMode(mCurrentMode);
    mComm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}


void SingleColorPage::colorWheelChanged(QColor color) {
    mData->color(color);
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::rChanged(int newR) {
    mData->color(QColor(newR, mData->color().green(), mData->color().blue()));
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::gChanged(int newG) {
    mData->color(QColor(mData->color().red(), newG, mData->color().blue()));
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::bChanged(int newB) {
    mData->color(QColor(mData->color().red(), mData->color().green(), newB));
    updateColorPreview();
    setNewColor();
}


// ----------------------------
// Protected
// ----------------------------


void SingleColorPage::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    QPixmap pixmap = QPixmap(":/images/color_wheel.png");
    ui->colorPicker->setPixmap(pixmap.scaled(ui->colorPicker->height(), ui->colorPicker->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// ----------------------------
// Private
// ----------------------------

void SingleColorPage::updateIcons() {
    QColor color(mData->color());

    ui->sliderR->slider->blockSignals(true);
    ui->sliderR->slider->setValue(color.red());
    ui->sliderR->slider->blockSignals(false);

    ui->sliderG->slider->blockSignals(true);
    ui->sliderG->slider->setValue(color.green());
    ui->sliderG->slider->blockSignals(false);

    ui->sliderB->slider->blockSignals(true);
    ui->sliderB->slider->setValue(color.blue());
    ui->sliderB->slider->blockSignals(true);

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
}

