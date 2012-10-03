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
#include "archiveinfo.h"
#include "fileactionfactories.h"
#include "fileactionhandlers.h"

FileActionFactory::FileActionFactory(ArchiveInfo * const archiveInfo)
:   m_archiveInfo(archiveInfo)
{
}

ArchiveInfo * const FileActionFactory::getArchiveInfo() const
{
    return m_archiveInfo;
}

UnzipFileActionFactory::UnzipFileActionFactory(ArchiveInfo * const archiveInfo)
:   FileActionFactory(archiveInfo)
{
    handlers[FA_OverWrite]=new UFAHDefault(this);
    handlers[FA_RenameDest]=new UFAHRenameDestinationManual(this);
    handlers[FA_RenameSource]=new UFAHRenameSourceManual(this);
    handlers[FA_RenameDestAuto]=new UFAHRenameDestinationAuto(this);
    handlers[FA_RenameSourceAuto]=new UFAHRenameSourceAuto(this);
    handlers[FA_OverWriteOlderAndSameAge]=new UFAHOverWriteOlderAndSameAge(this);
    handlers[FA_CopyLargerOverWriteSmaller]=new UFAHCopyLargerOverWriteSmaller(this);
    handlers[FA_CopySmallerOverWriteLarger]=new UFAHCopySmallerOverWriteLarger(this);
}

UnzipFileActionFactory::~UnzipFileActionFactory()
{
    handlers.clear();
}

FileActionHandler const * const UnzipFileActionFactory::getFileActionHandler(FileAction fileAction) const
{
    if(handlers.contains(fileAction))
        return handlers[fileAction];
    return handlers[FA_OverWrite];
}

ZipFileActionFactory::ZipFileActionFactory(ArchiveInfo * const archiveInfo)
:   FileActionFactory(archiveInfo),
    m_archive(new Archive(archiveInfo))
{
    handlers[FA_OverWrite]=new ZFAHDefault(this);
    handlers[FA_RenameDest]=new ZFAHRenameDestinationManual(this);
    handlers[FA_RenameSource]=new ZFAHRenameSourceManual(this);
    handlers[FA_RenameDestAuto]=new ZFAHRenameDestinationAuto(this);
    handlers[FA_RenameSourceAuto]=new ZFAHRenameSourceAuto(this);
    handlers[FA_OverWriteOlderAndSameAge]=new ZFAHOverWriteOlderAndSameAge(this);
    handlers[FA_CopyLargerOverWriteSmaller]=new ZFAHCopyLargerOverWriteSmaller(this);
    handlers[FA_CopySmallerOverWriteLarger]=new ZFAHCopySmallerOverWriteLarger(this);
}

ZipFileActionFactory::~ZipFileActionFactory()
{
    handlers.clear();
    delete m_archive;
}

FileActionHandler const * const ZipFileActionFactory::getFileActionHandler(FileAction fileAction) const
{
    if(handlers.contains(fileAction))
        return handlers[fileAction];
    return handlers[FA_OverWrite];
}

Archive * const ZipFileActionFactory::getArchive() const
{
    return m_archive;
}

void ZipFileActionFactory::closeOpenZipFiles() const
{
    m_archive->closeOpenZipHandle();
}
