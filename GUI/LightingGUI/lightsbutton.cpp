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
    mSetupHasBeenCalled = false;
    mIsMenuButton = false;

    button = new QPushButton(this);
    button->setCheckable(true);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(handleButton()));
}

void LightsButton::setupAsStandardButton(ELightingRoutine routine, EColorGroup colorGroup, std::shared_ptr<DataLayer> dataLayer) {
    mIconData = IconData(64, 64, dataLayer);
    button->setIcon(mIconData.renderAsQPixmap());
    mSetupHasBeenCalled = true;
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

    mLayout = new QVBoxLayout;
    mLayout->addWidget(button);
    mLayout->setContentsMargins(5,5,5,5);
    button->setMinimumWidth(75);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(mLayout);
}


void LightsButton::setupAsMenuButton(int pageNumber, std::shared_ptr<DataLayer> dataLayer) {
    mIconData = IconData(64, 64, dataLayer);
    if (pageNumber == 0) {
        mIconData.setSolidColor(QColor(0,255,0));
    } else if (pageNumber == 1) {
        mIconData.setMultiFade(EColorGroup::eCustom, true);
    } else {
        mIconData.setMultiFade(EColorGroup::eSevenColor);
    }
    button->setIcon(mIconData.renderAsQPixmap());

    mSetupHasBeenCalled = true;
    mIsMenuButton = true;
    mPageNumber = pageNumber;
    //this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0,0,0,0);
    mLayout->addWidget(button);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setLayout(mLayout);
}

void LightsButton::setupAsLabeledButton(QString label, ELightingRoutine routine, std::shared_ptr<DataLayer> dataLayer, EColorGroup colorGroup) {
    mIconData = IconData(64, 64, dataLayer);
    button->setIcon(mIconData.renderAsQPixmap());
    mSetupHasBeenCalled = true;
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

    buttonLabel = new QLabel;
    buttonLabel->setText(label);
    buttonLabel->setAlignment(Qt::AlignCenter);

    mLightingRoutine = routine;
    mColorGroup = colorGroup;

    mLayout = new QVBoxLayout;
    mLayout->addWidget(button);
    mLayout->addWidget(buttonLabel);
    mLayout->setContentsMargins(3,3,3,3);
    button->setMinimumWidth(75);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mLayout->setStretch(0, 50);
    mLayout->setStretch(1, 1);
    setLayout(mLayout);
}

ELightingRoutine LightsButton::lightingRoutine() {
    return mLightingRoutine;
}

EColorGroup LightsButton::colorGroup() {
    return mColorGroup;
}

void LightsButton::resizeEvent(QResizeEvent *event) {
    if (mIsMenuButton) {
        button->setIconSize(QSize(button->rect().size().height() * 0.8f,
                                  button->rect().size().height() * 0.8f));
    } else {
        button->setIconSize(QSize(this->size().height() * 0.6f,
                                  this->size().height() * 0.6f));
    }
}


void LightsButton::handleButton() {
    if (mSetupHasBeenCalled) {
        if (mIsMenuButton) {
            emit menuButtonClicked(mPageNumber);
        } else {
            emit buttonClicked((int)mLightingRoutine,(int)mColorGroup);
        }
    }
}
