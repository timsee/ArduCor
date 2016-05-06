/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "presetcolorspage.h"
#include "ui_PresetColorsPage.h"
#include <QDebug>
#include <QSignalMapper>
#include "icondata.h"

PresetColorsPage::PresetColorsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PresetColorsPage) {
    ui->setupUi(this);

    ui->scrollArea->setStyleSheet("background-color:transparent;");
}

PresetColorsPage::~PresetColorsPage() {
    delete ui;
}


void
PresetColorsPage::setupPresets()
{
    int colorGroupMax = (int)EColorGroup::eColorGroup_MAX - 1;
    mButtonCount = colorGroupMax * ((int)ELightingRoutine::eLightingRoutine_MAX - (int)ELightingRoutine::eMultiGlimmer);
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
                                       "RGB",
                                       "CMY",
                                       "SixColor",
                                       "SevenColor",
                                       "All"};

    mButtonLayout = new QGridLayout;
    mPresetButtons = std::vector<LightsButton *>(mButtonCount, nullptr);
    mPresetLabels = std::vector<QLabel *>(colorGroupMax, nullptr);

    int modeIndex = 0;
    int groupIndex = 0;
    int layoutColumnIndex = 0;
    for (int preset = (int)EColorGroup::eWater; preset < (int)EColorGroup::eColorGroup_MAX; preset++) {
        mPresetLabels[groupIndex] = new QLabel;
        mPresetLabels[groupIndex]->setText(labels[groupIndex].c_str());
        mPresetLabels[groupIndex]->setAlignment(Qt::AlignCenter);
        mPresetLabels[groupIndex]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        layoutColumnIndex = 0;
        // add to layout
        mButtonLayout->addWidget(mPresetLabels[groupIndex], layoutColumnIndex, groupIndex);
        layoutColumnIndex++;
        for (int routine = (int)ELightingRoutine::eMultiGlimmer; routine < (int)ELightingRoutine::eLightingRoutine_MAX; routine++) {
            mPresetButtons[modeIndex] = new LightsButton;
            mPresetButtons[modeIndex]->setupAsMultiButton((ELightingRoutine)routine, (EColorGroup)preset, mData);
            mPresetButtons[modeIndex]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(mPresetButtons[modeIndex], SIGNAL(presetClicked(int, int)), this, SLOT(multiButtonClicked(int, int)));

            // add to layout
            mButtonLayout->addWidget(mPresetButtons[modeIndex], layoutColumnIndex, groupIndex);
            layoutColumnIndex++;
            modeIndex++;
        }
        groupIndex++;
    }

    ui->scrollArea->setWidgetResizable(true);
    mButtonLayout->setVerticalSpacing(0);
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->scrollAreaWidgetContents->setLayout(mButtonLayout);

    ui->scrollArea->setStyleSheet("background-color:transparent;");
 }

void PresetColorsPage::highlightRoutineButton(ELightingRoutine routine, EColorGroup colorGroup) {
    for (int i = 0; i < mButtonCount; i++) {
        if ((mPresetButtons[i]->lightingRoutine() == routine)
                && (mPresetButtons[i]->colorGroup() == colorGroup)) {
            mPresetButtons[i]->button->setChecked(true);
            mPresetButtons[i]->button->setStyleSheet("background-color: rgb(67, 67, 67); ");
        }
        else {
            mPresetButtons[i]->button->setChecked(false);
            mPresetButtons[i]->button->setStyleSheet("background-color: rgb(47, 47, 47);");
        }
    }
}


// ----------------------------
// Slots
// ----------------------------

void PresetColorsPage::multiButtonClicked(int routine, int colorGroup) {
    mData->currentRoutine((ELightingRoutine)routine);
    mData->currentColorGroup((EColorGroup)colorGroup);
    mComm->sendRoutineChange((ELightingRoutine)routine, colorGroup);
    highlightRoutineButton(mData->currentRoutine(), mData->currentColorGroup());
    emit updateMainIcons();
}


// ----------------------------
// Protected
// ----------------------------

void PresetColorsPage::showEvent(QShowEvent *) {
    highlightRoutineButton(mData->currentRoutine(), mData->currentColorGroup());
}

