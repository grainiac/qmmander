/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Helper functions for zip archive access through the Zip Utils.
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
#include "ziputils.h"

namespace ZipUtils
{

void qDebugZipErrors(ZRESULT returnCode)
{
    if(returnCode)
    {
        char szMessage[256]="";
        FormatZipMessageZ(returnCode, szMessage, 256);
        QString debugMsg=QString("<ZIP Error> Code=0x%1, Reason=%2").arg(returnCode,8,16, QChar(48)).arg(szMessage);
        qDebug(debugMsg.toStdString().c_str());
    }
}

bool fileExists(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    HZIP zip=OpenZip(archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    bool bExists=false;
    if(zip)
    {
        bool bCaseInsensitive=false;
        int itemIndex;
        ZIPENTRY ze;
        ZRESULT res=FindZipItem(zip, archiveFilePath.toStdWString().c_str(), bCaseInsensitive, &itemIndex, &ze);
        if(!res)
            bExists=true;
        else
            qDebugZipErrors(res);
        CloseZip(zip);
    }
    return bExists;
}

qint64 fileSize(QString archivePathName, QString archiveFilePath, QString archivePassword, bool bCompressedSize)
{
    HZIP zip=OpenZip(archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    qint64 iSize=-1;
    if(zip)
    {
        bool bCaseInsensitive=false;
        int itemIndex;
        ZIPENTRY ze;
        ZRESULT res=FindZipItem(zip, archiveFilePath.toStdWString().c_str(), bCaseInsensitive, &itemIndex, &ze);
        if(!res)
            bCompressedSize ? iSize=ze.comp_size : iSize=ze.unc_size;
        else
            qDebugZipErrors(res);
        CloseZip(zip);
    }
    return iSize;
}

time_t fileTimeToUnixTime(unsigned int hi, unsigned int lo)
{
    qint64 llTime;
    llTime = (static_cast<qint64>(hi) << 32) + lo;
    llTime -= 116444736000000000LL;
    llTime /= 10000000;
    return static_cast<time_t>(llTime);
}

time_t fileLastModificationTime(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    HZIP zip=OpenZip(archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    time_t t=-1;
    if(zip)
    {
        bool bCaseInsensitive=false;
        int itemIndex;
        ZIPENTRY ze;
        ZRESULT res=FindZipItem(zip, archiveFilePath.toStdWString().c_str(), bCaseInsensitive, &itemIndex, &ze);
        if(!res)
            t=fileTimeToUnixTime(ze.mtime.dwHighDateTime, ze.mtime.dwLowDateTime);
        else
            qDebugZipErrors(res);
        CloseZip(zip);
    }
    return t;
}

void extractFile(QString archivePathName, QString archiveFilePath, QString archivePassword, QString destFilePath)
{
    HZIP zip=OpenZip(archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    if(zip)
    {
        bool bCaseInsensitive=false;
        int itemIndex;
        ZIPENTRY ze;        
        ZRESULT res=FindZipItem(zip, archiveFilePath.toStdWString().c_str(), bCaseInsensitive, &itemIndex, &ze);
        if(!res)
        {
            res=UnzipItem(zip, itemIndex, destFilePath.toStdWString().c_str());
        }
        qDebugZipErrors(res);
        CloseZip(zip);
    }
}

} // end of namespace ZipUtils
