
#ifndef LIGHTSSLIDER_H
#define LIGHTSSLIDER_H

#include "datalayer.h"

#include <QWidget>
#include <QSlider>

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The LightsSlider class provides a slider with behaviors similiar
 * to sliders on mobile devices. Clicking on the slider jumps the slider's value
 * to the location you clicked instead of the QSlider default of jumping by the page 
 * value. There is also the option to use a gradient instead of a solid color as the 
 * background color for the slider.
 *
 * QSliders can have their tick marks break when the project uses a custom style sheet,
 * so tick marks are drawn using QPaint, if needed.
 *
 * There is also an option for developers to set a value that above the slider's minimum 
 * as the "minmum possible." Users will see a slider that shows the minimum value, but they
 * won't below the value set as the minimum possible. This is used by the Array Colors Page 
 * in order to prevent users from choosing less than 2 colors for the array routines.
 *
 */
class LightsSlider : public QWidget
{
    Q_OBJECT

public:
    explicit LightsSlider(QWidget *parent = 0);

    /*!
     * \brief slider The actual and factual QSlider in this slider class
     */
    QSlider *slider;

    /*!
     * \brief setSliderColorBackground Does a dark to light gradient on the color provided on the background
     *        of the slider to the left of the thumb piece of the slider Uses a custom style sheet to achieve this effect.
     * \param color the color that will be put into a custom style sheet
     */
    void setSliderColorBackground(DataLayer::Color color);

    /*!
     * \brief setSnapToNearestTick set this if you want the slider to snap to the nearest tick instead
     *        of snapping to the direct location that the user clicked.
     * \param shouldSnap true if you want to it to snap, false otherwise.
     */
    void setSnapToNearestTick(bool shouldSnap);

    /*!
     * \brief setMinimumPossible allows the user to set a value other than the minimum as the minimum possible
     *        value the slider can hit.
     * \param useMinimumPossible tue if you want to use this new minimum, false otherwise.
     * \param minimumPossible the value that you want to be a minimum.
     */
    void setMinimumPossible(bool useMinimumPossible, int minimumPossible);


/*!
 * Sends out the value that the slider has been set to after all the processing of snapping 
 * it to a tick or accounting for its minium value or any of that.
 */
signals:
    void valueChanged(int);

public slots:
    void receivedValue(int);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool mShouldSnap;
    bool mUseMinimumPossible;
    int mMinimumPossible;
    /*!
     * solution based on this stack overflow response:
     * http://stackoverflow.com/a/15321654
     */
    int jumpSliderToPosition(QSlider *slider, int newPos);
    int snapSliderToNearestTick(QSlider *slider, int pos);
};

#endif // LIGHTSSLIDER_H
