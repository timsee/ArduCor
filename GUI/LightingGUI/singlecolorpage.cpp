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

    mIconData = IconData(ui->solidButton->iconSize().width(), ui->solidButton->iconSize().height());

    mPageButtons = std::shared_ptr<std::vector<QToolButton*> >(new std::vector<QToolButton*>(4, nullptr));
    (*mPageButtons.get())[0] = ui->solidButton;
    (*mPageButtons.get())[1] = ui->blinkButton;
    (*mPageButtons.get())[2] = ui->fadeButton;
    (*mPageButtons.get())[3] = ui->glimmerButton;

    // loop through the buttons and set up their clicks to a mapper
    QSignalMapper* signalMapper = new QSignalMapper(this);
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setCheckable(true);
        connect(button, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));
    }

    signalMapper->setMapping(ui->solidButton, (int)ELightingRoutine::eSingleSolid);
    signalMapper->setMapping(ui->blinkButton, (int)ELightingRoutine::eSingleBlink);
    signalMapper->setMapping(ui->fadeButton, (int)ELightingRoutine::eSingleFade);
    signalMapper->setMapping(ui->glimmerButton, (int)ELightingRoutine::eSingleGlimmer);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(modeChanged(int)));

    ui->colorPicker->chooseLayout(ELayoutColorPicker::eFullLayout);
    connect(ui->colorPicker, SIGNAL(colorUpdate(QColor)), this, SLOT(colorChanged(QColor)));
}


SingleColorPage::~SingleColorPage() {
    delete ui;
}

void SingleColorPage::chooseColor(QColor color) {
  ui->colorPicker->chooseColor(color);
}

void SingleColorPage::highlightRoutineButton(ELightingRoutine routine) {
    for (uint i = 0; i < mPageButtons->size(); i++) {
        QToolButton* button =(*mPageButtons.get())[i];
        button->setChecked(false);
    }

    if (routine == ELightingRoutine::eSingleSolid) {
        ui->solidButton->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleBlink) {
        ui->blinkButton->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleFade) {
        ui->fadeButton->setChecked(true);
    } else if (routine == ELightingRoutine::eSingleGlimmer) {
        ui->glimmerButton->setChecked(true);
    }
}

// ----------------------------
// Slots
// ----------------------------

void SingleColorPage::modeChanged(int newMode) {
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
    ui->solidButton->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addFade();
    ui->fadeButton->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addGlimmer();
    ui->glimmerButton->setIcon(mIconData.renderAsQPixmap());

    mIconData.setSolidColor(color);
    mIconData.addBlink();
    ui->blinkButton->setIcon(mIconData.renderAsQPixmap());

    emit updateMainIcons();
}

// ----------------------------
// Protected
// ----------------------------


void SingleColorPage::showEvent(QShowEvent *) {
  highlightRoutineButton(mData->currentRoutine());
}

