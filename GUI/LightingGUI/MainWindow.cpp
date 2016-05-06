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
#include <QSignalMapper>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle("LightingGUI");

    // --------------
    // Setup Backend
    // --------------
    mData = std::shared_ptr<DataLayer>(new DataLayer());
    mComm =std::shared_ptr<CommLayer>(new CommLayer());
    ui->singleColorPage->setup(mComm, mData);
    ui->customColorsPage->setup(mComm, mData);
    ui->presetColorsPage->setup(mComm, mData);
    ui->settingsPage->setup(mComm, mData);
    ui->presetColorsPage->setupPresets();

    // --------------
    // Setup Pages
    // --------------

    connect(ui->singleColorPage, SIGNAL(updateMainIcons()),  this, SLOT(updateMenuBar()));
    connect(ui->customColorsPage, SIGNAL(updateMainIcons()), this, SLOT(updateMenuBar()));
    connect(ui->presetColorsPage, SIGNAL(updateMainIcons()), this, SLOT(updateMenuBar()));

    // --------------
    // Setup Buttons
    // --------------

    // add buttons to vector to make them easier to loop through
    mPageButtons = std::shared_ptr<std::vector<QPushButton*> >(new std::vector<QPushButton*>(4, nullptr));
    (*mPageButtons.get())[0] = ui->singleColorButton;
    (*mPageButtons.get())[1] = ui->customArrayButton;
    (*mPageButtons.get())[2] = ui->presetArrayButton;
    (*mPageButtons.get())[3] = ui->settingsButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QPushButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
        button->setStyleSheet("background-color: rgb(52, 52, 52); ");
        signalMapper->setMapping(button, i);
    }
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(pageChanged(int)));


    // --------------
    // Setup Brightness Slider
    // --------------

    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(brightnessChanged(int)));
    // setup the slider that controls the LED's brightness
    ui->brightnessSlider->slider->setRange(0,100);
    ui->brightnessSlider->slider->setValue(50);
    ui->brightnessSlider->slider->setTickPosition(QSlider::TicksBelow);
    ui->brightnessSlider->slider->setTickInterval(20);
    ui->brightnessSlider->setSliderColorBackground(QColor(255,255,255));


    // --------------
    // Setup Preview Button
    // --------------

    connect(ui->onOffButton, SIGNAL(clicked(bool)), this, SLOT(toggleOnOff()));

    // setup the icons
    mIconData = IconData(64, 64, mData);
    mIconData.setSolidColor(QColor(0,255,0));
    ui->onOffButton->setIcon(mIconData.renderAsQPixmap());

    // --------------
    // Setup Remaining Page Button Icons
    // --------------

    mIconData.setSolidColor(QColor(0,255,0));
    ui->singleColorButton->setIcon(mIconData.renderAsQPixmap());

    mIconData.setMultiFade(EColorGroup::eCustom, true);
    ui->customArrayButton->setIcon(mIconData.renderAsQPixmap());

    mIconData.setMultiFade(EColorGroup::eSevenColor);
    ui->presetArrayButton->setIcon(mIconData.renderAsQPixmap());

    // --------------
    // Final setup
    // --------------

    // reset the LED array to defaults
    mComm->sendReset();
    // Start on SingleColorPage
    pageChanged(0);
    // setup the SingleColorPage
    ui->singleColorPage->chooseColor(mData->mainColor());
}

MainWindow::~MainWindow() {
    delete ui;
}


// ----------------------------
// Slots
// ----------------------------
bool hackIsOn = true;
void MainWindow::toggleOnOff() {
    //TODO: why are we faking the on/off? why not use whats already
    //      built in?
    if (hackIsOn) {
        mIconData.setSolidColor(QColor(0,0,0));
        ui->onOffButton->setIcon(mIconData.renderAsQPixmap());
        hackIsOn = false;
        mComm->sendBrightness(0);
    } else {
        mIconData.setLightingRoutine(mData->currentRoutine(), mData->currentColorGroup());
        ui->onOffButton->setIcon(mIconData.renderAsQPixmap());
        hackIsOn = true;
        mComm->sendBrightness(mData->brightness());
    }
}

void MainWindow::brightnessChanged(int newBrightness) {
   mData->brightness(newBrightness);
   mComm->sendBrightness(mData->brightness());
   hackIsOn = true;
}


void MainWindow::pageChanged(int pageIndex) {
    for (uint i = 0; i < mPageButtons->size(); i++) {
        (*mPageButtons.get())[i]->setChecked(false);
        (*mPageButtons.get())[i]->setStyleSheet("background-color: rgb(52, 52, 52); ");
    }
    ui->stackedWidget->setCurrentIndex(pageIndex);
    (*mPageButtons.get())[pageIndex]->setChecked(true);
    (*mPageButtons.get())[pageIndex]->setStyleSheet("background-color: rgb(80, 80, 80); ");
}


void MainWindow::updateMenuBar() {
    if (mData->currentRoutine() <= ELightingRoutine::eSingleGlimmer) {
        ui->brightnessSlider->setSliderColorBackground(mData->mainColor());
        mIconData.setSolidColor(mData->mainColor());
        ui->singleColorButton->setIcon(mIconData.renderAsQPixmap());
        mIconData.setLightingRoutine(mData->currentRoutine());
    } else {
        ui->brightnessSlider->setSliderColorBackground(mData->colorsAverage(mData->currentColorGroup()));
        mIconData.setLightingRoutine(mData->currentRoutine(), mData->currentColorGroup());
        if (mData->currentColorGroup() == EColorGroup::eCustom) {
            mIconData.setMultiFade(EColorGroup::eCustom, true);
           ui->customArrayButton->setIcon(mIconData.renderAsQPixmap());
        } else {
           mIconData.setMultiFade(mData->currentColorGroup());
           ui->presetArrayButton->setIcon(mIconData.renderAsQPixmap());
        }
    }
    ui->onOffButton->setIcon(mIconData.renderAsQPixmap());
}


// ----------------------------
// Protected
// ----------------------------

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(this->rect(), QBrush(QColor(48, 47, 47)));
}
