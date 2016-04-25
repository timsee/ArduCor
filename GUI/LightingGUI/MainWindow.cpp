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

/*!
 * \brief The ELightingPage enum used internally to map the updatePreview to
 * int to a lighting page.
 */
enum class ELightingPage {
    ePageSingleLEDRoutines,
    ePageArrayLEDRoutines,
    ePageMultiLEDRoutines,
    ePageSettings
};


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
    ui->customArrayPage->setup(mComm, mData);
    ui->presetArrayPage->setup(mComm, mData);
    ui->settingsPage->setup(mComm, mData);
    ui->presetArrayPage->setupPresets();

    // --------------
    // Setup Pages
    // --------------
    QSignalMapper* previewSignalMapper = new QSignalMapper(this);

    connect(ui->singleColorPage, SIGNAL(updateMainIcons()), previewSignalMapper, SLOT(map()));
    connect(ui->customArrayPage, SIGNAL(updateMainIcons()), previewSignalMapper, SLOT(map()));
    connect(ui->presetArrayPage, SIGNAL(updateMainIcons()), previewSignalMapper, SLOT(map()));

    previewSignalMapper->setMapping(ui->singleColorPage, (int)ELightingPage::ePageSingleLEDRoutines);
    previewSignalMapper->setMapping(ui->customArrayPage, (int)ELightingPage::ePageArrayLEDRoutines);
    previewSignalMapper->setMapping(ui->presetArrayPage, (int)ELightingPage::ePageMultiLEDRoutines);

    connect(previewSignalMapper, SIGNAL(mapped(int)), this, SLOT(updatePreviewIcon(int)));


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
    mPreviewIcon = IconData(64, 64, mData);
    mPreviewIcon.setSolidColor(QColor(0,255,0));
    ui->onOffButton->setIcon(mPreviewIcon.renderAsQPixmap());


    // --------------
    // Setup Remaining Page Button Icons
    // --------------

    mSinglePageIcon = IconData(64, 64, mData);
    mSinglePageIcon.setSolidColor(QColor(0,255,0));
    ui->singleColorButton->setIcon(mSinglePageIcon.renderAsQPixmap());

    mCustomArrayIcon = IconData(64, 64, mData);
    mCustomArrayIcon.setArrayFade(EColorPreset::eCustom);
    ui->customArrayButton->setIcon(mCustomArrayIcon.renderAsQPixmap());

    mPresetArrayIcon = IconData(64, 64, mData);
    mPresetArrayIcon.setArrayFade(EColorPreset::eSevenColor);
    ui->presetArrayButton->setIcon(mPresetArrayIcon.renderAsQPixmap());

    // --------------
    // Final setup
    // --------------

    // reset the LED array to defaults
    mComm->sendReset();
    // Start on SingleColorPage
    pageChanged((int)ELightingPage::ePageSingleLEDRoutines);
    // setup the SingleColorPage
    ui->singleColorPage->chooseColor(mData->mainColor());
}

MainWindow::~MainWindow() {
    delete ui;
}


// ----------------------------
// Slots
// ----------------------------

void MainWindow::toggleOnOff() {
    if (mData->isOn()) {
        mPreviewIcon.setSolidColor(QColor(0,0,0));
        ui->onOffButton->setIcon(mPreviewIcon.renderAsQPixmap());
        mData->isOn(false);
        mComm->sendBrightness(0);
    } else {
        ui->onOffButton->setIcon(mPreviewIcon.renderAsQPixmap());
        mData->isOn(true);
        mComm->sendBrightness(mData->brightness());
    }
}

void MainWindow::brightnessChanged(int newBrightness) {
   mData->brightness(newBrightness);
   mComm->sendBrightness(mData->brightness());
   mData->isOn(true);
}


void MainWindow::pageChanged(int pageIndex) {
    for (uint i = 0; i < mPageButtons->size(); i++)
    {
        QPushButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }
    ui->stackedWidget->setCurrentIndex(pageIndex);
    (*mPageButtons.get())[pageIndex]->setChecked(true);
}


void MainWindow::updatePreviewIcon(int lightingPage) {
    if ((ELightingPage)lightingPage == ELightingPage::ePageSingleLEDRoutines) {
        QColor color = mData->mainColor();
        mPreviewIcon.setSolidColor(color);
        mSinglePageIcon.setSolidColor(color);
        ui->singleColorButton->setIcon(mSinglePageIcon.renderAsQPixmap());
    } else if ((ELightingPage)lightingPage == ELightingPage::ePageArrayLEDRoutines) {
        mPreviewIcon.setSolidColor(QColor(0,0,0));
        mPreviewIcon.setArrayFade(EColorPreset::eCustom);
        mCustomArrayIcon.setArrayFade(EColorPreset::eCustom);
        ui->customArrayButton->setIcon(mCustomArrayIcon.renderAsQPixmap());
    } else if ((ELightingPage)lightingPage == ELightingPage::ePageMultiLEDRoutines) {
        mPreviewIcon.setSolidColor(QColor(0,0,0));
        mPreviewIcon.setArrayFade(EColorPreset::eSevenColor);
    }
    ui->onOffButton->setIcon(mPreviewIcon.renderAsQPixmap());
    mData->isOn(true);
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
