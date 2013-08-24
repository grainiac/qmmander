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
#ifndef ATTRIBUTESDATEDIALOG_H
#define ATTRIBUTESDATEDIALOG_H

#include <QDialog>
#include "winfileinfo.h"

namespace Ui {
    class AttributesDateDialog;
}

class FileProperties
{
    struct Attributes
    {
        Qt::CheckState A;
        Qt::CheckState H;
        Qt::CheckState R;
        Qt::CheckState S;
    };

public:
    FileProperties()
    :   changeAttributes_(0),
        changeCreationTime_(false),
        creationTimestamp_(0),
        changeLastModificationTime_(false),
        lastModificationTimestamp_(0),
        shiftModificationTime_(false),
        modificationTimeShiftInSeconds_(0),
        processSubFolders_(false)
    {
        atttributes_.A=Qt::Unchecked;
        atttributes_.H=Qt::Unchecked;
        atttributes_.R=Qt::Unchecked;
        atttributes_.S=Qt::Unchecked;
    }


    DWORD GetAttributes() const
    {
        DWORD attributes=0;
        if(atttributes_.A==Qt::Checked)
            attributes |= FILE_ATTRIBUTE_ARCHIVE;
        if(atttributes_.H==Qt::Checked)
            attributes |= FILE_ATTRIBUTE_HIDDEN;
        if(atttributes_.R==Qt::Checked)
            attributes |= FILE_ATTRIBUTE_READONLY;
        if(atttributes_.S==Qt::Checked)
            attributes |= FILE_ATTRIBUTE_SYSTEM;
        return attributes;
    }

    QString GetAttributesString() const
    {
        QString attributes;
        if(atttributes_.A == Qt::Checked)   attributes+="+A ";
        if(atttributes_.A == Qt::Unchecked) attributes+="-A ";
        if(atttributes_.H == Qt::Checked)   attributes+="+H ";
        if(atttributes_.H == Qt::Unchecked) attributes+="-H ";
        if(atttributes_.R == Qt::Checked)   attributes+="+R ";
        if(atttributes_.R == Qt::Unchecked) attributes+="-R ";
        if(atttributes_.S == Qt::Checked)   attributes+="+S ";
        if(atttributes_.S == Qt::Unchecked) attributes+="-S ";
        return attributes;
    }

    bool   changeAttributes_;
    Attributes atttributes_;

    bool   changeCreationTime_;
    time_t creationTimestamp_;

    bool   changeLastModificationTime_;
    time_t lastModificationTimestamp_;

    bool   shiftModificationTime_;
    int    modificationTimeShiftInSeconds_;

    bool   processSubFolders_;
};

class AttributesDateDialog : public QDialog
{
    Q_OBJECT
public:
    AttributesDateDialog(const WinFileInfoList& files, QWidget *parent = 0);
    ~AttributesDateDialog();

    const FileProperties& getFileProperties();

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent* e);

private slots:
    void onCheckBoxChangeAttributes(bool toggle);
    void onAttributesChanged(int checkState);

    void onCheckBoxChangeModificationDate(bool toggle);
    void onCheckChangeCreationTime(bool toggle);
    void onClickCopyCreationTime();
    void onClickUseActualTime4CreationTime();
    void onCheckChangeModificationTime(bool toggle);
    void onClickCopyModificationTime();
    void onClickUseActualTime4ModificationTime();

    void onCheckBoxShiftModificationDate(bool toggle);
    void onShiftTimeChanged(int value);
    void onShiftTimeMultiplierComboChanged(int index);

    void onCheckBoxChangeSubFolders(bool toggle);

private:
    void analyzeFileList();
    void initInfoSection();
    void initShiftDateCombo();
    void initAttributesSection();
    void initModificationDateSection();
    void changeShiftTime(int value, int unit);
    QDateTime getCreationTime();
    QDateTime getModificationTime();

    Ui::AttributesDateDialog *ui_;
    const WinFileInfoList& files_;
    int sizeOfFiles_;
    int fileCount_;
    int folderCount_;
    int ACount_, HCount_, RCount_, SCount_, CCount_, ECount_, JCount_;

    FileProperties properties_;
};

#endif // ATTRIBUTESDATEDIALOG_H
