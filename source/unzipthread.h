/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This thread unzips files from the archive(s).
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
#ifndef UNZIPTHREAD_H
#define UNZIPTHREAD_H

#include "archiverthread.h"
#include "fileactionfactories.h"

class ArchiveInfo;
class ZipUnzipProgressDialog;

class UnzipThread : public ArchiverThread
{    
    Q_OBJECT
public:
    UnzipThread(ZipUnzipProgressDialog  const * const parent,
                ArchiveInfo* const  archiveInfo,
                UnzipFileActionFactory const * const fileActionFactory);
    virtual ~UnzipThread();

    virtual void run();
    virtual void performFileAction(QString archiveFilePath) const;

private:
    void initializeUnzip() const;
    void unzipArchive(QString path2Archive, QString archiveName, QString pathInArchive="") const;    
    void unzipFile(QString archiveFilePath) const;
    void checkThatOuputPathExist(QString archiveFilePath) const;
    void updateActualArchiveInArchiveInfo(QString path2Archive, QString archiveName) const;
    void updateActualFileInArchiveInfo(QString archiveFilePath) const;

    QString getOutputDirectory() const;
    QString cleanPathName(QString pathName) const;
    QString makeOutFileName(QString archiveFilePath) const;

    ArchiveInfo* const m_archiveInfo;
    UnzipFileActionFactory const * const m_fileActionFactory;    
};

#endif // UNZIPTHREAD_H
