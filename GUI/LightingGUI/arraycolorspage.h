

#ifndef ARRAYCOLORSPAGE_H
#define ARRAYCOLORSPAGE_H

#include "lightingpage.h"
#include <QWidget>
#include <QDebug>
#include <QToolButton>

namespace Ui {
class ArrayColorsPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The ArrayColorsPage class uses the color array from the arduino's RoutinesRGB library.
 *
 * The bar at the top of the page determines how many of the colors from the array to use. For example,
 * when its set to `2` it will use only the first `2` colors. Setting it to `5` will use `5`
 * array colors, etc.
 *
 * \todo Add the ability to set each array color individually.
 *
 */
class ArrayColorsPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit ArrayColorsPage(QWidget *parent = 0);
    ~ArrayColorsPage();
    void highlightButton(ELightingMode lightingMode);


signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();

/*!
 * slots used to change the mode of the lights
 * or to change the settings of the different modes
 */
public slots:
    void modeChanged(int);
    void colorCountChanged(int);

protected:
    void showEvent(QShowEvent *event);

private:
    Ui::ArrayColorsPage *ui;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton*> > mPageButtons;

    void updateIcons();
};

#endif // ARRAYCOLORSPAGE_H
