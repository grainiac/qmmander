/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for file drop events.
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
#include "dropcommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"
#include <QObject>
#include <QMessageBox>

DropCommand::DropCommand()
:   sourceFiles_(0),
    destinationDirectory_(0),
    destinationFiles_(0)
{
}

DropCommand::~DropCommand()
{
    delete [] sourceFiles_;
    delete [] destinationDirectory_;
    delete [] destinationFiles_;
}

void DropCommand::execute()
{
    ::SHFileOperationW(&st_);
    MainWindow::getMainWindow()->getActiveExplorer()->refresh();
    MainWindow::getMainWindow()->getInactiveExplorer()->refresh();
}

DropCommand* DropCommand::dropCommandForActualSelection(QList<QUrl> files, QString to, Qt::DropAction action)
{
    MainWindow* mainWindow=const_cast<MainWindow*>(MainWindow::getMainWindow());
    DropCommand* dc=new DropCommand();

    QString dialogTitle   = (action==Qt::CopyAction) ? QObject::tr("Copy files?") : QObject::tr("Move files?");
    QString dialogMessage = (action==Qt::CopyAction) ? QObject::tr("Files will be copied to %1.").arg(to) : QObject::tr("Files will be moved to %1.").arg(to);
    int result = QMessageBox::question(mainWindow, dialogTitle, dialogMessage, QMessageBox::Ok | QMessageBox::Cancel);

    if(result==QMessageBox::Ok)
    {
        QStringList listFiles;
        for(int i=0; i<files.count(); i++)
        {
            QUrl url=files.at(i);
            QString cleanPath=url.toString();
            if(cleanPath.contains("file:///"))
                cleanPath.remove(0,8);
            if(cleanPath.at(0)=='/')
                cleanPath.remove(0,1);
            listFiles.push_back(cleanPath);
        }

        dc->sourceFiles_ = getSHFileOperationFromString(listFiles);
        dc->st_.hwnd=MainWindow::getMainWindow()->winId();

        if(action==Qt::CopyAction)
        {
            dc->destinationDirectory_ = getSHFileOperationCopyToString(to);
            dc->st_.pTo=dc->destinationDirectory_;
            dc->st_.wFunc=FO_COPY;
            dc->st_.fFlags = 0x00000000;
        }
        else // Move
        {
            dc->destinationFiles_ = getSHFileOperationMoveToString(listFiles, to);
            dc->st_.pTo=dc->destinationFiles_;
            dc->st_.wFunc=FO_MOVE;
            dc->st_.fFlags=FOF_MULTIDESTFILES | FOF_ALLOWUNDO | FOF_RENAMEONCOLLISION;
        }
        dc->st_.pFrom=dc->sourceFiles_;
    }
    return dc;
}
