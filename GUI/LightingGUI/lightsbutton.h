#ifndef LIGHTSBUTTON_H
#define LIGHTSBUTTON_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include "datalayer.h"
#include "icondata.h"
#include "lightingprotocols.h"

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The LightsButton class provides a QPushbutton with some extra
 * logic to generate its own IconData and custom signals.
 *
 * It can also be configured as a `presetButton`, which stores a ELightingMode and
 * EColorPreset in the button and emits them as a signal whenever its clicked.
 *
 */
class LightsButton : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     */
    explicit LightsButton(QWidget *parent = 0);

    /*!
     * \brief setupAsPresetButton sets up the button for use with the PresetArrayPage. Assigns
     *        it a mode, a preset, and an icon. Whenever the button is clicked it will emit
     *        presetClicked and will send its mode and preset in that signal.
     * \param mode the ELightingMode that emits when this button is pushed.
     * \param preset The EColorPreset that emits when this button is pushed.
     * \param dataLayer used to create the icon for the button
     */
    void setupAsPresetButton(ELightingMode mode, EColorPreset preset, std::shared_ptr<DataLayer> dataLayer);

    /*!
     * \brief lightingMode the ELightingMode assigned to the button by setupAsPresetButton.
     * \return the button's lighting mode
     */
    ELightingMode lightingMode();

    /*!
     * \brief lightingMode the EColorPreset assigned to the button by setupAsPresetButton.
     * \return the button's preset
     */
    EColorPreset colorPreset();

    /*!
     * \brief button The QPushButton that this QWidget wraps.
     */
    QPushButton *button;

signals:
    /*!
     * \brief presetClicked sent only when setupAsPresetButton has been called.
     */
    void presetClicked(int, int);

private slots:
    /*!
     * \brief buttonClicked listens for a click on the button->
     */
    void buttonClicked();

protected:
    /*!
    * \brief resizeEvent used to resize the icon on the QPushButton.
    */
   virtual void resizeEvent(QResizeEvent *);

private:
    /*!
     * \brief mLayout layout of a lights button
     */
    QHBoxLayout *mLayout;

    /*!
     * \brief mIconData icon data used by the button's
     *        icon.
     */
    IconData mIconData;

    /*!
     * \brief mIsPresetButton true if setupAsPresetButton() is called, false otherwise.
     */
    bool mIsPresetButton;

    /*!
     * \brief mLightingMode stored lighting mode. can only be
     *        used if the button is set up as a presetButton.
     *        If it is a preset button, this will be emitted
     *        as a signal whenever the button is clicked.
     */
    ELightingMode mLightingMode;
    /*!
     * \brief mColorPreset stored color preset. can only be
     *        used if the button is set up as a presetButton.
     *        If it is a preset button, this will be emitted
     *        as a signal whenever the button is clicked.
     */
    EColorPreset mColorPreset;

};

#endif // LIGHTSBUTTON_H
