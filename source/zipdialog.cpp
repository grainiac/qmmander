/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This is a dialog with some zip-option  which is shown before the
** file compression process.
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
#include "zipdialog.h"
#include "ui_zipdialog.h"
#include "archiveinfo.h"

#include <QFileDialog>

ZipDialog::ZipDialog(QWidget* pParent, ArchiveInfo* pAI)
:   QDialog(pParent),
    ui(new Ui::ZipDialog),
    m_pArchiveInfo(pAI)
{
    ui->setupUi(this);

    ui->labelFileCount->setText(QString("Zip %1 file(s) into archive").arg(m_pArchiveInfo->m_selectedFiles.count()));
    WinFileInfo fi=m_pArchiveInfo->m_selectedFiles.at(0);
    QString archiveName=QString("%1\\%2.zip").arg(m_pArchiveInfo->m_to).arg(fi.fileName());
    ui->lineEditDestination->setText(archiveName);
    ui->lineEditFilter->setText(m_pArchiveInfo->m_filter);    
    ui->checkBoxRecursiveSubdirs->setChecked(m_pArchiveInfo->m_zipFlags & ZipRecursive);

    Qt::WindowFlags windowFlags=this->windowFlags() & ~(Qt::WindowSystemMenuHint |
                                                        Qt::WindowMinMaxButtonsHint |
                                                        Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags);

    connect(ui->pushButtonArchive, SIGNAL(clicked()), this, SLOT(selectArchive()));
}

ZipDialog::~ZipDialog()
{
    delete ui;
}

void ZipDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ZipDialog::accept()
{
    QDialog::accept();
    m_pArchiveInfo->m_to=ui->lineEditDestination->text();
    m_pArchiveInfo->m_filter=ui->lineEditFilter->text();
    int iFlags=0;
    if(ui->checkBoxRecursiveSubdirs->checkState())
        iFlags|=ZipRecursive;
    if(ui->checkBoxMoveToArchive->checkState())
        iFlags|=ZipMove2Archive;
    if(ui->checkBoxSeparateArchives->checkState())
        iFlags|=ZipSeparateArchives;
     m_pArchiveInfo->m_zipFlags=QFlag(iFlags);
}

void ZipDialog::selectDestination()
{
    if(ui->checkBoxSeparateArchives->isChecked())
        selectDirectory();
    else
        selectArchive();
}

void ZipDialog::selectArchive()
{
    QFileDialog* pDlg=new QFileDialog(this, tr("Select an archive"), m_pArchiveInfo->m_to);
    pDlg->setFileMode(QFileDialog::AnyFile);
    pDlg->setNameFilter(tr("zip archives (*.zip)"));
    pDlg->setAcceptMode(QFileDialog::AcceptOpen);
    if(pDlg->exec()==QDialog::Accepted)
    {
        QStringList selectedFiles=pDlg->selectedFiles();
        if(selectedFiles.count())
        {
            QString archiveFilePath=selectedFiles.at(0);
            QFileInfo fi(archiveFilePath);
            archiveFilePath.left(archiveFilePath.lastIndexOf(fi.suffix())-1);
            archiveFilePath+=".zip";
            ui->lineEditDestination->setText(archiveFilePath);
        }
    }
}

void ZipDialog::selectDirectory()
{
    QFileDialog* pDlg=new QFileDialog(this, tr("Select a directory"), m_pArchiveInfo->m_to);
    pDlg->setFileMode(QFileDialog::Directory);
    pDlg->setAcceptMode(QFileDialog::AcceptOpen);
    if(pDlg->exec()==QDialog::Accepted)
    {
        QStringList selectedFiles=pDlg->selectedFiles();
        if(selectedFiles.count())
        {
            QString strPath=selectedFiles.at(0);
            strPath=strPath.replace('/',"\\");
            ui->lineEditDestination->setText(strPath);
        }
    }
}
