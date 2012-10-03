/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A dialog which allows the user to select a action before the application
** performs a file copy, file move or file extraction operation.
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
#include "fileactiondialog.h"
#include "ui_fileactiondialog.h"
#include "archiveinfo.h"

class ArchiveInfo;

FileActionDialog::FileActionDialog(ArchiveInfo * const archiveInfo,
                                   FileAction& fileAction,
                                   QWidget *parent)
:   QDialog(parent),    
    ui(new Ui::FileActionDialog),
    m_archiveInfo(archiveInfo),
    m_fileAction(&fileAction)
{
    ui->setupUi(this);

    ui->labelSourceFile->setText(getSourceFile());
    ui->labelSourceFileInfos->setText(getSourceFileInfos());
    ui->labelTargetFile->setText(getTargetFile());
    ui->labelTargetFileInfos->setText(getTargetFileInfos());

    ui->comboBoxOptions->addItem(tr("Select action..."),                              QVariant::fromValue((int)FA_NoAction));
    ui->comboBoxOptions->addItem(tr("Rename source file"),                            QVariant::fromValue((int)FA_RenameSource));
    ui->comboBoxOptions->addItem(tr("Rename target file"),                            QVariant::fromValue((int)FA_RenameDest));
    ui->comboBoxOptions->addItem(tr("Auto-Rename source file"),                       QVariant::fromValue((int)FA_RenameSourceAuto));
    ui->comboBoxOptions->addItem(tr("Auto-Rename target file"),                       QVariant::fromValue((int)FA_RenameDestAuto));
    ui->comboBoxOptions->addItem(tr("Overwrite files that are older or of same age"), QVariant::fromValue((int)FA_OverWriteOlderAndSameAge));
    ui->comboBoxOptions->addItem(tr("Overwrite smaller & copy larger files"),         QVariant::fromValue((int)FA_CopyLargerOverWriteSmaller));
    ui->comboBoxOptions->addItem(tr("Overwrite larger & copy smaller files"),         QVariant::fromValue((int)FA_CopySmallerOverWriteLarger));
    ui->comboBoxOptions->view()->setFixedWidth(300);
    connect(ui->comboBoxOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(currentActionComboIndexChanged(int)));

    connect(ui->pushButtonOverwrite, SIGNAL(clicked()), this, SLOT(actionSelected()));
    connect(ui->pushButtonOverwriteAll, SIGNAL(clicked()), this, SLOT(actionSelected()));
    connect(ui->pushButtonSkip, SIGNAL(clicked()), this, SLOT(actionSelected()));
    connect(ui->pushButtonSkipAll, SIGNAL(clicked()), this, SLOT(actionSelected()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));

    Qt::WindowFlags windowFlags=this->windowFlags() & ~(Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint);    
    setWindowFlags(windowFlags);

    *m_fileAction=FA_NoAction; // FA_NoAction is the default return value for cancel action.
}

FileActionDialog::~FileActionDialog()
{
    delete ui;
}

void FileActionDialog::changeEvent(QEvent *e)
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

void FileActionDialog::currentActionComboIndexChanged(int iIndex)
{
    QVariant variant=ui->comboBoxOptions->itemData(iIndex, Qt::UserRole);

    if(variant.type()==QVariant::Int)
    {        
        FileAction fa=(FileAction)variant.toInt();
        if(fa!=FA_NoAction)
        {
            *m_fileAction=fa;
            accept();
        }
    }
}

void FileActionDialog::reject()
{
    QDialog::reject();
}

void FileActionDialog::actionSelected()
{
    QObject* pSender=sender();

    if(ui->pushButtonOverwrite==pSender)
        *m_fileAction=FA_OverWrite;
    else if(ui->pushButtonOverwriteAll==pSender)
        *m_fileAction=FA_OverWriteAll;
    else if(ui->pushButtonSkip==pSender)
        *m_fileAction=FA_Skip;
    else if(ui->pushButtonSkipAll==pSender)
        *m_fileAction=FA_SkipAll;

    accept();
}

QString FileActionDialog::getSourceFile()
{
    return m_archiveInfo->m_actualFileName;
}

QString FileActionDialog::getTargetFile()
{
    return m_archiveInfo->m_destFileName;
}

QString FileActionDialog::getSourceFileInfos()
{
    return QString("%1 %2 Bytes, %3 %4").arg(tr("Size"))
                                        .arg(m_archiveInfo->m_actualFileSize)
                                        .arg(tr("last modified"))
                                        .arg(QDateTime::fromTime_t(m_archiveInfo->m_actualFileLastModification).toString("dd.MM.yyyy hh:mm:ss"));
}

QString FileActionDialog::getTargetFileInfos()
{
    return QString("%1 %2 Bytes, %3 %4").arg(tr("Size"))
                                        .arg(m_archiveInfo->m_destFileSize)
                                        .arg(tr("last modified"))
                                        .arg(QDateTime::fromTime_t(m_archiveInfo->m_destFileLastModification).toString("dd.MM.yyyy hh:mm:ss"));
}
