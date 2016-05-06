
#ifndef LIGHTSSLIDER_H
#define LIGHTSSLIDER_H

#include "datalayer.h"

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <memory>

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
    /*!
     * \brief Constructor
     */
    explicit LightsSlider(QWidget *parent = 0);

    /*!
     * \brief slider The actual and factual QSlider in this slider class
     */
    std::shared_ptr<QSlider> slider;
    /*!
     * \brief label label for the slider, allows you to label the slider, but by default its an empty value.
     */
    QLabel *label;

    /*!
     * \brief setSliderColorBackground Does a dark to light gradient on the color provided on the background
     *        of the slider to the left of the thumb piece of the slider Uses a custom style sheet to achieve this effect.
     * \param color the color that will be put into a custom style sheet
     */
    void setSliderColorBackground(QColor color);

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

signals:
    /*!
     * \brief Sends out the value that the slider has been set to after all the processing of snapping
     *        it to a tick or accounting for its minium value or any of that.
     */
    void valueChanged(int);

private slots:
    /*!
     * \brief receivedValue called whenever the QSlider signals valueChanged().
     */
    void receivedValue(int);

protected:
    /*!
     * \brief resizeEvent makes sure that the QSlider resizes with the QWidget
     */
    void resizeEvent(QResizeEvent *);
    /*!
     * \brief paintEvent handles edge cases created by the having a QSlider and
     *        an elaborate stylesheet. Draws the application's background color
     *        and the slider's ticks.
     */
    void paintEvent(QPaintEvent *);

private:
    /*!
     * \brief mShouldSnap true if the slider should snap to the nearest ticks, false
     *        if it should use the value specified by the user.
     */
    bool mShouldSnap;
    /*!
     * \brief mUseMinimumPossible true if the slider should have a minimum possible value it
     *        can be set to, which must be above its actual minimum. False otherwise.
     */
    bool mUseMinimumPossible;
    /*!
     * \brief mMinimumPossible true if it should use the minimum possible setting,
     *        false otherwise.
     */
    int mMinimumPossible;

    /*!
     * \brief Makes it so that by default, the slider jumps to the position the user
     *        clicks instead of jumping up by a page value. This gives the slider a more
     *        "mobile like" experience than the standard QSlider.
     *
     * solution based on this stack overflow response:
     * http://stackoverflow.com/a/15321654
     *
     * \param slider the slider that was clicked
     * \param newPos the position that user clicked.
     * \return the new position of the slider.
     */
    int jumpSliderToPosition(std::shared_ptr<QSlider> slider, int newPos);
    /*!
     * \brief snapSliderToNearestTick checks the value that the slider is
     *        is getting changed to and, if its not exactly over one of the
     *        sliders ticks, it'll update the value to the value of the closest
     *        tick. This provides extra logic to jumpSliderToPosition, but only
     *        if its enabled by setSnapToNearestTick().
     *
     * \param slider the internal slider
     * \param pos the position determined by jumpSliderToPosition.
     * \return the final position of the slider.
     */
    int snapSliderToNearestTick(std::shared_ptr<QSlider> slider, int pos);
};

#endif // LIGHTSSLIDER_H
