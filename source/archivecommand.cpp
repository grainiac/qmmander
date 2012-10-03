/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for (un)zipping files.
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
#include "archivecommand.h"
#include "archiveinfo.h"
#include "FileExplorer.h"
#include "mainwindow.h"
#include "unzipdialog.h"
#include "zipdialog.h"
#include "zipunzipprogressdialog.h"
#include <memory>
#include <QMessageBox>

ArchiveCommand::ArchiveCommand(bool mode)
:   mode_(mode),
    archiveInfo_(0),
    progressDlg_(0),
    mainWindow_(const_cast<MainWindow*>(MainWindow::getMainWindow()))
{
}

void ArchiveCommand::execute()
{
    if(mode_)
        zipMode();
    else
        unzipMode();
}

void ArchiveCommand::zipMode()
{
    archiveInfo_=getZipUnzipFilesAndDestination(true);

    if(archiveInfo_)
    {
        QSharedPointer<ZipDialog> zipDlg(new ZipDialog(mainWindow_, archiveInfo_));
        if(zipDlg->exec()==QDialog::Accepted)
           doArchiveOperation();
        else
            emit(archiveCommandFinished(this));
    }
}

void ArchiveCommand::unzipMode()
{
    archiveInfo_=getZipUnzipFilesAndDestination(false);

    if(archiveInfo_)
    {
        QSharedPointer<UnzipDialog> unzipDlg(new UnzipDialog(mainWindow_, archiveInfo_));
        if(unzipDlg->exec()==QDialog::Accepted)
           doArchiveOperation();
        else
            emit(archiveCommandFinished(this));
    }
}

ArchiveInfo* ArchiveCommand::getZipUnzipFilesAndDestination(bool trueZipModeFalseUnzipMode)
{
    ArchiveInfo* archiveInfo=new ArchiveInfo(trueZipModeFalseUnzipMode);
    archiveInfo->m_from=mainWindow_->getActiveExplorer()->getSelectedPath();
    archiveInfo->m_to=mainWindow_->getInactiveExplorer()->getSelectedPath();
    archiveInfo->m_selectedFiles=mainWindow_->getActiveExplorer()->getSelectedFiles();

    if(archiveInfo && archiveInfo->m_selectedFiles.count()==0)
    {
        QMessageBox::warning(mainWindow_,
                             QObject::tr("No files selected"),
                             QObject::tr("Please select some files!"));
        delete archiveInfo;
        archiveInfo=0;
    }
    return archiveInfo;
}

void ArchiveCommand::doArchiveOperation()
{
    progressDlg_=new ZipUnzipProgressDialog(mainWindow_, archiveInfo_);
    progressDlg_->setAttribute(Qt::WA_DeleteOnClose);
    progressDlg_->show();
    connect(progressDlg_, SIGNAL(finished()), this, SLOT(ZipUnzipOperationFinished()));
}

void ArchiveCommand::ZipUnzipOperationFinished()
{
    delete progressDlg_;
    mainWindow_->getActiveExplorer()->refresh();
    mainWindow_->getInactiveExplorer()->refresh();
    emit(archiveCommandFinished(this));
}

ArchiveCommand* ArchiveCommand::archiveCommandForCompression()
{
    return new ArchiveCommand(true);
}

ArchiveCommand* ArchiveCommand::archiveCommandForDecompression()
{
    return new ArchiveCommand(false);
}

