/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <QDialog>
#include "winfileinfo.h"

namespace Ui {
    class RenameDialog;
}

class RenameDialog : public QDialog {
    Q_OBJECT
public:
    RenameDialog(const WinFileInfo& fi, QWidget *parent = 0);
    RenameDialog(const QFileInfo& fi, QWidget *parent = 0);
    ~RenameDialog();

    QString getFileName();

protected:
    void changeEvent(QEvent *e);

private slots:
    void editMode(bool editInOneLine);

private:        
    Ui::RenameDialog *ui;
};

#endif // RENAMEDIALOG_H
