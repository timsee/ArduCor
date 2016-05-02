
#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "lightingpage.h"
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
 * \brief The SettingsPage provides a way to configure the
 * application settings.
 *
 * It currently provides the ability to change the speed the LEDs update
 * and the amount of minutes it takes for the LEDs to timeout.
 *
 * It also shows a list of the available serial connections
 * and clicking  on a serial connection will attempt to connect
 * to it.
 *
 */
class SettingsPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     */
    explicit SettingsPage(QWidget *parent = 0);
    /*!
     * \brief Deconstructor
     */
    ~SettingsPage();

public slots:
    /*!
     * \brief speedChanged signaled whenever the slider that controls
     *        the LEDs speed changes its value.
     */
    void speedChanged(int);
    /*!
     * \brief timeoutChanged signaled whenever the slider that controls
     *        the LEDs idle time out changes its value.
     */
    void timeoutChanged(int);
    /*!
     * \brief listClicked signaled whenever the serial list is clicked. It
     *        attempts to connect to the serial device that is clicked, if
     *        its not already connected.
     */
    void listClicked(QListWidgetItem *);

protected:
    /*!
     * \brief showEvent called before the this page is shown. Used to sync up
     *        any changes that may have happened on other pages.
     */
    void showEvent(QShowEvent *);

private:
    /*!
     * \brief ui pointer to Qt UI form.
     */
    Ui::SettingsPage *ui;

    /*!
     * \brief mFirstLoad temporary hack while loading the list of connected
     *        serial devices. Allows the first time loading the serial list
     *        to have unique behavior.
     */
    bool mFirstLoad;

    /*!
     * \brief updateSerialList updates the serial list based on the information
     *        found in the CommLayer. Does not search for new serial devices.
     */
    void updateSerialList();
};

#endif // SETTINGSPAGE_H
