/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "presetarraypage.h"
#include "ui_presetarraypage.h"
#include <QDebug>
#include <QSignalMapper>
#include "icondata.h"

PresetArrayPage::PresetArrayPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PresetArrayPage) {
    ui->setupUi(this);

    ui->scrollArea->setStyleSheet("background-color:transparent;");
}

PresetArrayPage::~PresetArrayPage() {
    delete ui;
}


void
PresetArrayPage::setupPresets()
{
    mPresetMax = (int)EColorPreset::eColorPreset_MAX - 1;
    mButtonCount = mPresetMax * ((int)ELightingMode::eLightingMode_MAX - (int)ELightingMode::eMultiGlimmer);
    std::vector<std::string> labels = {"Water",
                                       "Frozen",
                                       "Snow",
                                       "Cool",
                                       "Warm",
                                       "Fire",
                                       "Evil",
                                       "Corrosive",
                                       "Poison",
                                       "Rose",
                                       "PinkGreen",
                                       "RedWhiteBlue",
                                       "All",
                                       "RGB",
                                       "CMY",
                                       "SixColor",
                                       "SevenColor"};

    mButtonLayout = new QGridLayout;
    mPresetButtons = std::vector<LightsButton *>(mButtonCount, nullptr);
    mPresetLabels = std::vector<QLabel *>(mPresetMax, nullptr);

    int modeIndex = 0;
    int presetIndex = 0;
    int layoutColumnIndex = 0;
    for (int preset = (int)EColorPreset::eWater; preset < (int)EColorPreset::eColorPreset_MAX; preset++) {
        mPresetLabels[presetIndex] = new QLabel;
        mPresetLabels[presetIndex]->setText(labels[presetIndex].c_str());
        mPresetLabels[presetIndex]->setAlignment(Qt::AlignCenter);
        mPresetLabels[presetIndex]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        layoutColumnIndex = 0;
        // add to layout
        mButtonLayout->addWidget(mPresetLabels[presetIndex], layoutColumnIndex, presetIndex);
        layoutColumnIndex++;
        for (int mode = (int)ELightingMode::eMultiGlimmer; mode < (int)ELightingMode::eLightingMode_MAX; mode++) {
            mPresetButtons[modeIndex] = new LightsButton;
            mPresetButtons[modeIndex]->setupAsPresetButton((ELightingMode)mode, (EColorPreset)preset, mData);
            mPresetButtons[modeIndex]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(mPresetButtons[modeIndex], SIGNAL(presetClicked(int, int)), this, SLOT(presetButtonClicked(int, int)));

            // add to layout
            mButtonLayout->addWidget(mPresetButtons[modeIndex], layoutColumnIndex, presetIndex);
            layoutColumnIndex++;
            modeIndex++;
        }
        presetIndex++;
    }

    ui->scrollArea->setWidgetResizable(true);
    mButtonLayout->setVerticalSpacing(0);
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->scrollAreaWidgetContents->setLayout(mButtonLayout);

    ui->scrollArea->setStyleSheet("background-color:transparent;");
 }

void PresetArrayPage::highlightModeButton(ELightingMode mode, EColorPreset preset) {
    for (int i = 0; i < mButtonCount; i++) {
        if ((mPresetButtons[i]->lightingMode() == mode)
                && (mPresetButtons[i]->colorPreset() == preset)) {
            mPresetButtons[i]->button->setChecked(true);
        }
        else {
            mPresetButtons[i]->button->setChecked(false);
        }
    }
}


// ----------------------------
// Slots
// ----------------------------

void PresetArrayPage::presetButtonClicked(int mode, int preset) {
    mData->currentMode((ELightingMode)mode);
    mData->preset((EColorPreset)preset);
    mComm->sendArrayModeChange((ELightingMode)mode, preset);
    highlightModeButton(mData->currentMode(), mData->preset());
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void PresetArrayPage::showEvent(QShowEvent *) {
    highlightModeButton(mData->currentMode(), mData->preset());
}

