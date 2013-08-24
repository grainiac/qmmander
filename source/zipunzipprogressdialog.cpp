/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This dialog shows the progress of  the zip/unzip threads.
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
#include "zipunzipprogressdialog.h"
#include "ui_zipunzipprogressdialog.h"
#include "archiveinfo.h"
#include "fileactiondialog.h"
#include "fileactionfactories.h"
#include "renamedialog.h"
#include "unzipthread.h"
#include "zipthread.h"
#include "zipunzipwatcherthread.h"

#include <QPushButton>

ZipUnzipProgressDialog::ZipUnzipProgressDialog(QWidget *parent, ArchiveInfo* archiveInfo)
:   QDialog(parent),
    m_bShown(false),
    m_archiveInfo(archiveInfo),
    ui(new Ui::ZipUnzipProgressDialog),
    m_pPauseButton(0),
    m_pUnzipper(0),
    m_unzipFileActionFactory(0),
    m_pZipper(0),
    m_zipFileActionFactory(0),
    m_pWatcher(0)
{
    ui->setupUi(this);
    m_pPauseButton=new QPushButton(tr("Pause"), this);
    m_pPauseButton->setCheckable(true);
    ui->buttonBox->addButton(m_pPauseButton,QDialogButtonBox::ApplyRole);

    ui->l11->setText(tr("Time elapsed:"));
    ui->l21->setText(tr("Remaining time:"));
    ui->l31->setText(tr("Files:"));

    ui->r11->setText(tr("Uncompressed size:"));
    ui->r21->setText(tr("Speed:"));
    ui->r31->setText(tr("Processed:"));

    Qt::WindowFlags windowFlags=this->windowFlags() & ~(Qt::WindowSystemMenuHint |
                                                        Qt::WindowMinMaxButtonsHint |
                                                        Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags);
    setDialogValues("","","","","","","","");

    initModeDependentStuff();

    m_pWatcher=new ZipUnzipWatcherThread(this, m_archiveInfo);
}

void ZipUnzipProgressDialog::initModeDependentStuff()
{
    m_archiveInfo->m_trueZipModeFalseUnzipMode
    ? initDialog4ZipOperations()
    : initDialog4UnzipOperations();
}

void ZipUnzipProgressDialog::initDialog4ZipOperations()
{
    setWindowTitle(tr("Preparing zip operation..."));
    m_zipFileActionFactory=new ZipFileActionFactory(m_archiveInfo);
    m_pZipper=new ZipThread(this, m_archiveInfo, m_zipFileActionFactory);
    connect(this->m_pPauseButton, SIGNAL(toggled(bool)), m_pZipper, SLOT(pause(bool)));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejectZip()));
}

void ZipUnzipProgressDialog::initDialog4UnzipOperations()
{
    setWindowTitle(tr("Preparing unzip operation..."));
    m_unzipFileActionFactory=new UnzipFileActionFactory(m_archiveInfo);
    m_pUnzipper=new UnzipThread(this, m_archiveInfo, m_unzipFileActionFactory);
    connect(this->m_pPauseButton, SIGNAL(toggled(bool)), m_pUnzipper, SLOT(pause(bool)));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(rejectUnzip()));
}

ZipUnzipProgressDialog::~ZipUnzipProgressDialog()
{    
    m_pWatcher->terminate();
    delete m_pWatcher;
    delete m_pUnzipper;
    delete m_unzipFileActionFactory;
    delete m_pZipper;
    delete m_zipFileActionFactory;
    delete m_archiveInfo;
    delete ui;
}

void ZipUnzipProgressDialog::workerThreadFinished()
{
    emit(finished());
}

QThread* ZipUnzipProgressDialog::workerThreadHandle()
{
    if(m_pUnzipper)
        return m_pUnzipper;
    else if(m_pZipper)
        return m_pZipper;
    return 0;
}

void ZipUnzipProgressDialog::rejectUnzip()
{
    m_pUnzipper->initializeTermination();
    while(!m_pUnzipper->isTerminationSet());
    m_pUnzipper->terminateThread();
}

void ZipUnzipProgressDialog::rejectZip()
{
    m_pZipper->initializeTermination();
    while(!m_pZipper->isTerminationSet());
    m_pZipper->terminateThread();
}

void ZipUnzipProgressDialog::ask4FileAction()
{    
    runFileActionDialog() == QDialog::Accepted
    ?   handleFileAction()
    :   rejectFileAction();
}

int ZipUnzipProgressDialog::runFileActionDialog()
{
    FileActionDialog* pDlg=new FileActionDialog(m_archiveInfo, m_fileAction);
    int iRet=pDlg->exec();
    delete pDlg;
    return iRet;
}

void ZipUnzipProgressDialog::handleFileAction()
{
    checkIfFileNeedToBeRenamed();
    m_archiveInfo->m_fileAction=m_fileAction;
    m_archiveInfo->m_isUserInputRequested=false;
}

void ZipUnzipProgressDialog::checkIfFileNeedToBeRenamed()
{
    if( (m_fileAction==FA_RenameDest) ||
        (m_fileAction==FA_RenameSource) )
    {
        m_archiveInfo->m_destFileName=renameFile(m_archiveInfo->m_destFileName);
    }
}

QString ZipUnzipProgressDialog::renameFile(QString oldName)
{
    QString newName;
    do
    {
        newName=runRenameDialog(oldName);        
    }
    while(newName==oldName && newName!="");
    return newName;
}

QString ZipUnzipProgressDialog::runRenameDialog(QString oldName)
{
    QFileInfo fileInfo(oldName);
    RenameDialog dlg(fileInfo);
    if(dlg.exec()==QDialog::Accepted)
    {
        QString newName=QString("%1/%2").arg(fileInfo.path()).arg(dlg.getFileName());
        return newName;
    }
    return "";
}

void ZipUnzipProgressDialog::rejectFileAction()
{
    m_archiveInfo->m_trueZipModeFalseUnzipMode
    ?    rejectZip()
    :    rejectUnzip();
}

void ZipUnzipProgressDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);        
        break;
    default:
        break;
    }
}
void ZipUnzipProgressDialog::paintEvent(QPaintEvent* e)
{
    QWidget::paintEvent(e);
    if(!m_bShown)
    {
        m_bShown=true;
        if(m_pUnzipper)
            m_pUnzipper->start();
        else if(m_pZipper)
            m_pZipper->start();
        m_pWatcher->start();
    }
}

void ZipUnzipProgressDialog::updateOnFileUnzipped()
{
    if(m_bShown)
        update();
}

void ZipUnzipProgressDialog::update()
{
    qreal dProcessed=static_cast<qreal>(m_archiveInfo->m_bytesProcessed)/static_cast<qreal>(1024);
    qreal dTotal=static_cast<qreal>(m_archiveInfo->m_totalSize)/static_cast<qreal>(1024);
    qreal dVal=dProcessed/dTotal*100;
    int iVal=static_cast<int>(dVal+.5);
    ui->progressBar->setValue(iVal);

    QString strTitle;
    if(m_archiveInfo->m_trueZipModeFalseUnzipMode)
        strTitle=QString("%1% finished...zipping files to archive %2").arg(iVal).arg(m_archiveInfo->m_to);
    else
        strTitle=QString("%1% finished...unzipping files from archive %2").arg(iVal).arg(m_archiveInfo->m_actualArchive);
    setWindowTitle(strTitle);

    QString strActFileTotalFiles=QString("%1/%2").arg(m_archiveInfo->m_filesProcessed).arg(m_archiveInfo->m_fileCount);
    QString strTotalSize=QString("%1 MiB").arg(static_cast<qreal>(m_archiveInfo->m_totalSize)/static_cast<qreal>(1024*1024), 7, 'f', 2);
    QString strBytesProcessed=QString("%1 MiB").arg(static_cast<qreal>(m_archiveInfo->m_bytesProcessed)/static_cast<qreal>(1024*1024), 7, 'f', 2);

    setDialogValues(m_archiveInfo->m_timeElapsed, m_archiveInfo->m_timeRemaining, strActFileTotalFiles,
                    strTotalSize, m_archiveInfo->m_speed, strBytesProcessed,
                    m_archiveInfo->m_path2ActualFile, m_archiveInfo->m_actualFileName);

}

void ZipUnzipProgressDialog::setDialogValues(QString strTimeElapsed, QString strTimeRemaining, QString strFiles,
                                             QString strTotalSize,   QString strSpeed, QString strProcessed,
                                             QString strPath2File,   QString strFileName)
{
    ui->l12->setText(strTimeElapsed);
    ui->l22->setText(strTimeRemaining);
    ui->l32->setText(strFiles);

    ui->r12->setText(strTotalSize);
    ui->r22->setText(strSpeed);
    ui->r32->setText(strProcessed);

    ui->labelPath->setText(strPath2File);
    ui->labelFile->setText(strFileName);
}
