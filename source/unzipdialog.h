/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
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
#ifndef UNZIPDIALOG_H
#define UNZIPDIALOG_H

#include <QDialog>
#include <QDir>

class ArchiveInfo;

namespace Ui {
    class UnzipDialog;
}

class UnzipDialog : public QDialog {
    Q_OBJECT
public:
    enum UnzipFlag { UnzipPath              =   0x001,
                     UnzipInSeparateSubDirs =   0x002,
                     UnzipAllFlags = UnzipPath | UnzipInSeparateSubDirs };

    Q_DECLARE_FLAGS(UnzipFlags, UnzipFlag)

    UnzipDialog(QWidget* pParent, ArchiveInfo* pAI);
    ~UnzipDialog();    

public slots:    
    void accept();
    
protected:
    void changeEvent(QEvent *e);

private slots:
    void selectDirectory();

private:    
    Ui::UnzipDialog* ui;
    ArchiveInfo* m_pArchiveInfo;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(UnzipDialog::UnzipFlags)

#endif // UNZIPDIALOG_H
