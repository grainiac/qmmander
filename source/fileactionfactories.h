/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Factories for FileAction strategies.
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
#ifndef FILEACTIONFACTORIES_H
#define FILEACTIONFACTORIES_H

#include <QtCore>
#include "fileaction.h"

class Archive;
class ArchiveInfo;
class FileActionHandler;

class FileActionFactory
{
public:
    FileActionFactory(ArchiveInfo * const archiveInfo);
    virtual ~FileActionFactory() {}

    virtual FileActionHandler const * const getFileActionHandler(FileAction fileAction) const = 0;

    ArchiveInfo * const getArchiveInfo() const;

private:
    ArchiveInfo  * const m_archiveInfo;
};

class UnzipFileActionFactory : public FileActionFactory
{
public:
    UnzipFileActionFactory(ArchiveInfo * const archiveInfo);
    virtual ~UnzipFileActionFactory();

    virtual FileActionHandler const * const getFileActionHandler(FileAction fileAction) const;

private:
    QMap<FileAction, FileActionHandler*> handlers;
};

class ZipFileActionFactory : public FileActionFactory
{
public:
    ZipFileActionFactory(ArchiveInfo * const archiveInfo);
    virtual ~ZipFileActionFactory();

    virtual FileActionHandler const * const getFileActionHandler(FileAction fileAction) const;

    Archive * const getArchive() const;

    void closeOpenZipFiles() const;
private:
    QMap<FileAction, FileActionHandler*> handlers;
    Archive * const m_archive;
};


#endif // FILEACTIONFACTORIES_H
