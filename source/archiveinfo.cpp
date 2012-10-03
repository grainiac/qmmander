/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#include "archiveinfo.h"

bool ArchiveInfo::doesFileFilterMatch(QString filePath)
{
    QFile file(filePath);
    return ( m_filter=="*" ||
             m_filter==""  ||
             file.fileName().contains(m_filter) );
}

bool ArchiveInfo::isFileActionValid()
{
    return (m_fileAction != FA_NoAction ||
            m_fileAction != FA_Skip ||
            m_fileAction != FA_SkipAll);
}

void ArchiveInfo::resetSingleExecutableFileAction()
{
    if( isSingleExecutableFileAction() )
        m_fileAction=FA_NoAction;
}

bool ArchiveInfo::askUser4FileAction()
{
    if(!m_trueZipModeFalseUnzipMode)
    {
        return ( QFile(m_destFileName).exists() &&
                 m_fileAction==FA_NoAction );
    }
    else
    {
        return false;
    }
}

bool ArchiveInfo::isValidDirectory(const WinFileInfo* fileInfo) const
{
    return ( fileInfo->isDir() &&
             fileInfo->fileName()!="." &&
             fileInfo->fileName()!=".." );
}

bool ArchiveInfo::isSingleExecutableFileAction()
{
    return ( (m_fileAction==FA_Skip) ||
             (m_fileAction==FA_OverWrite) ||
             (m_fileAction==FA_RenameDest) ||
             (m_fileAction==FA_RenameSource) );
}
