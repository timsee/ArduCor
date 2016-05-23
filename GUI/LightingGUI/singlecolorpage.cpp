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

    ui->colorPicker->chooseLayout(ELayoutColorPicker::eFullLayout);
    connect(ui->colorPicker, SIGNAL(colorUpdate(QColor)), this, SLOT(colorChanged(QColor)));
}


SingleColorPage::~SingleColorPage() {
    delete ui;
}

void SingleColorPage::chooseColor(QColor color) {
  ui->colorPicker->chooseColor(color);
}


void SingleColorPage::setupButtons() {
    mIconData = IconData(64,64, mData);

    std::vector<std::string> labels = {"Solid",
                                       "Blink",
                                       "Fade",
                                       "Glimmer"};

    std::vector<LightsButton *> buttons = {ui->solidButton,
                                           ui->blinkButton,
                                           ui->fadeButton,
                                           ui->glimmerButton};

    mRoutineButtons = std::shared_ptr<std::vector<LightsButton*> >(new std::vector<LightsButton*>(4, nullptr));
    for (int i = 0; i < 4; ++i) {
        (*mRoutineButtons.get())[i] = buttons[i];
        (*mRoutineButtons.get())[i]->setupAsLabeledButton(QString::fromStdString(labels[i]), (ELightingRoutine)(i + 1), mData);
        connect((*mRoutineButtons.get())[i], SIGNAL(buttonClicked(int, int)), this, SLOT(modeChanged(int, int)));
   }
}

void SingleColorPage::highlightRoutineButton(ELightingRoutine routine) {
    for (uint i = 0; i < mRoutineButtons->size(); i++) {
        (*mRoutineButtons.get())[i]->button->setChecked(false);
    }

    if (routine == ELightingRoutine::eSingleSolid) {
        ui->solidButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleBlink) {
        ui->blinkButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleFade) {
        ui->fadeButton->button->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleGlimmer) {
        ui->glimmerButton->button->setChecked(true);
    }
}

// ----------------------------
// Slots
// ----------------------------
// newGroup is ignored for single color routines
void SingleColorPage::modeChanged(int newMode, int newGroup) {
    mData->currentRoutine((ELightingRoutine)newMode);
    mComm->sendRoutineChange((ELightingRoutine)newMode);
    highlightRoutineButton((ELightingRoutine)newMode);
    emit updateMainIcons();
}

void SingleColorPage::colorChanged(QColor color) {
    mData->mainColor(color);

    mComm->sendMainColorChange(mData->mainColor());
    if (!(mData->currentRoutine() == ELightingRoutine::eSingleBlink
            || mData->currentRoutine() == ELightingRoutine::eSingleSolid
            || mData->currentRoutine() == ELightingRoutine::eSingleFade
            || mData->currentRoutine() == ELightingRoutine::eSingleGlimmer)) {
        mData->currentRoutine(ELightingRoutine::eSingleGlimmer);
        mComm->sendRoutineChange(ELightingRoutine::eSingleGlimmer);
    }

    mIconData.setSolidColor(color);
    ui->solidButton->button->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addFade();
    ui->fadeButton->button->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addGlimmer();
    ui->glimmerButton->button->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addBlink();
    ui->blinkButton->button->setIcon(mIconData.renderAsQPixmap());

    emit updateMainIcons();
}

// ----------------------------
// Protected
// ----------------------------


void SingleColorPage::showEvent(QShowEvent *) {
  highlightRoutineButton(mData->currentRoutine());
}

