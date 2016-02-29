/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef SINGLECOLORPAGE_H
#define SINGLECOLORPAGE_H

#include "lightscontrol.h"
#include "icondata.h"
#include "lightsslider.h"

#include <QWidget>
#include <QSlider>

namespace Ui {
class SingleColorPage;
}

/*!
 * \brief The SingleColorPage class allows uses to change
 * the main color of the LED system, which is used to display
 * single color routines such as blinking on and off or
 * displaying a single solid color.
 */
class SingleColorPage : public QWidget
{
    Q_OBJECT

public:
    explicit SingleColorPage(QWidget *parent = 0);
    ~SingleColorPage();
    LightsControl *LEDs;
    void highlightButton(DataLayer::ELightingMode lightingMode);

/*!
 * used to signal back to the main page that it should update its top-left icon
 * with new RGB values
 */
signals:
    void colorUpdated(int, int, int);

public slots:
    void rChanged(int newR);
    void gChanged(int newG);
    void bChanged(int newB);

    void changeToOff();
    void changeToSolid();
    void changeToBlink();
    void changeToFade();
    void changeToGlimmer();

    void setNewColor();
    void updateIcons();
    void colorWheelChanged(int r, int g, int b);

protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::SingleColorPage *ui;
    int mRed;
    int mGreen;
    int mBlue;

    IconData mSolidData;
    IconData mFadeData;
    IconData mBlinkData;
    IconData mGlimmerData;

    DataLayer::ELightingMode mCurrentMode;
    void updateColorPreview();
};

#endif // SINGLECOLORPAGE_H
