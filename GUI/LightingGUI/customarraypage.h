

#ifndef CUSTOMARRAYPAGE_H
#define CUSTOMARRAYPAGE_H

#include "lightingpage.h"
#include "icondata.h"

#include <QWidget>
#include <QDebug>
#include <QToolButton>
#include <QPushButton>
#include <QGridLayout>

namespace Ui {
class CustomArrayPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The CustomArrayPage class uses the color array routines from the arduino's RoutinesRGB library.
 *
 * The page contains an interface to change the individual array colors and a series of buttons
 * that change the mode.
 *
 * The top interface is split into three parts: a ColorPicker, a color array preview, and
 * a LightsSlider. The ColorPicker chooses the color that is currently selected from the color
 * array preview. The LightsSlider below the preview is used to choose how many colors are active
 * while using an array mode.
 */
class CustomArrayPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit CustomArrayPage(QWidget *parent = 0);
    ~CustomArrayPage();

    void highlightModeButton(ELightingMode lightingMode);

signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();

/*!
 * slots used to change the mode of the lights
 * or to change the settings of the different modes
 */
public slots:
    void modeChanged(int);
    void colorsUsedChanged(int);
    void colorChanged(QColor);
    void selectArrayColor(int);

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::CustomArrayPage *ui;

    std::shared_ptr<std::vector<QToolButton *> > mArrayColorsButtons;
    std::shared_ptr<std::vector<IconData> > mArrayColorsIconData;

    /*!
     * \brief mGreyIcon used for greying out icons
     */
    IconData mGreyIcon;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton *> > mPageButtons;

    /*!
     * \brief mCurrentColorPickerIndex current index being set by the GUI.
     */
    int mCurrentColorPickerIndex;
    void updateColorArray();
    void updateIcons();
};

#endif // CUSTOMARRAYPAGE_H
