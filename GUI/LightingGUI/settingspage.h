
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
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The SettingsPage class provides a way to configure
 * the application settings.
 *
 * It currently provides the ability to change the speed the LEDs update
 * and the amount of minutes it takes for the LEDs to timeout.
 *
 * It also shows a list of the available serial connections
 * and clicking  on a serial connection will attempt to connect
 * to it.
 *
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
