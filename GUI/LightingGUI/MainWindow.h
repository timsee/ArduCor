
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "lightingpage.h"
#include "icondata.h"

namespace Ui {
class MainWindow;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The MainWindow class is the only window in this application.
 * It contains a QStackedWidget which shows 4 pages: SingleColorPage, ArrayColorsPage,
 * MultiColorPage, and the SettingsPage.
 *
 * On the top of the MainWindow, there is an on/off button in the far left. This button
 * will reflect the current mode when on, and will be black when off. There is
 * also a slider to control the overall brightness of the LEDs. 
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    /*!
     * \brief toggleOnOff Connected to the button in the top left of the GUI at all times.
     * Toggles between running the current routine at current settings, and off.
     */
    void toggleOnOff();
    /*!
     * \brief brightnessChanged Connected to the the slider at the top, this takeas a value between 0-100
     *  and sends that value to the lights to control how bright they are.
     */
    void brightnessChanged(int);
    /*!
     * \brief pageChanged Connected to the main menu buttons. Each button sends a different int,
     * which opens a different page on the QStackedWidget.
     */
    void pageChanged(int);
    /*!
     * \brief updatePreviewIcon used to update the icon in the top left of the screen that shows
     *        the current mode.
     */
    void updatePreviewIcon(int);

private:
    Ui::MainWindow *ui;

    /*!
     * \brief communication pointer to communication object
     * for sending comannds to the lights
     */
    std::shared_ptr<CommLayer> mComm;

    /*!
     * \brief data layer that maintains and tracks the states of the lights
     *  and the saved data of the GUI
     */
    std::shared_ptr<DataLayer> mData;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QPushButton*> > mPageButtons;

    /*!
     * \brief mPreviewIcon the data used by the PreviewButton. This gets updated
     * whenever the lighting mode changes or when a single LED routine changes hues.
     */
    IconData mPreviewIcon;
    /*!
     * \brief mSinglePageIcon the icon data used by the SinglePageButton. This icon
     * changes when a single LED routine changes hues.
     */
    IconData mSinglePageIcon;
    /*!
     * \brief mCustomArrayIcon the icon data used by the customArrayPageButton. This icon
     * changes when any of its colors change hue.
     */
    IconData mCustomArrayIcon;
    /*!
     * \brief mPresetArrayIcon the icon data used by the presetArrayPageButton. This icon
     * changes when any of its colors change hue.
     */
    IconData mPresetArrayIcon;

protected:
    /*!
     * \brief paintEvent called whenever there is a paint update. This is used
     *        to draw the dark grey background, since using the stylesheet
     *        for backgrounds makes some lesser features of GUI elements break.
     *
     * \param event event triggered that requires a repaint
     */
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
