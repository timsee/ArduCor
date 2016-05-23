
#ifndef SINGLECOLORPAGE_H
#define SINGLECOLORPAGE_H

#include "icondata.h"
#include "lightsslider.h"
#include "lightingpage.h"
#include "colorpicker.h"
#include "lightsbutton.h"

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
 * \brief The SingleColorPage provides a way to change the main
 * color of the LED system and to set it in single-color modes.
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
    /*!
     * \brief Constructor
     */
    explicit SingleColorPage(QWidget *parent = 0);
    /*!
     * \brief Deconstructor
     */
    ~SingleColorPage();
    /*!
     * \brief highlightRoutineButton highlights the button that implements
     *        the routine parameter. If it can't find a button that
     *        implements the lighting routine, then all buttons are unhighlighted
     * \param routine the mode that the highlighted button implements.
     */
    void highlightRoutineButton(ELightingRoutine routine);
    /*!
     * \brief chooseColor allows the color of the ColorPicker and the single
     *        color routines to be set programmatically. Used by the MainWindow
     *        during startup.
     * \param color the color that the SingleColorPage should use for its colorPicker
     *        and its single color routines.
     */
    void chooseColor(QColor color);

    /*!
     * \brief setupButtons sets up the routine buttons. Requires the DataLayer
     *        of the application to be set up first.
     */
    void setupButtons();

signals:
    /*!
     * \brief Used to signal back to the main page that it should update its top-left icon
     *        with new RGB values
     */
    void updateMainIcons();


public slots:
    /*!
     * \brief modeChanged signaled whenever a mode button is pressed, used to
     *        to tell the LEDs to update.
     */
    void modeChanged(int, int);
    /*!
     * \brief colorChanged signaled whenever the ColorPicker chooses a new color.
     */
    void colorChanged(QColor);

protected:
    /*!
     * \brief showEvent called whenever the page is about to be shown, used to
     *        to make sure that the the hilighted buttons and the modes that could
     *        have changed from other pages are still in sync on this page.
     */
    void showEvent(QShowEvent *);

private:
    /*!
     * \brief ui pointer to Qt UI form.
     */
    Ui::SingleColorPage *ui;

    /*!
     * \brief mRoutineButtons pointers to all the main buttons, used
     *        to iterate through them quickly.
     */
    std::shared_ptr<std::vector<LightsButton*> > mRoutineButtons;

    /*!
     * \brief IconData for the Single Color Routine Icons
     */
    IconData mIconData;
};

#endif // SINGLECOLORPAGE_H
