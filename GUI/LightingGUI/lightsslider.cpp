/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "lightsslider.h"

#include <QtCore>
#include <QtGui>
#include <QStyleOption>


LightsSlider::LightsSlider(QWidget *parent) : QWidget(parent) {

    mHeightScaleFactor = 1.0f;

    // --------------
    // Setup Thrrole Timer
    // --------------
    mThrottleTimer = new QTimer(this);
    connect(mThrottleTimer, SIGNAL(timeout()), this, SLOT(resetThrottleFlag()));

    this->setAutoFillBackground(true);
    slider = std::shared_ptr<QSlider>(new QSlider(Qt::Horizontal, this));
    slider->setAutoFillBackground(true);
    slider->setFixedSize(0,0);
    setMinimumPossible(false, 0);
    setSnapToNearestTick(false);
    connect(slider.get(), SIGNAL(valueChanged(int)), this, SLOT(receivedValue(int)));
    connect(slider.get(), SIGNAL(sliderReleased()),  this, SLOT(releasedSlider()));

    mLayout = new QVBoxLayout;
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0,0,0,0);
    mLayout->addWidget(slider.get());
    setLayout(mLayout);
    mSliderColorSet = false;
}


void LightsSlider::setSliderColorBackground(QColor color) {
    mSliderColor = color;
    mSliderColorSet = true;
    // compute a darker version for our gradient
    QColor darkColor = QColor((uint8_t)(color.red()   / 4),
                              (uint8_t)(color.green() / 4),
                              (uint8_t)(color.blue()  / 4));

    // slider handle is only controllable via stylesheets but the values needed for style sheets
    // breaks in some environments (such as high pixel density android screens). to get around this,
    // we always set the handle size programmatically whenever we udpate the stylesheet.
    int sliderHandleSize = (int)std::min(this->size().width() / 12.0f, (float)slider->size().height());

    // generate a stylesheet based off of the color with a gradient
    QString styleSheetString = QString("QSlider::sub-page:horizontal{ "
                                       " background:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 rgb(%1, %2, %3), stop: 1 rgb(%4, %5, %6));"
                                       " background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1, stop: 0 rgb(%1, %2, %3), stop: 1 rgb(%4, %5, %6));"
                                       "}"
                                       "QSlider::handle:horizontal {"
                                        "width: %7px;"
                                        "}"
                                       ).arg(QString::number(darkColor.red()),
                                                QString::number(darkColor.green()),
                                                QString::number(darkColor.blue()),
                                                QString::number(color.red()),
                                                QString::number(color.green()),
                                                QString::number(color.blue()),
                                                QString::number(sliderHandleSize));
    slider->setStyleSheet(styleSheetString);
}


void LightsSlider::receivedValue(int value) {
    value = jumpSliderToPosition(slider, value);
    slider->blockSignals(true);
    slider->setValue(value);
    if (!mThrottleFlag) {
        emit valueChanged(value);
        mThrottleFlag = true;
    }

    slider->blockSignals(false);
}

/*!
 * solution based on this stack overflow response:
 * http://stackoverflow.com/a/15321654
 */
int LightsSlider::jumpSliderToPosition(std::shared_ptr<QSlider> slider, int newPos) {

    Qt::MouseButtons btns = QApplication::mouseButtons();
    QPoint localMousePos = slider->mapFromGlobal(QCursor::pos());
    // check if a click happens directly on the slider
    bool clickOnSlider = (btns & Qt::LeftButton)
                          && (localMousePos.x() >= 0)
                          && (localMousePos.y() >= 0)
                          && (localMousePos.x() < slider->size().width())
                          && (localMousePos.y() < slider->size().height());

    // if its a click on the slider, use our custom logic.
    if (clickOnSlider) {
        // calculate how far from the left the click on the slider is.
        float posRatio = localMousePos.x() / (float)slider->size().width();
        int sliderRange = slider->maximum() - slider->minimum();
        // update newPos to our new value
        newPos = slider->minimum() + sliderRange * posRatio;
    }

    // check if snapping is enabled, and snap if necessary
    if (mShouldSnap) {
        newPos = snapSliderToNearestTick(slider, newPos);
    }
    // check if minimum possible is enabled, and update if necessary
    if (mUseMinimumPossible && (newPos < mMinimumPossible)) {
        newPos = mMinimumPossible;
    }
    return newPos;
}


int LightsSlider::snapSliderToNearestTick(std::shared_ptr<QSlider> slider, int pos) {
    int numberOfFullTicks = pos / slider->tickInterval();
    int leftTick = slider->minimum() + numberOfFullTicks * slider->tickInterval();
    int rightTick = slider->minimum()  + (numberOfFullTicks + 1) * slider->tickInterval();
    if ((leftTick - pos) < (pos - rightTick)) {
        pos = rightTick;
    } else {
        pos = leftTick;
    }
    return pos;
}


void LightsSlider::setMinimumPossible(bool useMinimumPossible, int minimumPossible) {
    mUseMinimumPossible = useMinimumPossible;
    mMinimumPossible = minimumPossible;
    if (mUseMinimumPossible && (slider->value() < mMinimumPossible)) {
        jumpSliderToPosition(slider, mMinimumPossible);
    }
}


void LightsSlider::resizeEvent(QResizeEvent *event) {
    Q_UNUSED (event);
    slider->setFixedSize(this->rect().width(), this->rect().height() * mHeightScaleFactor);
    float newY = this->rect().height() * (1.0 - mHeightScaleFactor) / 2.0f;
    slider->setGeometry(slider->rect().x(),
                        newY,
                        this->rect().width(),
                        this->rect().height() * mHeightScaleFactor);
    if (mSliderColorSet) {
        setSliderColorBackground(mSliderColor);
    }

}

void LightsSlider::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    mThrottleTimer->start(200);
}

void LightsSlider::hideEvent(QHideEvent *event) {
    Q_UNUSED(event);
    mThrottleTimer->stop();
    mThrottleFlag = false;
}


void LightsSlider::paintEvent(QPaintEvent *event) {
    Q_UNUSED (event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(255, 255, 255, 60));
    painter.fillRect(this->rect(), QBrush(QColor(48, 47, 47)));

    // draw tick marks
    // do this manually because they are very badly behaved with style sheets
    int interval = slider->tickInterval();
    if (interval == 0) {
        interval = slider->pageStep();
    }

    if (slider->tickPosition() != QSlider::NoTicks) {
        for (int i = slider->minimum(); i <= slider->maximum(); i += interval) {
            int x = round((double)(((double)(i - slider->minimum()) / (slider->maximum() - slider->minimum())) * (slider->width()))) - 1;
            int h = 2;
            if (slider->tickPosition() == QSlider::TicksBothSides || slider->tickPosition() == QSlider::TicksAbove) {
                int y = slider->rect().top();
                painter.drawLine(x, y, x, y / 2 + h);
            }
            if (slider->tickPosition() == QSlider::TicksBothSides || slider->tickPosition() == QSlider::TicksBelow) {
                int y = this->rect().bottom();
                painter.drawLine(x, y, x, y / 2 - h);
            }
        }
    }
}


void LightsSlider::setSliderHeight(float percent) {
    mHeightScaleFactor = percent;
    float newY = this->rect().height() * (1.0 - mHeightScaleFactor) / 2.0f;
    slider->setGeometry(slider->rect().x(),
                        newY,
                        this->rect().width(),
                        this->rect().height() * mHeightScaleFactor);
}


void LightsSlider::setSnapToNearestTick(bool shouldSnap) {
    mShouldSnap = shouldSnap;
}


void LightsSlider::resetThrottleFlag() {
    mThrottleFlag = false;
}


void LightsSlider::releasedSlider() {
    emit valueChanged(slider->value());
}
