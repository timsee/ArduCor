
#ifndef PRESETARRAYPAGE_H
#define PRESETARRAYPAGE_H

#include "lightingpage.h"

#include <QWidget>
#include <QToolButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "lightsbutton.h"

namespace Ui {
class PresetArrayPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The PresetArrayPage provides a way to use the Color Presets
 *        from the arduino's RoutineRGB library.
 *
 * It contains a grid of buttons that map color presets to lighting
 * modes. The list expands horizontally into a QScrollArea.
 *
 */
class PresetArrayPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    /*!
     * \brief Constructor
     */
    explicit PresetArrayPage(QWidget *parent = 0);
    /*!
     * \brief Deconstructor
     */
    ~PresetArrayPage();
    /*!
     * \brief highlightModeButton highlights the button that implements
     *        the lightingMode parameter. If it can't find a button that
     *        implements lightingMode, then all buttons are unhighlighted
     * \param lightingMode the mode that the highlighted button implements.
     * \param preset the color preset that the highlighted button implements.
     */
    void highlightModeButton(ELightingMode mode, EColorPreset preset);
    /*!
     * \brief setupPresets sets up the preset buttons. Requires the DataLayer
     *        of the application to be set up first.
     */
    void setupPresets();

signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();


public slots:
    /*!
     * \brief presetButtonClicked every button setup as a presetButton will signal
     *        this slot whenever they are clicked.
     * \param mode the stored ELightingMode of the button cast to an int.
     * \param preset the stored EColorPreset of the button cast to an int.
     */
    void presetButtonClicked(int mode, int preset);

protected:
    /*!
     * called whenever the page is shown on screen.
     */
    void showEvent(QShowEvent *);

private:
    /*!
     * \brief ui pointer to Qt UI form.
     */
    Ui::PresetArrayPage *ui;

    /*!
     * \brief mButtonLayout layout used to put the buttons in the QScrollArea as a grid.
     */
    QGridLayout *mButtonLayout;
    /*!
     * \brief mPresetButtons Buttons used in the grid in the scroll area, each one signals
     *        a EColorPreset and a ELightingMode.
     */
    std::vector<LightsButton *> mPresetButtons;
    /*!
     * \brief mPresetLabels labels for the top of the columns of the preset buttons.
     */
    std::vector<QLabel *> mPresetLabels;

    /*!
     * \brief mButtonCount number of buttons used, equals the number of presets
     *        multiplied by the number of lighting routines that the presets can use.
     */
    int mButtonCount;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton*> > mPageButtons;

};

#endif // PRESETARRAYPAGE_H
