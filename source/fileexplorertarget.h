/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** File model for the FileExplorerView.
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
#ifndef FILEEXPLORERTARGET_H
#define FILEEXPLORERTARGET_H

#include "winfileinfo.h"

class FileExplorerTarget
{
public:
    FileExplorerTarget();
    FileExplorerTarget(const FileExplorerTarget& source);
    ~FileExplorerTarget();
    FileExplorerTarget& operator=(const FileExplorerTarget& rhs);

    bool changeDir(QString path);
    void sortFiles(WinFileInfo::SortOptions sortBy, QString filter, bool sortAscending);

    QString getCanonicalPath() const;
    const WinFileInfoPtrList& getFiles() const;
    const WinFileInfoIndices& getSortedFileIndices() const;

private:
    void extractArchiveInformation();
    void buildCanonicalPath();
    void sortFiles();

    WinFileInfoPtrList* files_;
    WinFileInfoIndices sortedIndices_;

    QString path_;
    QString archiveName_;
    QString pathInArchive_;
    QString canonicalPath_;
};

#endif // FILEEXPLORERTARGET_H
