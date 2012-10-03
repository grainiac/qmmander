/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for changing file dates/times.
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
#include "changefiledatetimecommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "attributesdatedialog.h"
#include "attributesanddatechangerthread.h"

ChangeFileDateTimeCommand::ChangeFileDateTimeCommand()
{
}

void ChangeFileDateTimeCommand::execute()
{
    MainWindow* mainWindow=const_cast<MainWindow*>(MainWindow::getMainWindow());
    WinFileInfoList files=mainWindow->getActiveExplorer()->getSelectedFiles();
    if(files.count())
    {
        AttributesDateDialog dlg(files, mainWindow);
        if (dlg.exec()==QDialog::Accepted)
        {
            QProgressDialog* progressDialog=new QProgressDialog(tr("Adjusting file attributes and file dates"),
                                                                tr("Cancel"),
                                                                0,
                                                                files.count());

            AttributesAndDateChangerThread* thread=new AttributesAndDateChangerThread(files,
                                                                                      dlg.getFileProperties(),
                                                                                      *progressDialog,
                                                                                      mainWindow);

            connect(thread, SIGNAL(operationFinished(AttributesAndDateChangerThread*)), this, SLOT(attributesAndDateThreadFinished(AttributesAndDateChangerThread*)));
            connect(thread, SIGNAL(processMessage(QString)), progressDialog, SLOT(setLabelText(QString)));
            connect(thread, SIGNAL(fileRangeMessage(int,int)), progressDialog, SLOT(setRange(int,int)));
            connect(progressDialog, SIGNAL(canceled()), thread, SLOT(cancelOperation()));

            progressDialog->show();
            thread->start();
        }
    }
}

void ChangeFileDateTimeCommand::attributesAndDateThreadFinished(AttributesAndDateChangerThread* thread)
{
    MainWindow* mainWindow=const_cast<MainWindow*>(MainWindow::getMainWindow());
    mainWindow->actionRefresh();
    QProgressDialog* dlg=const_cast<QProgressDialog*>(&thread->getProgressDialog());
    dlg->done(0);
    delete dlg;
    delete thread;
    emit(changeFileDateTimeCommandFinished(this));
}

ChangeFileDateTimeCommand* ChangeFileDateTimeCommand::changeFileDateTimeCommandForActualSelection()
{
    return new ChangeFileDateTimeCommand();
}
