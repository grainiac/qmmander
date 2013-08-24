/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This thread zips files into archive(s).
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
#ifndef ZIPTHREAD_H
#define ZIPTHREAD_H

#include "archiverthread.h"
#include "fileactionfactories.h"

class ArchiveInfo;
class ZipUnzipProgressDialog;

class ZipThread : public ArchiverThread
{
    Q_OBJECT
public:
    ZipThread(ZipUnzipProgressDialog const * const parent,
              ArchiveInfo * const archiveInfo,
              ZipFileActionFactory const * const fileActionFactory);
    virtual ~ZipThread();

    virtual void run();
    virtual void performFileAction(QString sourceFilePath) const;

private:
    void initializeZip() const;
    void zipFileOrDirectory(WinFileInfo const * const fileInfo) const;
    void zipDirectory(QString directoryPathName) const;
    void zipFile(QString filePathName) const;
    void updateActualFileInArchiveInfo(QString sourceFilePath) const;
    QString makeArchiveFilePath(QString sourceFilePath) const;    

    ArchiveInfo* const m_archiveInfo;
    ZipFileActionFactory const * const m_fileActionFactory;
};

#endif // ZIPTHREAD_H
