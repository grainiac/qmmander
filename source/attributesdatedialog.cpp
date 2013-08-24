/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A dialog for adjusting file attributes and dates.
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
#include "attributesdatedialog.h"
#include "ui_attributesdatedialog.h"
#include <QResizeEvent>

AttributesDateDialog::AttributesDateDialog(const WinFileInfoList& files, QWidget *parent)
:   QDialog(parent),
    ui_(new Ui::AttributesDateDialog),
    files_(files),
    sizeOfFiles_(0),
    fileCount_(0),
    folderCount_(0),
    ACount_(0),
    HCount_(0),
    RCount_(0),
    SCount_(0),
    CCount_(0),
    ECount_(0),
    JCount_(0)
{
    ui_->setupUi(this);
    Qt::WindowFlags flags=windowFlags() & ~(Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint | Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
    ui_->groupBoxAttributes->setVisible(false);
    ui_->groupBoxDate->setVisible(false);
    ui_->groupBoxShiftModificationDate->setVisible(false);
    connect(ui_->checkBoxChangeAttributes, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChangeAttributes(bool)));
    connect(ui_->groupBoxAttributes, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChangeAttributes(bool)));
    connect(ui_->checkAttribArchive, SIGNAL(stateChanged(int)), this, SLOT(onAttributesChanged(int)));
    connect(ui_->checkAttribReadOnly, SIGNAL(stateChanged(int)), this, SLOT(onAttributesChanged(int)));
    connect(ui_->checkAttribHidden, SIGNAL(stateChanged(int)), this, SLOT(onAttributesChanged(int)));
    connect(ui_->checkAttribSystem, SIGNAL(stateChanged(int)), this, SLOT(onAttributesChanged(int)));

    connect(ui_->checkBoxChangeDate, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChangeModificationDate(bool)));
    connect(ui_->groupBoxDate, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChangeModificationDate(bool)));
    connect(ui_->checkCreationTime, SIGNAL(toggled(bool)), SLOT(onCheckChangeCreationTime(bool)));
    connect(ui_->pushButtonCopyCreationTime, SIGNAL(clicked()), this, SLOT(onClickCopyCreationTime()));
    connect(ui_->pushButtonActualTime, SIGNAL(clicked()), this, SLOT(onClickUseActualTime4CreationTime()));
    connect(ui_->checkModificationTime, SIGNAL(toggled(bool)), SLOT(onCheckChangeModificationTime(bool)));
    connect(ui_->pushButtonCopyModificationTime, SIGNAL(clicked()), this, SLOT(onClickCopyModificationTime()));
    connect(ui_->pushButtonActualTime2, SIGNAL(clicked()), this, SLOT(onClickUseActualTime4ModificationTime()));

    connect(ui_->checkBoxShiftModificationDate, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxShiftModificationDate(bool)));
    connect(ui_->groupBoxShiftModificationDate, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxShiftModificationDate(bool)));
    connect(ui_->spinBoxTimeShift, SIGNAL(valueChanged(int)), this, SLOT(onShiftTimeChanged(int)));
    connect(ui_->comboBoxTimeShift, SIGNAL(currentIndexChanged(int)), SLOT(onShiftTimeMultiplierComboChanged(int)));

    connect(ui_->checkChangeSubfolders, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxChangeSubFolders(bool)));

    analyzeFileList();
    initInfoSection();
    initShiftDateCombo();
    initAttributesSection();
    initModificationDateSection();

    if(folderCount_==0)
        ui_->checkChangeSubfolders->setEnabled(false);
}

AttributesDateDialog::~AttributesDateDialog()
{
    delete ui_;
}

void AttributesDateDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui_->retranslateUi(this);
        break;
    default:
        break;
    }
}

void AttributesDateDialog::onCheckBoxChangeAttributes(bool toggle)
{
    if(toggle)
    {
        ui_->checkBoxChangeAttributes->setVisible(false);
        ui_->checkBoxChangeAttributes->setChecked(false);
        ui_->groupBoxAttributes->setVisible(true);
        ui_->groupBoxAttributes->setChecked(true);
    }
    else
    {
        ui_->checkBoxChangeAttributes->setVisible(true);
        ui_->groupBoxAttributes->setVisible(false);
        ui_->groupBoxAttributes->setChecked(false);
    }
    properties_.changeAttributes_=toggle;
    update();
}

void AttributesDateDialog::onCheckBoxChangeModificationDate(bool toggle)
{
    if(toggle)
    {
        ui_->checkBoxChangeDate->setVisible(false);
        ui_->checkBoxChangeDate->setChecked(false);
        ui_->groupBoxDate->setVisible(true);
        ui_->groupBoxDate->setChecked(true);
    }
    else
    {
        ui_->checkBoxChangeDate->setVisible(true);
        ui_->groupBoxDate->setVisible(false);
        ui_->groupBoxDate->setChecked(false);
        properties_.changeCreationTime_=false;
        properties_.changeLastModificationTime_=false;
    }
    update();
}

void AttributesDateDialog::onCheckBoxShiftModificationDate(bool toggle)
{
    if(toggle)
    {
        ui_->checkBoxShiftModificationDate->setVisible(false);
        ui_->checkBoxShiftModificationDate->setChecked(false);
        ui_->groupBoxShiftModificationDate->setVisible(true);
        ui_->groupBoxShiftModificationDate->setChecked(true);
        changeShiftTime(0,0);
    }
    else
    {
        ui_->checkBoxShiftModificationDate->setVisible(true);
        ui_->groupBoxShiftModificationDate->setVisible(false);
        ui_->groupBoxShiftModificationDate->setChecked(false);
    }
    properties_.shiftModificationTime_=toggle;
    update();
}

void AttributesDateDialog::paintEvent(QPaintEvent* e)
{
    QDialog::paintEvent(e);
    QPoint pos=ui_->checkChangeSubfolders->pos();
    resize(QSize(size().width(), pos.y()));
}

void AttributesDateDialog::analyzeFileList()
{
    for(int i=0; i<files_.count(); i++)
    {
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_ARCHIVE)
            ACount_++;
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_HIDDEN)
            HCount_++;
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_READONLY)
            RCount_++;
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_SYSTEM)
            SCount_++;

        if(files_.at(i).attributes() & FILE_ATTRIBUTE_COMPRESSED)
            CCount_++;
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_ENCRYPTED)
            ECount_++;
        if(files_.at(i).attributes() & FILE_ATTRIBUTE_REPARSE_POINT)
            JCount_++;

        if(files_.at(i).isFile() )
        {
            sizeOfFiles_+=files_.at(i).size();
            fileCount_++;
        }
        if(files_.at(i).isDir())
            folderCount_++;
    }
    sizeOfFiles_/=1024;
}

void AttributesDateDialog::initInfoSection()
{
    QString info=tr("%1 file(s), %2 folder(s) selected");
    ui_->labelSelectedFiles->setText(QString(info).arg(fileCount_).arg(folderCount_));
    ui_->labelSelectionSizeValue->setText(QString("%1 KiBytes").arg(sizeOfFiles_));
}

void AttributesDateDialog::initShiftDateCombo()
{
    ui_->comboBoxTimeShift->addItem(tr("second(s)"));
    ui_->comboBoxTimeShift->addItem(tr("minute(s)"));
    ui_->comboBoxTimeShift->addItem(tr("hour(s)"));
    ui_->comboBoxTimeShift->addItem(tr("day(s)"));
    ui_->comboBoxTimeShift->addItem(tr("month(s)"));
    ui_->comboBoxTimeShift->addItem(tr("year(s)"));
}

void AttributesDateDialog::initAttributesSection()
{
    properties_.atttributes_.A=Qt::Unchecked;
    properties_.atttributes_.H=Qt::Unchecked;
    properties_.atttributes_.R=Qt::Unchecked;
    properties_.atttributes_.S=Qt::Unchecked;

    int totalFiles=fileCount_+folderCount_;
    if ( totalFiles == ACount_ )
    {
        ui_->checkAttribArchive->setCheckState(Qt::Checked);
        properties_.atttributes_.A=Qt::Checked;
    }
    else if ( (totalFiles > ACount_) && (ACount_ != 0) )
    {
        ui_->checkAttribArchive->setCheckState(Qt::PartiallyChecked);
        properties_.atttributes_.A=Qt::PartiallyChecked;
    }

    if ( totalFiles == HCount_ )
    {
        ui_->checkAttribHidden->setCheckState(Qt::Checked);
        properties_.atttributes_.H=Qt::Checked;
    }
    else if ( (totalFiles > HCount_) && (HCount_ != 0) )
    {
        ui_->checkAttribHidden->setCheckState(Qt::PartiallyChecked);
        properties_.atttributes_.H=Qt::PartiallyChecked;
    }

    if ( totalFiles == RCount_ )
    {
        ui_->checkAttribReadOnly->setCheckState(Qt::Checked);
        properties_.atttributes_.R=Qt::Checked;
    }
    else if ( (totalFiles > RCount_) && (RCount_ != 0) )
    {
        ui_->checkAttribReadOnly->setCheckState(Qt::PartiallyChecked);
        properties_.atttributes_.R=Qt::PartiallyChecked;
    }

    if ( totalFiles == SCount_ )
    {
        ui_->checkAttribSystem->setCheckState(Qt::Checked);
        properties_.atttributes_.S=Qt::Checked;
    }
    else if ( (totalFiles > SCount_) && (SCount_ != 0) )
    {
        ui_->checkAttribSystem->setCheckState(Qt::PartiallyChecked);
        properties_.atttributes_.S=Qt::PartiallyChecked;
    }

    if ( totalFiles == CCount_ )
        ui_->checkAttribCompressed->setCheckState(Qt::Checked);
    else if ( (totalFiles > CCount_) && (CCount_ != 0) )
        ui_->checkAttribCompressed->setCheckState(Qt::PartiallyChecked);

    if ( totalFiles == ECount_ )
        ui_->checkAttribEncrypted->setCheckState(Qt::Checked);
    else if ( (totalFiles > ECount_) && (ECount_ != 0) )
        ui_->checkAttribEncrypted->setCheckState(Qt::PartiallyChecked);

    if ( totalFiles == JCount_ )
        ui_->checkAttribJunctionPoint->setCheckState(Qt::Checked);
    else if ( (totalFiles > JCount_) && (JCount_ != 0) )
        ui_->checkAttribJunctionPoint->setCheckState(Qt::PartiallyChecked);
}

void AttributesDateDialog::initModificationDateSection()
{    
    QDateTime creationTime=QDateTime::fromTime_t(files_.at(0).creationTime());
    QDateTime modificationTime=QDateTime::fromTime_t(files_.at(0).lastModificationTime());
    QDateTime lastAccess=QDateTime::fromTime_t(files_.at(0).lastAccessTime());

    ui_->timeEditCreationTime->setTime(creationTime.time());
    ui_->dateEditCreationTime->setDate(creationTime.date());
    ui_->timeEditModificationTime->setTime(modificationTime.time());
    ui_->dateEditModificationTime->setDate(modificationTime.date());
    ui_->labelLastAccessTimeData->setText(lastAccess.toString("dd.MM.yyyy hh:mm:ss"));
}

void AttributesDateDialog::onClickCopyCreationTime()
{
    ui_->timeEditModificationTime->setTime(getCreationTime().time());
    ui_->dateEditModificationTime->setDate(getCreationTime().date());
    properties_.lastModificationTimestamp_=getCreationTime().toTime_t();
}

void AttributesDateDialog::onClickUseActualTime4CreationTime()
{
    QDateTime d=QDateTime::currentDateTime();
    ui_->timeEditCreationTime->setTime(d.time());
    ui_->dateEditCreationTime->setDate(d.date());
    properties_.creationTimestamp_=d.toTime_t();
}

void AttributesDateDialog::onClickCopyModificationTime()
{
    ui_->timeEditCreationTime->setTime(getModificationTime().time());
    ui_->dateEditCreationTime->setDate(getModificationTime().date());
    properties_.creationTimestamp_=getModificationTime().toTime_t();
}

void AttributesDateDialog::onClickUseActualTime4ModificationTime()
{
    QDateTime d=QDateTime::currentDateTime();
    ui_->timeEditModificationTime->setTime(d.time());
    ui_->dateEditModificationTime->setDate(d.date());
    properties_.lastModificationTimestamp_=d.toTime_t();
}

void AttributesDateDialog::onCheckChangeCreationTime(bool toggle)
{
    properties_.changeCreationTime_=toggle;
    properties_.creationTimestamp_=getCreationTime().toTime_t();
}

void AttributesDateDialog::onCheckChangeModificationTime(bool toggle)
{
    properties_.changeLastModificationTime_=toggle;
    properties_.changeLastModificationTime_=getModificationTime().toTime_t();
}

void AttributesDateDialog::onShiftTimeChanged(int value)
{
    int index=ui_->comboBoxTimeShift->currentIndex();
    changeShiftTime(value, index);
}

void AttributesDateDialog::onShiftTimeMultiplierComboChanged(int index)
{
    int value=ui_->spinBoxTimeShift->value();
    changeShiftTime(value, index);
}

void AttributesDateDialog::changeShiftTime(int value, int unit)
{
    QDateTime d;
    switch(unit)
    {
    case 0: //seconds
        properties_.modificationTimeShiftInSeconds_=value;
        break;
    case 1: //minutes
        properties_.modificationTimeShiftInSeconds_=value*60;
        break;
    case 2: //hours
        properties_.modificationTimeShiftInSeconds_=value*60*60;
        break;
    case 3: //days
        properties_.modificationTimeShiftInSeconds_=value*24*60*60;
        break;
    case 4: //months        
        properties_.modificationTimeShiftInSeconds_=getModificationTime().addMonths(value).toTime_t();
        break;
    case 5: //years
        properties_.modificationTimeShiftInSeconds_=getModificationTime().addYears(value).toTime_t();
        break;
    }
}

void AttributesDateDialog::onAttributesChanged(int checkState)
{
    QCheckBox* sender=dynamic_cast<QCheckBox*>(QObject::sender());    
    if(sender)
    {
        Qt::CheckState state=static_cast<Qt::CheckState>(checkState);
        if(sender == ui_->checkAttribArchive)
            properties_.atttributes_.A=state;
        else if(sender == ui_->checkAttribHidden)
            properties_.atttributes_.H=state;
        else if(sender == ui_->checkAttribReadOnly)
            properties_.atttributes_.R=state;
        else if(sender == ui_->checkAttribSystem)
            properties_.atttributes_.S=state;
    }
}

const FileProperties& AttributesDateDialog::getFileProperties()
{
    return properties_;
}

QDateTime AttributesDateDialog::getCreationTime()
{
    QDateTime d;
    d.setTime(ui_->timeEditCreationTime->time());
    d.setDate(ui_->dateEditCreationTime->date());
    return d;
}

QDateTime AttributesDateDialog::getModificationTime()
{
    QDateTime d;
    d.setTime(ui_->timeEditModificationTime->time());
    d.setDate(ui_->dateEditModificationTime->date());
    return d;
}

void AttributesDateDialog::onCheckBoxChangeSubFolders(bool toggle)
{
    properties_.processSubFolders_=toggle;
}
