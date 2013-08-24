/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This dialog shows the progress of the zip/unzip threads.
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
#ifndef ZIPUNZIPPROGRESSDIALOG_H
#define ZIPUNZIPPROGRESSDIALOG_H

#include <QDialog>
#include "fileaction.h"

class QPushButton;
class QThread;
class ArchiveInfo;
class UnzipThread;
class UnzipFileActionFactory;
class ZipThread;
class ZipFileActionFactory;
class ZipUnzipWatcherThread;

namespace Ui
{
    class ZipUnzipProgressDialog;
}

class ZipUnzipProgressDialog : public QDialog
{
    Q_OBJECT
public:
    ZipUnzipProgressDialog(QWidget *parent,
                           ArchiveInfo * const archiveInfo);
    ~ZipUnzipProgressDialog();

public slots:    
    void update();
    void updateOnFileUnzipped();
    void rejectUnzip();
    void rejectZip();

signals:
    void finished();

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent* e);

private slots:    
    void ask4FileAction();
    int  runFileActionDialog();
    void checkIfFileNeedToBeRenamed();
    QString renameFile(QString oldName);
    QString runRenameDialog(QString oldName);
    void handleFileAction();
    void rejectFileAction();
    void workerThreadFinished();

private:
    void initModeDependentStuff();
    void initDialog4UnzipOperations();
    void initDialog4ZipOperations();

    friend class ZipUnzipWatcherThread;
    QThread* workerThreadHandle();

    void setDialogValues(QString strTimeElapsed, QString strTimeRemaining, QString strFiles,
                         QString strTotalSize,   QString strSpeed,         QString strProcessed,
                         QString strPath2File,   QString strFileName);

    bool m_bShown;
    ArchiveInfo * const m_archiveInfo;
    Ui::ZipUnzipProgressDialog *ui;
    QPushButton* m_pPauseButton;
    UnzipThread* m_pUnzipper;
    UnzipFileActionFactory* m_unzipFileActionFactory;
    ZipThread* m_pZipper;
    ZipFileActionFactory* m_zipFileActionFactory;
    ZipUnzipWatcherThread* m_pWatcher;
    FileAction m_fileAction;
};

#endif // ZIPUNZIPPROGRESSDIALOG_H
