
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
 * \brief The PresetArrayPage class contains a grid of
 * buttons that map color presets to lighting modes. The
 * list expands horizontally into a QScrollArea.
 *
 */
class PresetArrayPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit PresetArrayPage(QWidget *parent = 0);
    ~PresetArrayPage();
    void highlightModeButton(ELightingMode mode, EColorPreset preset);
    void setupPresets();

signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();


public slots:
    void presetButtonClicked(int mode, int preset);

protected:
    /*!
     * called whenever the page is shown on screen.
     */
    void showEvent(QShowEvent *);

private:
    Ui::PresetArrayPage *ui;

    QGridLayout *mButtonLayout;
    std::vector<LightsButton *> mPresetButtons;
    std::vector<QLabel *> mPresetLabels;
    int mButtonCount;
    int mPresetMax;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton*> > mPageButtons;

};

#endif // PRESETARRAYPAGE_H
