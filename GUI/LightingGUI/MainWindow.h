
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "lightscontrol.h"
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
    LightsControl *LEDs;

/*!
 * These slots change pages or change the states
 * of the top menu
 */
public slots:
    void changeToSingleColor();
    void changeToMultiColor();
    void changeToSavedColors();
    void changeToSettings();
    void brightnessChanged(int newBrightness);
    void toggleOnOff();
    void updateSingleColor(int r, int g, int b);
    void updateToArrayColors();
    void updateToMultiColors();

private:
    Ui::MainWindow *ui;
    int mBrightness;
    bool mIsOn;

    IconData onButtonData;
    IconData offButtonData;
    IconData singleButtonData;
    IconData arrayButtonData;

protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
