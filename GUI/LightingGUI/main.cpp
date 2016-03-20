/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */
#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // load the style sheet
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists()) {
        qDebug() << "Unable to set stylesheet, file not found\n";
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
