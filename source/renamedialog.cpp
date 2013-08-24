/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A widget class providing a dialog for renaming files.
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
#include "renamedialog.h"
#include "ui_renamedialog.h"

RenameDialog::RenameDialog(const WinFileInfo& fi, QWidget *parent)
:   QDialog(parent),
    ui(new Ui::RenameDialog)
{    
    ui->setupUi(this);
    Qt::WindowFlags flags=windowFlags() & ~(Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QString strFileName=fi.fileName();
    strFileName=strFileName.left(strFileName.lastIndexOf('.'));
    ui->lineEditOriginalName->setText(fi.fileName());
    ui->lineEditNewName->setText(strFileName);
    ui->lineEditNewExtension->setText(fi.suffix());
}

RenameDialog::RenameDialog(const QFileInfo& fi, QWidget *parent)
:   QDialog(parent),
    ui(new Ui::RenameDialog)
{
    ui->setupUi(this);
    Qt::WindowFlags flags=windowFlags() & ~(Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QString strFileName=fi.fileName();
    strFileName=strFileName.left(strFileName.lastIndexOf('.'));
    ui->lineEditOriginalName->setText(fi.fileName());
    ui->lineEditNewName->setText(strFileName);
    ui->lineEditNewExtension->setText(fi.suffix());
}

RenameDialog::~RenameDialog()
{
    delete ui;
}

void RenameDialog::changeEvent(QEvent *e)
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

void RenameDialog::editMode(bool bEditInOneLine)
{
    if(bEditInOneLine)
    {
        if(ui->lineEditNewExtension->text().length()>0)
            ui->lineEditNewName->setText(ui->lineEditNewName->text()+"."+ui->lineEditNewExtension->text());
        ui->lineEditNewExtension->setText("");
        ui->lineEditNewExtension->setEnabled(false);
    }
    else
    {
        ui->lineEditNewExtension->setEnabled(true);

        QString fileName=ui->lineEditNewName->text();
        ui->lineEditNewName->setText(fileName.left(fileName.lastIndexOf('.')));

        int lastDot = fileName.lastIndexOf('.');
        if (lastDot != -1)            
            ui->lineEditNewExtension->setText(fileName.mid(lastDot + 1));
    }
}

QString RenameDialog::getFileName()
{
    if(ui->checkBoxOneField->isChecked())
        return ui->lineEditNewName->text();
    else
        return ui->lineEditNewName->text()+"."+ui->lineEditNewExtension->text();
}
