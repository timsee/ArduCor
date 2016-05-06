/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "lightsbutton.h"
#include <QDebug>

LightsButton::LightsButton(QWidget *parent) : QWidget(parent) {
    mLightingRoutine = ELightingRoutine::eLightingRoutine_MAX;
    mColorGroup = EColorGroup::eColorGroup_MAX;
    mIsPresetButton = false;

    button = new QPushButton(this);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(buttonClicked()));

    mLayout = new QHBoxLayout;
    mLayout->addWidget(button);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(mLayout);
}

void LightsButton::setupAsMultiButton(ELightingRoutine routine, EColorGroup colorGroup, std::shared_ptr<DataLayer> dataLayer) {
    mIconData = IconData(120, 120, dataLayer);
    button->setIcon(mIconData.renderAsQPixmap());

    mIsPresetButton = true;
    bool renderIcon = false;
    switch(routine) {
        case ELightingRoutine::eMultiGlimmer:
        case ELightingRoutine::eMultiFade:
        case ELightingRoutine::eMultiRandomSolid:
        case ELightingRoutine::eMultiRandomIndividual:
        case ELightingRoutine::eMultiBarsSolid:
        case ELightingRoutine::eMultiBarsMoving:
            renderIcon = true;
            break;
        default:
            break;
    }

    if (renderIcon) {
        mIconData.setLightingRoutine(routine, colorGroup);
        button->setIcon(mIconData.renderAsQPixmap());
    }

    mLightingRoutine = routine;
    mColorGroup = colorGroup;
}

ELightingRoutine LightsButton::lightingRoutine() {
    return mLightingRoutine;
}

EColorGroup LightsButton::colorGroup() {
    return mColorGroup;
}

void LightsButton::resizeEvent(QResizeEvent *event) {
    button->setIconSize(QSize(button->rect().size().width() * 4 / 5,
                              button->rect().size().height() * 4 / 5));
}


void LightsButton::buttonClicked() {
    if (mIsPresetButton) {
        emit presetClicked((int)mLightingRoutine,(int)mColorGroup);
    }
}
