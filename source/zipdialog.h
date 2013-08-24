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
#ifndef ZIPDIALOG_H
#define ZIPDIALOG_H

#include <QDialog>

class ArchiveInfo;

namespace Ui
{
    class ZipDialog;
}

class ZipDialog : public QDialog
{
    Q_OBJECT
public:
    enum ZipFlag { ZipRecursive        =   0x001,
                   ZipMove2Archive     =   0x002,
                   ZipSeparateArchives =   0x004,
                   ZipAllFlags = ZipRecursive | ZipMove2Archive | ZipSeparateArchives };

    Q_DECLARE_FLAGS(ZipFlags, ZipFlag)

    ZipDialog(QWidget* pParent, ArchiveInfo* pAI);
    ~ZipDialog();

public slots:
    void accept();

protected:
    void changeEvent(QEvent *e);

private slots:
    void selectDestination();
    void selectArchive();
    void selectDirectory();

private:
    Ui::ZipDialog* ui;
    ArchiveInfo* m_pArchiveInfo;
};

#endif // ZIPDIALOG_H
