/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A small wrapper around the PhysicsFS API.
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
#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "winfileinfo.h"
#include "ZipUtils/zip.h"
#include <QtCore>

class ArchiveInfo;
struct PHYSFS_File;

class Archive
{
public:
    Archive(ArchiveInfo * const archivInfo, int bufferSizeInKiBytes=64);
    ~Archive();

    static WinFileInfoPtrList* listFiles(QString path2Archive, QString archiveName, QString pathInArchive);
    void   removeFile(QString archivePathName, QString archiveFilePath);
    bool   exists(QString archivePathName, QString archiveFilePath, QString archivePassword="");
    qint64 fileSize(QString archivePathName, QString archiveFilePath, QString archivePassword="");
    qint64 fileSizeCompressed(QString archivePathName, QString archiveFilePath, QString archivePassword="");
    time_t fileLastModificationTime(QString archivePathName, QString archiveFilePath, QString archivePassword="");

    int initUnzip(); // 1 on success, else error
    int initZip(); // 1 on success, else error

    void archiveFile(QString sourceFilePath, QString archiveFilePath, QString archivePassword="");
    void closeOpenZipHandle();
    void extractFile(QString archiveFilePath, QString destinationFilePath);

private:
    void scanDirectory(QString directoryPath);
    void scanFile(const WinFileInfo& fileInfo);
    void scanArchive(QString archiveDirectoryPath);
    void scanArchiveFile(QString archiveFilePath);
    void setFileTimes(QString filePath, time_t time);
    PHYSFS_File* openArchiveFile(QString archiveFilePath, bool trueWriteModeDefaultReadMode=false);
    void closeArchiveFile(PHYSFS_File* pFile);
    void extractFileWithPhysFS(QString archiveFilePath, QString destinationFilePath);
    void extractFileWithZipUtils(QString archiveFilePath, QString destinationFilePath);

    int initPhysFS();
    int deinitPhysFS();

    ArchiveInfo * const m_archiveInfo;
    int m_bufferSize;
    char* m_buffer;

    HZIP m_zipHandle;
    QString m_archivePathName;
    bool m_archiveFormatIs7Zip;
};

#endif // ARCHIVE_H
