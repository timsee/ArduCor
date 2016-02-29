/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "singlecolorpage.h"
#include "ui_singlecolorpage.h"
#include <QColorDialog>
#include <QDebug>


SingleColorPage::SingleColorPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SingleColorPage) {
    ui->setupUi(this);

    QPixmap pixmap = QPixmap(":/images/color_wheel.png");
    ui->colorPicker->setPixmap(pixmap.scaled(175, 175, Qt::KeepAspectRatio));
    mRed = 0;
    mGreen = 255;
    mBlue = 0;

    mSolidData = IconData(ui->solidButton->iconSize().width(), ui->solidButton->iconSize().height());
    mFadeData = IconData(ui->fadeButton->iconSize().width(), ui->fadeButton->iconSize().height());
    mGlimmerData = IconData(ui->glimmerButton->iconSize().width(), ui->glimmerButton->iconSize().height());
    mBlinkData = IconData(ui->blinkButton->iconSize().width(), ui->blinkButton->iconSize().height());

    updateIcons();
    updateColorPreview();

    ui->solidButton->setCheckable(true);
    ui->blinkButton->setCheckable(true);
    ui->fadeButton->setCheckable(true);
    ui->glimmerButton->setCheckable(true);

    ui->sliderR->setSliderColorBackground({255, 0, 0});
    ui->sliderG->setSliderColorBackground({0, 255, 0});
    ui->sliderB->setSliderColorBackground({0, 0, 255});

    ui->sliderR->slider->setValue(mRed);
    ui->sliderG->slider->setValue(mGreen);
    ui->sliderB->slider->setValue(mBlue);


    // set up the sliders
    connect(ui->colorPicker, SIGNAL(colorWheelUpdate(int,int,int)),
            this, SLOT(colorWheelChanged(int,int,int)));
    connect(ui->sliderR, SIGNAL(valueChanged(int)),
            this, SLOT(rChanged(int)));
    connect(ui->sliderG, SIGNAL(valueChanged(int)),
            this, SLOT(gChanged(int)));
    connect(ui->sliderB, SIGNAL(valueChanged(int)),
            this, SLOT(bChanged(int)));
    connect(ui->solidButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToSolid()));
    connect(ui->blinkButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToBlink()));
    connect(ui->fadeButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToFade()));
    connect(ui->glimmerButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToGlimmer()));

}


SingleColorPage::~SingleColorPage() {
    delete ui;
}

void SingleColorPage::setNewColor() {
    LEDs->comm->sendMainColorChange(mRed, mGreen, mBlue);
    if (!(LEDs->data->getCurrentMode() == DataLayer::eLightingModeSingleBlink
            || LEDs->data->getCurrentMode() == DataLayer::eLightingModeSingleConstant
            || LEDs->data->getCurrentMode() == DataLayer::eLightingModeSingleFade
            || LEDs->data->getCurrentMode() == DataLayer::eLightingModeSingleGlimmer)) {
        LEDs->data->setCurrentMode(DataLayer::eLightingModeSingleGlimmer);
        LEDs->comm->sendModeChange(DataLayer::eLightingModeSingleGlimmer);
    }
}

void SingleColorPage::rChanged(int newR) {
    mRed = newR;
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::gChanged(int newG) {
    mGreen = newG;
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::bChanged(int newB) {
    mBlue = newB;
    updateColorPreview();
    setNewColor();
}

void SingleColorPage::updateColorPreview() {
    QColor color;
    color.setRed(mRed);
    color.setGreen(mGreen);
    color.setBlue(mBlue);
    updateIcons();
    // emit the updates to other UI elements
    emit colorUpdated(mRed, mGreen, mBlue);
}


void SingleColorPage::highlightButton(DataLayer::ELightingMode lightingMode) {
    ui->solidButton->setChecked(false);
    ui->blinkButton->setChecked(false);
    ui->fadeButton->setChecked(false);
    ui->glimmerButton->setChecked(false);

    if (lightingMode == DataLayer::eLightingModeSingleConstant) {
        ui->solidButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSingleBlink) {
        ui->blinkButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSingleFade) {
        ui->fadeButton->setChecked(true);
    } else if (lightingMode == DataLayer::eLightingModeSingleGlimmer) {
        ui->glimmerButton->setChecked(true);
    }
}

void SingleColorPage::showEvent(QShowEvent *) {
  mCurrentMode = LEDs->data->getCurrentMode();
  highlightButton(mCurrentMode);
}


void SingleColorPage::updateIcons() {
    mSolidData.setSolidColor(mRed, mGreen, mBlue);
    ui->solidButton->setIcon(mSolidData.renderAsQPixmap());

    mFadeData.setSolidColor(mRed, mGreen, mBlue);
    mFadeData.addFade();
    ui->fadeButton->setIcon(mFadeData.renderAsQPixmap());

    mGlimmerData.setSolidColor(mRed, mGreen, mBlue);
    mGlimmerData.addGlimmer();
    ui->glimmerButton->setIcon(mGlimmerData.renderAsQPixmap());

    mBlinkData.setSolidColor(mRed, mGreen, mBlue);
    mBlinkData.addBlink();
    ui->blinkButton->setIcon(mBlinkData.renderAsQPixmap());
}

void SingleColorPage::changeToOff() {
    mCurrentMode = DataLayer::eLightingModeOff;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}

void SingleColorPage::changeToSolid() {
    mCurrentMode = DataLayer::eLightingModeSingleConstant;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}

void SingleColorPage::changeToBlink() {
    mCurrentMode = DataLayer::eLightingModeSingleBlink;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}

void SingleColorPage::changeToFade() {
    mCurrentMode = DataLayer::eLightingModeSingleFade;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}

void SingleColorPage::changeToGlimmer() {
    mCurrentMode = DataLayer::eLightingModeSingleGlimmer;
    LEDs->data->setCurrentMode(mCurrentMode);
    LEDs->comm->sendModeChange(mCurrentMode);
    highlightButton(mCurrentMode);
}


void SingleColorPage::colorWheelChanged(int r, int g, int b) {
    mRed = r;
    mGreen = g;
    mBlue = b;
    ui->sliderR->slider->setValue(r);
    ui->sliderG->slider->setValue(g);
    ui->sliderB->slider->setValue(b);
    updateColorPreview();
    setNewColor();
}


void SingleColorPage::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    QPixmap pixmap = QPixmap(":/images/color_wheel.png");
    ui->colorPicker->setPixmap(pixmap.scaled(ui->colorPicker->height(), ui->colorPicker->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

