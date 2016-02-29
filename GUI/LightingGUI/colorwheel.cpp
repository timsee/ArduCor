/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "colorwheel.h"

#include <QDebug>

ColorWheel::ColorWheel(QWidget* parent) {
    Q_UNUSED(parent);

}

ColorWheel::~ColorWheel() {

}

void ColorWheel::mouseMoveEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    //qDebug() << "mouse moved";
}

void ColorWheel::mousePressEvent(QMouseEvent* event) {
    // solve for potential deadspace since the qLabel expands horizontally to fit in its
    // container if needed. So this enforces that mouse press events only trigger if they are
    // directly above the circle.
    int deadZoneLeft = this->width() / 2 - this->height() / 2;
    int deadZoneRight = this->width() / 2 + this->height() / 2;
    if (event->pos().x() > deadZoneLeft && event->pos().x() < deadZoneRight) {
        // map the raw x and y position of the mouse press events to the pixmap's dimensions
        int x = map(event->pos().x(), deadZoneLeft, deadZoneRight, 0, this->pixmap()->width());
        int y = map(event->pos().y(), 0, this->height(), 0, this->pixmap()->height());

        // grab the color of the pixel that the user has clicked
        QColor color = QColor(this->pixmap()->toImage().pixel((int)x, (int)y));

        // add deadzone in the center so turning off lights in possible with the center
        int centerDeadzone = 10;
        if (color.red() < centerDeadzone && color.green() < centerDeadzone && color.blue() < centerDeadzone) {
            emit colorWheelUpdate(0, 0, 0);
        } else {
            // if the values are outside of the deadzone, emit them
            emit colorWheelUpdate(color.red(), color.green(), color.blue());
        }
    }
}

void ColorWheel::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
   // qDebug() << "mouse released";
}

int ColorWheel::map(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
