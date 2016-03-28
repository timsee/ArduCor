
#ifndef SINGLECOLORPAGE_H
#define SINGLECOLORPAGE_H

#include "icondata.h"
#include "lightsslider.h"
#include "lightingpage.h"

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
 * The single color modes current supported are solid, blink, fade, and glimmer.
 *
 * The page provides a color wheel and RGB sliders to give the user multiple ways
 * to set the color.
 *
 */
class SingleColorPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit SingleColorPage(QWidget *parent = 0);
    ~SingleColorPage();
    void highlightButton(ELightingMode lightingMode);
    void updateColorPreview();

/*!
 * used to signal back to the main page that it should update its top-left icon
 * with new RGB values
 */
signals:
    void updateMainIcons();

public slots:
    void modeChanged(int);

    void setNewColor();
    void colorWheelChanged(QColor);

    void rChanged(int);
    void gChanged(int);
    void bChanged(int);


protected:
    void showEvent(QShowEvent *);
    void resizeEvent(QResizeEvent *event);

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
    void updateIcons();

};

#endif // SINGLECOLORPAGE_H
