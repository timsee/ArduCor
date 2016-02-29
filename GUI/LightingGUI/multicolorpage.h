/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef MULTICOLORPAGE_H
#define MULTICOLORPAGE_H

#include "lightscontrol.h"

#include <QWidget>

namespace Ui {
class MultiColorPage;
}

/*!
 * \brief The MultiColorPage class contains
 * preprogrammed lighting routines that use a
 * a variety of colors. One example is individually
 * setting each LED a different random color.
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
