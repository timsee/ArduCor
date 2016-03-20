

#ifndef ARRAYCOLORSPAGE_H
#define ARRAYCOLORSPAGE_H

#include "lightscontrol.h"

#include <QWidget>
#include <QDebug>

namespace Ui {
class ArrayColorsPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The ArrayColorsPage class uses the color array from the arduino's RoutinesRGB library.
 *
 * The bar at the top of the page determines how many of the colors from the array to use. For example,
 * when its set to `2` it will use only the first `2` colors. Setting it to `5` will use `5`
 * array colors, etc.
 *
 * \todo Add the ability to set each array color individually.
 *
 */
class ArrayColorsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ArrayColorsPage(QWidget *parent = 0);
    ~ArrayColorsPage();
    LightsControl *LEDs;
    void highlightButton(DataLayer::ELightingMode lightingMode);

/*!
 * used to signal back to the main page that it should update its top-left icon
 * with a new color mode
 */
signals:
    void updateMainIcons();

/*!
 * slots used to change the mode of the lights
 * or to change the settings of the different modes
 */
public slots:
    void changeToGlimmer();
    void changeToRandomIndividual();
    void changeToRandomSolid();
    void changeToFade();
    void changeToBarsSolid();
    void changeToBarsMoving();
    void colorCountChanged(int newCount);

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::ArrayColorsPage *ui;
    DataLayer::ELightingMode mCurrentMode;

    void updateIcons();
};

#endif // ARRAYCOLORSPAGE_H
