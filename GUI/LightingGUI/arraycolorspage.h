/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef ARRAYCOLORSPAGE_H
#define ARRAYCOLORSPAGE_H

#include "lightscontrol.h"

#include <QWidget>
#include <QDebug>

namespace Ui {
class ArrayColorsPage;
}

/*!
 * \brief The ArrayColorsPage class uses the saved array on the arduino
 * to set the lights. This array can have all its colors changed individually,
 * but currently it just allows you to choose how many colors to use from the array.
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
