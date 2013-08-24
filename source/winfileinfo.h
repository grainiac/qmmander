/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A class providing file information similar to QFileInfo.
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
#ifndef WINFILEINFO_H
#define WINFILEINFO_H

#include <QtCore>
#include <list>
#include "windows.h"

class WinFileInfo;

typedef QList<WinFileInfo>  WinFileInfoList;
typedef QList<WinFileInfo*> WinFileInfoPtrList;
typedef QList<quint64>      WinFileInfoIndices;
typedef WIN32_FIND_DATA     Win32FileInfo;

class QIcon;

class WinFileInfo
{    
public:
    WinFileInfo();
    WinFileInfo(const Win32FileInfo& st);
    WinFileInfo(QString SMBPath, bool isSMBRoot);
    WinFileInfo(QString archive, QString fileName, bool isDir, quint64 fileSize, qint64 lastModTime);
    ~WinFileInfo();

    static QStringList getDriveList();
    static QString getDriveFromPath(QString path);
    static QString markArchiveNameInPath(QString path);
    static QString getFileTypeDescripton(const WinFileInfo& fi);
    static QString getAttributeString(const WinFileInfo& fi);
    static QString getFreeSpaceOnDrive(QString path);
    static WinFileInfoPtrList* listFiles(QString path);
    static void executeRegisteredShellActionForFile(QString filePath);

    enum SortOptions
    {
        WFI_FILENAME=0,
        WFI_SIZE,
        WFI_DATE,
        WFI_DESCRIPTION,
        WFI_ATTRIBUTES
    };

    static WinFileInfoIndices sortFileInfoList(WinFileInfoPtrList const * const list,
                                               SortOptions sortBy,
                                               QString filter,
                                               bool ascending);

    void setData(const Win32FileInfo& fi);
    Win32FileInfo data();

    void setPath2File(QString strPath);

    QString fileName() const;
    QString archiveFileName() const;
    QString filePath() const;
    QString path() const;
    QString suffix() const;
    QString archive() const;
    QString alternateFileName() const;
    quint64 size() const;
    qint32 attributes() const;
    bool setAttributes(DWORD attributes);
    time_t creationTime() const;
    time_t lastAccessTime() const;
    time_t lastModificationTime() const;
    time_t fileTime2IntegerTime(const FILETIME* ft) const;

    static time_t timeFromSystemTime(const SYSTEMTIME& systime);
    static void timetToFileTime(time_t t, LPFILETIME pft);

    bool isArchiveFile() const;
    bool isDir() const;
    bool isFile() const;
    bool isExecutable() const;
    bool isShellLink() const;
    bool isSymLink() const;
    bool isRoot() const;
    static bool isSmbPath(QString path);
    static bool isSmbRoot(QString path);
    static bool isSmbShareRoot(QString path);
    bool isSMBPath() const;
    void executeRegisteredShellAction();

    QIcon getWinIcon() const;

    QDataStream& serialize(QDataStream& out) const;
    QDataStream& deserialize(QDataStream& in);

    QString resolveLnkFile();

private:
    qint64 dword2qint64(DWORD low, DWORD high) const;

    Win32FileInfo fileInfo_;
    QString path_;
    QString smbPath_;
    QString archive_;
    bool isSmbPath_;
    bool isSmbRoot_;
    bool isArchiveFile_;
    bool isArchiveDirectory_;
};

QDataStream& operator<<(QDataStream &out, const WinFileInfo& instance);

QDataStream& operator>>(QDataStream &in, WinFileInfo& instance);

#endif // WINFILEINFO_H
