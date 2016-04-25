/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "lightsbutton.h"
#include <QDebug>

LightsButton::LightsButton(QWidget *parent) : QWidget(parent) {
    mLightingMode = ELightingMode::eLightingMode_MAX;
    mColorPreset = EColorPreset::eColorPreset_MAX;
    mIsPresetButton = false;

    button = new QPushButton(this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

    mLayout = new QHBoxLayout;
    mLayout->addWidget(button);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(mLayout);
}

void LightsButton::setupAsPresetButton(ELightingMode mode, EColorPreset preset, std::shared_ptr<DataLayer> dataLayer) {
    mIconData = IconData(120, 120, dataLayer);
    button->setIcon(mIconData.renderAsQPixmap());

    mIsPresetButton = true;
    bool renderIcon = false;
    switch(mode) {
        case ELightingMode::eMultiGlimmer:
            mIconData.setArrayGlimmer(preset);
            renderIcon = true;
            break;
        case ELightingMode::eMultiFade:
            mIconData.setArrayFade(preset);
            renderIcon = true;
            break;
        case ELightingMode::eMultiRandomSolid:
            mIconData.setArrayRandomSolid(preset);
            renderIcon = true;
            break;
        case ELightingMode::eMultiRandomIndividual:
            mIconData.setArrayRandomIndividual(preset);
            renderIcon = true;
            break;
        case ELightingMode::eMultiBarsSolid:
            mIconData.setArrayBarsSolid(preset);
            renderIcon = true;
            break;
        case ELightingMode::eMultiBarsMoving:
            mIconData.setArrayBarsMoving(preset);
            renderIcon = true;
            break;
        default:
            break;
    }

    if (renderIcon) {
        button->setIcon(mIconData.renderAsQPixmap());
    }

    mLightingMode = mode;
    mColorPreset = preset;
}

ELightingMode LightsButton::lightingMode() {
    return mLightingMode;
}

EColorPreset LightsButton::colorPreset() {
    return mColorPreset;
}

void LightsButton::resizeEvent(QResizeEvent *event) {
    button->setIconSize(QSize(button->rect().size().width() * 4 / 5,
                              button->rect().size().height() * 4 / 5));
}


void LightsButton::buttonClicked() {
    if (mIsPresetButton) {
        emit presetClicked((int)mLightingMode,(int)mColorPreset);
    }
}
