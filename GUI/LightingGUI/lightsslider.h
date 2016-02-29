/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef LIGHTSSLIDER_H
#define LIGHTSSLIDER_H

#include "datalayer.h"

#include <QWidget>
#include <QSlider>

/*!
 * \brief The LightsSlider class is mostly a QSlider, but we plan to do
 * some things out side of the context of just QSlider, so we're subclassing
 * QWidget instead.
 *
 * QSliders have their tick marks break when theres a
 * custom style sheet, so tick marks are drawn using QPaint, if needed.
 *
 * The slider also has some custom logic. Clicking on the slider jumps the value
 * to the location of your click instead of doing a tick towards it. Also, sliders
 * can be given a minimum value that they can't go below.
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
 * Sends out the value that the slider has been set to after all the processing of snapping it to a tick or
 * accounting for its minium value or any of that.
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
