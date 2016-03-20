
#ifndef MULTICOLORPAGE_H
#define MULTICOLORPAGE_H

#include "lightscontrol.h"

#include <QWidget>

namespace Ui {
class MultiColorPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The MultiColorPage class contains preprogrammed lighting routines
 * that use a variety of colors. These routines require no parameters
 * or settings and because of this, its the simplest page in the
 * application: Its a grid of buttons.
 *
 */
class MultiColorPage : public QWidget
{
    Q_OBJECT

public:
    explicit MultiColorPage(QWidget *parent = 0);
    ~MultiColorPage();
    LightsControl *LEDs;
    void highlightButton(DataLayer::ELightingMode lightingMode);


/*!
 * used to signal back to the main page that it should update its top-left icon
 * with a new color mode
 */
signals:
    void updateMainIcons();

/*!
 * Slots for button callbacks, used to change
 * the mode of the lights
 */
public slots:
    void changeToRandomSolid();
    void changeToRandomIndividual();
    void changeToFadeAll();

/*!
 * called whenever the page is shown on screen.
 */
protected:
    void showEvent(QShowEvent *);

private:
    Ui::MultiColorPage *ui;
    DataLayer::ELightingMode mCurrentMode;
};

#endif // MULTICOLORPAGE_H
