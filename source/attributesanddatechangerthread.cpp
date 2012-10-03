/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** This thread performs the adjustment of file attributes and dates.
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
#include "attributesanddatechangerthread.h"

AttributesAndDateChangerThread::AttributesAndDateChangerThread(const WinFileInfoList& files,
                                                               const FileProperties& properties,
                                                               const QProgressDialog& dlg,
                                                               QObject *parent)
:   QThread(parent),
    files_(0),
    properties_(properties),
    progressDialog_(dlg),
    dirCounter_(0),
    fileCounter_(0),
    sumOfFilesAndDirectories_(0),
    actualElement_(0)
{
    files_=new WinFileInfoPtrList();
    for(int i=0; i<files.size(); i++)
    {
        WinFileInfo* fi=new WinFileInfo();
        *fi=files.at(i);
        files_->push_back(fi);
    }
}

AttributesAndDateChangerThread::~AttributesAndDateChangerThread()
{
    delete files_;
}

void AttributesAndDateChangerThread::run()
{        
    emit(processMessage(tr("Counting files...")));
    for(int i=0; i<files_->count(); i++)
    {
        const WinFileInfo& fi=*files_->at(i);
        if(isProcessableDir(fi))
        {
            dirCounter_++;
            countFiles(fi.filePath());
        }
        else if(fi.isFile())
            fileCounter_++;
    }

    sumOfFilesAndDirectories_=fileCounter_+dirCounter_;
    emit(fileRangeMessage(0,sumOfFilesAndDirectories_));
    processTimer_.start();

    for(int i=0; i<files_->count(); i++)
    {
        const WinFileInfo& fi=*files_->at(i);        
        if(isProcessableDir(fi))
        {
            processFileOrDirectory(fi);
            if(properties_.processSubFolders_)
                processSubDirectory(fi);
        }
        else if(fi.isFile())
            processFileOrDirectory(fi);
    }

    emit(operationFinished(this));

    quit();
}

void AttributesAndDateChangerThread::countFiles(QString path) const
{
    WinFileInfoPtrList* filesInDirectory=WinFileInfo::listFiles(path);

    for(int i=0; i<filesInDirectory->count(); i++)
    {
        const WinFileInfo& fi=*filesInDirectory->at(i);
        if(isProcessableDir(fi))
        {
            dirCounter_++;
            countFiles(fi.filePath());
        }
        if(fi.isFile())
            fileCounter_++;
    }

    delete filesInDirectory;
}

void AttributesAndDateChangerThread::processSubDirectory(const WinFileInfo& fileInfo) const
{
    WinFileInfoPtrList* filesInDirectory=WinFileInfo::listFiles(fileInfo.filePath());

    for(int i=0; i<filesInDirectory->count(); i++)
    {
        const WinFileInfo& fi=*filesInDirectory->at(i);

        if(isProcessableDir(fi))
        {
            processFileOrDirectory(fi);
            processSubDirectory(fi);
        }
        else if(fi.isFile())
            processFileOrDirectory(fi);
    }

    delete filesInDirectory;
}

bool AttributesAndDateChangerThread::isProcessableDir(const WinFileInfo& fileInfo) const
{
    return fileInfo.isDir() && !fileInfo.fileName().endsWith(".");
}

void AttributesAndDateChangerThread::processFileOrDirectory(const WinFileInfo& fileInfo) const
{
    actualElement_++;

    changeAttributes(fileInfo);
    changeTimes(fileInfo);

    sendProcessMessage(fileInfo);
}

void AttributesAndDateChangerThread::changeAttributes(const WinFileInfo& fileInfo) const
{
    if(properties_.changeAttributes_)
    {
        SetFileAttributes(fileInfo.filePath().toStdWString().c_str(), properties_.GetAttributes());
        qDebug("Setting attributes %s to file: %s", properties_.GetAttributesString().toStdString().c_str(), fileInfo.filePath().toStdString().c_str());
    }
}
void AttributesAndDateChangerThread::changeTimes(const WinFileInfo& fileInfo) const
{
    // TODO: time changes are not calculated in local time so the shift is not correct
    if(timesNeedChange())
    {
        HANDLE file;
        // Process needs admin rights to succeed here
        if((file = CreateFile(fileInfo.filePath().toStdWString().c_str(),
                              GENERIC_ALL,
                              FILE_SHARE_READ|FILE_SHARE_WRITE,
                              0,
                              OPEN_EXISTING,
                              0,
                              0)) != INVALID_HANDLE_VALUE)
        {
            FILETIME creationTime, lastAccessTime, lastModificationTime;

            WinFileInfo::timetToFileTime(properties_.changeCreationTime_
                                            ? properties_.creationTimestamp_
                                            : fileInfo.creationTime(),
                                         &creationTime);

            WinFileInfo::timetToFileTime(fileInfo.lastAccessTime(), &lastAccessTime);

            if(properties_.shiftModificationTime_)
            {
                time_t newModTime=fileInfo.lastModificationTime()+properties_.modificationTimeShiftInSeconds_;
                WinFileInfo::timetToFileTime(newModTime, &lastModificationTime);
                lastAccessTime=lastModificationTime;
            }
            else
            {
                WinFileInfo::timetToFileTime(properties_.changeLastModificationTime_
                                                ? properties_.lastModificationTimestamp_
                                                : fileInfo.lastModificationTime(),
                                             &lastModificationTime);
                lastAccessTime=lastModificationTime;
            }

            SetFileTime(file, &creationTime, &lastAccessTime, &lastModificationTime);
        }
        CloseHandle(file);
    }
}

bool AttributesAndDateChangerThread::timesNeedChange() const
{
    return properties_.changeCreationTime_ || properties_.changeLastModificationTime_ || properties_.shiftModificationTime_;
}

void AttributesAndDateChangerThread::sendProcessMessage(const WinFileInfo& fileInfo) const
{
    if(processTimer_.elapsed()>100)
    {
        QString message=QString(tr("Processing %1 (%2/%3)"))
                        .arg(fileInfo.fileName())
                        .arg(actualElement_)
                        .arg(sumOfFilesAndDirectories_);

        emit(processMessage(message));
        processTimer_.restart();
    }
}

void AttributesAndDateChangerThread::cancelOperation()
{
    terminate();
    emit(operationFinished(this));
}

const QProgressDialog& AttributesAndDateChangerThread::getProgressDialog() const
{
    return progressDialog_;
}
