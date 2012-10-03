/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A thread which checks periodically if the number of drives has changed.
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
#include "drivecheckerthread.h"
#include "winfileinfo.h"
#include "mainwindow.h"

DriveCheckerThread::DriveCheckerThread(QObject* pParent)
:   QThread(pParent)
{
}

void DriveCheckerThread::run()
{
    forever
    {
        int iAppDriveCount=MainWindow::getMainWindow()->getDriveCount();
        while(WinFileInfo::getDriveList().count()==iAppDriveCount)
        {
            //qDebug(QString("DriveCheckerThread: Drive count hasn't changed. Wait 3 seconds....").toStdString().c_str());
            usleep(3000000); // sleep for 3 seconds
        }
        //qDebug(QString("DriveCheckerThread: Drive count has changed!").toStdString().c_str());
        emit(driveCountChanged());
        usleep(3000000); // sleep for 3 seconds
    }
}
