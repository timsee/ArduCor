/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "lightscontrol.h"
#include "lightsslider.h"

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class SettingsPage;
}

/*!
 * \brief The SettingsPage class allows you
 * to change the timeout or speed of the LEDs
 * It also shows a list of the available serial connections
 * and clicking  on a serial connection will attempt to connect
 * to it.
 */
class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = 0);
    ~SettingsPage();
    LightsControl *LEDs;

/*!
 * used to update the settings of the LED system
 */
public slots:
    void speedChanged(int newSpeed);
    void timeoutChanged(int newTimeout);
    void listClicked(QListWidgetItem* item);

protected:
    void showEvent(QShowEvent *);

private:
    Ui::SettingsPage *ui;
    int mSpeed;
    int mTimeout;
    bool mFirstLoad;


    void updateSerialList();
};

#endif // SETTINGSPAGE_H
