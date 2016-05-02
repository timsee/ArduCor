

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
 * \brief The CustomArrayPage provides a way to use the Custom Color Array
 *        from the arduino's RoutinesRGB library.
 *
 * The page contains an interface to change the individual array colors and
 * a series of buttons that change the mode.
 *
 * The top interface is split into three parts: a ColorPicker, a color array
 * preview, and  a LightsSlider. The ColorPicker chooses the color that is
 * currently selected from the color array preview. The LightsSlider below the
 * preview is used to choose how many colors are active while using an array mode.
 */
class CustomArrayPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     */
    explicit CustomArrayPage(QWidget *parent = 0);
    /*!
     * \brief Deconstructor
     */
    ~CustomArrayPage();

    /*!
     * \brief highlightModeButton highlights the button that implements
     *        the lightingMode parameter. If it can't find a button that
     *        implements lightingMode, then all buttons are unhighlighted
     * \param lightingMode the mode that the highlighted button implements.
     */
    void highlightModeButton(ELightingMode lightingMode);

signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();

public slots:
    /*!
     * \brief modeChanged called whenever a mode button is pressed.
     */
    void modeChanged(int);
    /*!
     * \brief colorsUsedChanged called whenever the colors used slider
     *        (the slider above the mode buttons) changes its value.
     */
    void colorsUsedChanged(int);
    /*!
     * \brief colorChanged called whenever the color picker is used.
     */
    void colorChanged(QColor);
    /*!
     * \brief selectArrayColor called whenever a color array icon
     *        is clicked. This sets the that index of the color array
     *        as the color you can change with the color picker.
     */
    void selectArrayColor(int);

protected:
    /*!
     * \brief showEvent used to check for edge cases and to sync up the
     *        page to recent state changes before displaying the page
     */
    void showEvent(QShowEvent *);

private:
    /*!
     * \brief ui pointer to Qt UI form.
     */
    Ui::CustomArrayPage *ui;

    /*!
     * \brief mArrayColorsButtons buttons for changing the lighting mode.
     */
    std::shared_ptr<std::vector<QToolButton *> > mArrayColorsButtons;
    /*!
     * \brief mArrayColorsIconData IconData for the buttons that are used for
     *        changing the lighting mode.
     */
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

    /*!
     * \brief updateColorArray called whenever the color array experiences
     *        a change, from the user interacting with the colorPicker
     *        or from the user interacting with the colors used array
     */
    void updateColorArray();

    /*!
     * \brief updateIcons called whenever the colors getting used change
     *        so that the icons can sync up to the new color values.
     */
    void updateIcons();
};

#endif // CUSTOMARRAYPAGE_H
