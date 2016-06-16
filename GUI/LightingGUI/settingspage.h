
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
 * It provides the ability to change the speed the LEDs update
 * and the amount of minutes it takes for the LEDs to timeout.
 *
 * It also provides a way to switch between different communication types
 * such as Serial and UDP. This interface automatically populates serial
 * connections, but allows users to add and remove connections for UDP
 * and HTTP.
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

    /*!
     * \brief highlightButton highlight the commtype button of the desired ECommType
     * \param currentCommType the button that you want to highlight
     */
    void highlightButton(ECommType currentCommType);

    /*!
     * \brief commTypeSelected called when the comm type updates and changes
     */
    void commTypeSelected(int);

    /*!
     * \brief plusButtonClicked called whenever the plus button is clicked
     */
    void plusButtonClicked();

    /*!
     * \brief minusButtonClicked called whenever the minus button is clicked
     */
    void minusButtonClicked();

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
     * \brief updateConnectionList updates the GUI elements that display the
     *        CommLayer's connection list.
     */
    void updateConnectionList();

    /*!
     * \brief mCurrentListString name of the current comm list connection
     */
    QString mCurrentListString;

    /*!
     * \brief mSliderSpeedValue storage for the current slider value, which
     *        differs from the actual slider speed saved in the data layer.
     */
    int mSliderSpeedValue;

};

#endif // SETTINGSPAGE_H
