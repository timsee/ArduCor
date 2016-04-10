
#ifndef SINGLECOLORPAGE_H
#define SINGLECOLORPAGE_H

#include "icondata.h"
#include "lightsslider.h"
#include "lightingpage.h"
#include "colorpicker.h"

#include <QWidget>
#include <QSlider>
#include <QToolButton>

namespace Ui {
class SingleColorPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The SingleColorPage class allows users to change
 * the main color of the LED system and to set it in single-color modes.
 *
 * The page contains a ColorPicker widget used to choose the color and a series of
 * buttons that change the mode.
 *
 * The single color modes current supported are solid, blink, fade, and glimmer.
 *
 */
class SingleColorPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit SingleColorPage(QWidget *parent = 0);
    ~SingleColorPage();
    void highlightModeButton(ELightingMode lightingMode);
    void chooseColor(QColor color);

/*!
 * used to signal back to the main page that it should update its top-left icon
 * with new RGB values
 */
signals:
    void updateMainIcons();


public slots:
    void modeChanged(int);

    void colorChanged(QColor);

protected:
    void showEvent(QShowEvent *);

private:
    Ui::SingleColorPage *ui;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton*> > mPageButtons;

    std::vector<IconData> iconData;

    IconData mSolidData;
    IconData mFadeData;
    IconData mBlinkData;
    IconData mGlimmerData;

    ELightingMode mCurrentMode;

};

#endif // SINGLECOLORPAGE_H
