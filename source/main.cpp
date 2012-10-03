/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** The main function of the Qmmander application.
**
** Qmmander is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**     
** Qmmander is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**    
** You should have received a copy of the GNU General Public License
** along with Qmmander.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#include <QtGui/QApplication>
#include <QSplashScreen>
#include <QPainter>
#include <QLocale>
#include <QTranslator>
#include "mainwindow.h"
#include "version.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/images/images/splash.png");
    QPixmap corner;
    QString version=VERSION_STRING;

    if(version.contains("pre"))
        corner=QPixmap(":/images/images/pre_corner.png");
    if(version.contains('a'))
        corner=QPixmap(":/images/images/alpha_corner.png");
    if(version.contains('b'))
        corner=QPixmap(":/images/images/beta_corner.png");
    if(version.contains("rc"))
        corner=QPixmap(":/images/images/rc_corner.png");

    QPainter painter(&pixmap);
    painter.drawPixmap(pixmap.width()-56, pixmap.height()-56, 56, 56, corner);
    painter.drawText(294, 77, // Left-upper corner of text rectangle
                     97, 20,  // size of text rectangle
                     Qt::AlignTop | Qt::AlignRight, // flags
                     QString("v%1").arg(VERSION_STRING));

    QSplashScreen* splash=new QSplashScreen(pixmap);
    splash->show();

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("Qmmander_") + locale);
    a.installTranslator(&translator);

    MainWindow w(splash);
    w.show();
    splash->finish(&w);
    return a.exec();             
}
