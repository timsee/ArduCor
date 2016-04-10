
#ifndef MULTICOLORPAGE_H
#define MULTICOLORPAGE_H

#include "lightingpage.h"

#include <QWidget>
#include <QToolButton>


namespace Ui {
class MultiColorPage;
}

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The MultiColorPage class contains preprogrammed lighting routines
 * that use a variety of colors. These routines require no parameters
 * or settings and because of this, it's the simplest page in the
 * application: It's a grid of buttons.
 *
 */
class MultiColorPage : public QWidget, public LightingPage
{
    Q_OBJECT

public:
    explicit MultiColorPage(QWidget *parent = 0);
    ~MultiColorPage();
    void highlightModeButton(ELightingMode lightingMode);


signals:
    /*!
     * used to signal back to the main page that it should update its top-left icon
     * with a new color mode
     */
    void updateMainIcons();


public slots:
    void modeChanged(int);

protected:
    /*!
     * called whenever the page is shown on screen.
     */
    void showEvent(QShowEvent *);

private:
    Ui::MultiColorPage *ui;

    /*!
     * \brief mPageButtons pointers to all the main buttons, used
     * to iterate through them quickly.
     */
    std::shared_ptr<std::vector<QToolButton*> > mPageButtons;

};

#endif // MULTICOLORPAGE_H
