/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
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
#ifndef FILEACTIONDIALOG_H
#define FILEACTIONDIALOG_H

#include <QDialog>
#include "fileaction.h"

class ArchiveInfo;

namespace Ui
{
    class FileActionDialog;
}

class FileActionDialog : public QDialog
{
    Q_OBJECT
public:
    FileActionDialog(ArchiveInfo * const archiveInfo,
                     FileAction& fileAction,
                     QWidget *parent = 0);

    ~FileActionDialog();

public slots:
    virtual void reject();

protected:
    void changeEvent(QEvent *e);

private slots:
    void currentActionComboIndexChanged(int iIndex);
    void actionSelected();

private:    
    QString getSourceFile();
    QString getTargetFile();
    QString getSourceFileInfos();
    QString getTargetFileInfos();

    Ui::FileActionDialog *ui;
    ArchiveInfo * const m_archiveInfo;
    FileAction* m_fileAction;    
};

#endif // FILEACTIONDIALOG_H
