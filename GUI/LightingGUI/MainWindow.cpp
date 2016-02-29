/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include <QDebug>
#include <QPainter>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // setup the LED control backend
    LEDs = new LightsControl();
    mIsOn = true;

    // connect the LightsControl system to all the UI pages
    ui->singleColorPage->LEDs = LEDs;
    ui->multiColorPage->LEDs = LEDs;
    ui->arrayColorsPage->LEDs = LEDs;
    ui->settingsPage->LEDs = LEDs;

    // setup the menu buttons to change the widgets
    connect(ui->singleColorButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToSingleColor()));
    connect(ui->multiColorButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToMultiColor()));
    connect(ui->arrayColorsButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToSavedColors()));
    connect(ui->settingsButton, SIGNAL(clicked(bool)),
            this, SLOT(changeToSettings()));    
    connect(ui->onOffButton, SIGNAL(clicked(bool)),
            this, SLOT(toggleOnOff()));
    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)),
            this, SLOT(brightnessChanged(int)));
    connect(ui->singleColorPage, SIGNAL(colorUpdated(int,int,int)),
            this, SLOT(updateSingleColor(int,int,int)));

    // update the top menu when lighting modes change
    connect(ui->multiColorPage, SIGNAL(updateMainIcons()),
            this, SLOT(updateToMultiColors()));
    connect(ui->arrayColorsPage, SIGNAL(updateMainIcons()),
            this, SLOT(updateToArrayColors()));

    // setup the slider that controls the LED's brightness
    ui->brightnessSlider->slider->setRange(0,100);
    ui->brightnessSlider->slider->setValue(50);
    ui->brightnessSlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->brightnessSlider->slider->setTickInterval(20);
    ui->brightnessSlider->setSliderColorBackground({255,255,255});

    // setup the buttons
    ui->singleColorButton->setCheckable(true);
    ui->multiColorButton->setCheckable(true);
    ui->arrayColorsButton->setCheckable(true);
    ui->settingsButton->setCheckable(true);

    // setup the icons
    onButtonData = IconData(64, 64, LEDs->data);
    onButtonData.setSolidColor(0,255,0);
    ui->onOffButton->setIcon(onButtonData.renderAsQPixmap());
    offButtonData = IconData(64, 64);
    offButtonData.setSolidColor(0,0,0);

    IconData multiData = IconData(64, 64, LEDs->data);
    multiData.setRandomColors();
    ui->multiColorButton->setIcon(multiData.renderAsQPixmap());

    singleButtonData = IconData(64, 64, LEDs->data);
    singleButtonData.setSolidColor(0,255,0);
    ui->singleColorButton->setIcon(singleButtonData.renderAsQPixmap());

    arrayButtonData = IconData(64, 64, LEDs->data);
    arrayButtonData.setArrayColors();
    ui->arrayColorsButton->setIcon(arrayButtonData.renderAsQPixmap());

    // default the window to the single color page
    changeToSingleColor();
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::updateSingleColor(int r, int g, int b) {
    onButtonData.setSolidColor(r,g,b);
    ui->onOffButton->setIcon(onButtonData.renderAsQPixmap());
    ui->singleColorButton->setIcon(onButtonData.renderAsQPixmap());
    mIsOn = true;
}

void MainWindow::updateToArrayColors() {
    onButtonData.setSolidColor(0,0,0);
    onButtonData.setArrayFade();
    ui->onOffButton->setIcon(onButtonData.renderAsQPixmap());
    mIsOn = true;
}

void MainWindow::updateToMultiColors() {
    onButtonData.setSolidColor(0,0,0);
    onButtonData.setRandomColors();
    ui->onOffButton->setIcon(onButtonData.renderAsQPixmap());
    mIsOn = true;
}

void MainWindow::brightnessChanged(int newBrightness) {
   mBrightness = newBrightness;
   LEDs->data->setBrightness(newBrightness);
   LEDs->comm->sendBrightness(newBrightness);
   mIsOn = true;
}


void MainWindow::toggleOnOff() {
    if (mIsOn) {
        ui->onOffButton->setIcon(offButtonData.renderAsQPixmap());

        mIsOn = false;
        LEDs->comm->sendBrightness(0);
    } else {
        ui->onOffButton->setIcon(onButtonData.renderAsQPixmap());
        //qDebug() << LEDs->data->getBrightness();
        mIsOn = true;
        LEDs->comm->sendBrightness(LEDs->data->getBrightness());
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(this->rect(), QBrush(QColor(48, 47, 47)));
}

void MainWindow::changeToSingleColor() {
      ui->stackedWidget->setCurrentIndex(0);
      ui->singleColorButton->setChecked(true);
      ui->multiColorButton->setChecked(false);
      ui->arrayColorsButton->setChecked(false);
      ui->settingsButton->setChecked(false);
}

void MainWindow::changeToMultiColor() {
      ui->stackedWidget->setCurrentIndex(1);
      ui->singleColorButton->setChecked(false);
      ui->multiColorButton->setChecked(true);
      ui->arrayColorsButton->setChecked(false);
      ui->settingsButton->setChecked(false);
}

void MainWindow::changeToSavedColors() {
      ui->stackedWidget->setCurrentIndex(2);
      ui->singleColorButton->setChecked(false);
      ui->multiColorButton->setChecked(false);
      ui->arrayColorsButton->setChecked(true);
      ui->settingsButton->setChecked(false);
}

void MainWindow::changeToSettings() {
      ui->stackedWidget->setCurrentIndex(3);
      ui->singleColorButton->setChecked(false);
      ui->multiColorButton->setChecked(false);
      ui->arrayColorsButton->setChecked(false);
      ui->settingsButton->setChecked(true);
}
