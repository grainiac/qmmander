/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Command which displays an About Qmmander Dialog.
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
#include "aboutcommand.h"
#include <QString>
#include <QMessageBox>
#include "mainwindow.h"
#include "version.h"

void AboutCommand::execute()
{
    QString html =QString("<html><body><table style=""text-align: left; width: 100%;"" border=""0"" cellspacing=""10""><tbody>")+
                  QString("<tr><td><h1><img src="":/images/images/qmmander24.png"">")+
                  QString("&nbsp;&nbsp;Qmmander</h1></td></tr>")+
                  QString("<tr><td colspan=""2"">Version ")+QString(VERSION_STRING)+QString("</td></tr>")+
                  QString("<tr><td colspan=""2"">")+QString(COPYRIGHT_STRING)+QString("</td></tr>")+
                  QString("<tr><td colspan=""2"">All rights reserved.</td></tr>")+
                  QString("<tr><td colspan=""2"">Qmmander project @code.google:</td></tr>")+
                  QString("<tr><td colspan=""2""><a href=\"http://code.google.com/p/qmmander/\">http://code.google.com/p/qmmander/</a></td></tr>")+
                  QString("</tbody></table></body></html>");

    QMessageBox msgBox(const_cast<MainWindow*>(MainWindow::getMainWindow()));
    msgBox.setStyleSheet("QMessageBox{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 gray, stop: 0.6 white)}");
    msgBox.addButton(QMessageBox::Ok);
    msgBox.setText(html);
    msgBox.exec();
}

AboutCommand * AboutCommand::getAboutCommand()
{
    return new AboutCommand();
}
