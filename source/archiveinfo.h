/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A helper class for storing process information on an archive.
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
#ifndef ARCHIVEINFO_H
#define ARCHIVEINFO_H

#include <QtCore>
#include "archive.h"
#include "unzipdialog.h"
#include "zipdialog.h"
#include "fileaction.h"
#include "winfileinfo.h"

class ArchiveInfo
{
public:
    ArchiveInfo(bool trueZipModeFalseUnzipMode=false)
    :   m_trueZipModeFalseUnzipMode(trueZipModeFalseUnzipMode),
        m_filter("*"),
        m_from(""),
        m_to(""),
        m_unzipFlags(UnzipDialog::UnzipPath),
        m_zipFlags(ZipDialog::ZipRecursive),
        m_isUserInputRequested(false),
        m_fileAction(FA_NoAction),
        m_fileCount(0),
        m_totalSize(0),
        m_bytesProcessed(0),
        m_bytesProcessedOld(0),
        m_filesProcessed(0),
        m_percent(0),
        m_timeElapsed(""),
        m_timeRemaining(""),
        m_speed(""),
        m_path2Archive(""),
        m_actualArchive(""),
        m_path2ActualFile(""),
        m_actualFileName("")
    {
    };    

    bool doesFileFilterMatch(QString filePath);
    bool isFileActionValid();
    void resetSingleExecutableFileAction();
    bool askUser4FileAction();
    bool isValidDirectory(const WinFileInfo* fileInfo) const;


    bool m_trueZipModeFalseUnzipMode;
    QString m_filter;
    QString m_from;
    QString m_to;
    WinFileInfoList m_selectedFiles;
    UnzipDialog::UnzipFlags m_unzipFlags;
    ZipDialog::ZipFlags m_zipFlags;

    bool m_isUserInputRequested;
    FileAction m_fileAction;

    qint64 m_fileCount;
    qint64 m_totalSize;
    qint64 m_bytesProcessed;
    qint64 m_bytesProcessedOld;
    int     m_filesProcessed;
    int     m_percent;
    QString m_timeElapsed;
    QString m_timeRemaining;
    QString m_speed;

    QString m_path2Archive;
    QString m_actualArchive;
    QString m_path2ActualFile;
    QString m_actualFileName;

    qint64  m_actualFileSize;
    time_t  m_actualFileLastModification;

    QString m_destFileName;
    qint64  m_destFileSize;
    time_t  m_destFileLastModification;

private:
    bool isSingleExecutableFileAction();
};

#endif // ARCHIVEINFO_H
