/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This is the Update Dialog for the Qmmander Application.
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
#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
    class UpdateDialog;
}

class UpdateDialog : public QDialog {
    Q_OBJECT
public:
    UpdateDialog(bool bCheck4UpdatesAtStart, QWidget *parent = 0);
    ~UpdateDialog();

    void setLabelText(QString strText);    
    bool getUpdateBoxState();

protected:
    void changeEvent(QEvent *e);

private slots:
    void checkBoxToggled(bool bCheckState);

private:
    Ui::UpdateDialog *ui;
    bool m_bCheckState;
};

#endif // UPDATEDIALOG_H
