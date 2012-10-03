/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** This is a dialog with some unzip-option  which is shown before the
** uncompression process.
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
#include "unzipdialog.h"
#include "ui_unzipdialog.h"
#include "archiveinfo.h"

#include <QFileDialog>

UnzipDialog::UnzipDialog(QWidget* pParent, ArchiveInfo* pAI)
:   QDialog(pParent),
    ui(new Ui::UnzipDialog),
    m_pArchiveInfo(pAI)
{
    ui->setupUi(this);

    ui->lineEditDestination->setText(m_pArchiveInfo->m_to);
    ui->lineEditFilter->setText(m_pArchiveInfo->m_filter);
    ui->checkBoxUnpackPath->setChecked(m_pArchiveInfo->m_unzipFlags & UnzipPath);
    ui->checkBoxSeparateDirs->setChecked(m_pArchiveInfo->m_unzipFlags & UnzipInSeparateSubDirs);

    Qt::WindowFlags windowFlags=this->windowFlags() & ~(Qt::WindowSystemMenuHint |
                                                        Qt::WindowMinMaxButtonsHint |
                                                        Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags);

    connect(ui->pushButtonDir, SIGNAL(clicked()), this, SLOT(selectDirectory()));
}

UnzipDialog::~UnzipDialog()
{
    delete ui;
}

void UnzipDialog::selectDirectory()
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

void UnzipDialog::changeEvent(QEvent *e)
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

void UnzipDialog::accept()
{
    QDialog::accept();
    m_pArchiveInfo->m_to=ui->lineEditDestination->text();
    m_pArchiveInfo->m_filter=ui->lineEditFilter->text();
    int iFlags=0;
    if(ui->checkBoxUnpackPath->checkState())
        iFlags|=UnzipPath;
    if(ui->checkBoxSeparateDirs->checkState())
        iFlags|=UnzipInSeparateSubDirs;
     m_pArchiveInfo->m_unzipFlags=QFlag(iFlags);
}
