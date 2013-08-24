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
#include "updatedialog.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(bool bCheck4UpdatesAtStart, QWidget *parent)
:   QDialog(parent),    
    ui(new Ui::UpdateDialog),
    m_bCheckState(bCheck4UpdatesAtStart)
{
    ui->setupUi(this);
    ui->checkBox->setChecked(m_bCheckState);
    Qt::WindowFlags flags=windowFlags() & ~(Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
    setStyleSheet("QDialog{background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 gray, stop: 0.6 white)}");
    setMinimumSize(this->size());  // prevent resizing
    setMaximumSize(this->size());
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::changeEvent(QEvent *e)
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

void UpdateDialog::setLabelText(QString strText)
{
    ui->label->setText(strText);
}

bool UpdateDialog::getUpdateBoxState()
{
    return m_bCheckState;
}

void UpdateDialog::checkBoxToggled(bool bCheckState)
{
    m_bCheckState=bCheckState;
}
