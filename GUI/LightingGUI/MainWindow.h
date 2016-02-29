/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "lightscontrol.h"
#include "icondata.h"

namespace Ui {
class MainWindow;
}

/*!
 * \brief The MainWindow class is well, the main window.
 * It contains a QStackedWidget which shows the various
 * pages of the app, an on/off button in the top left,
 * and a brightness slider across the top.
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
