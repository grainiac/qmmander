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
#include "zipthread.h"
#include "archive.h"
#include "archiveinfo.h"
#include "fileactionhandlers.h"
#include "zipunzipprogressdialog.h"
#include "FileExplorerView.h"

ZipThread::ZipThread(ZipUnzipProgressDialog const * const parent,
                     ArchiveInfo * const archiveInfo,
                     ZipFileActionFactory const * const fileActionFactory)
:   ArchiverThread(parent, archiveInfo),
    m_archiveInfo(archiveInfo),
    m_fileActionFactory(fileActionFactory)
{
}

ZipThread::~ZipThread()
{
}

void ZipThread::run()
{    
    initializeZip();

    for(int i=0; i<m_archiveInfo->m_selectedFiles.count(); i++)
    {
        WinFileInfo fileInfo=m_archiveInfo->m_selectedFiles.at(i); // an archive

        if(m_archiveInfo->m_zipFlags & ZipDialog::ZipSeparateArchives)
        {
            QString to=m_archiveInfo->m_to;
            m_archiveInfo->m_to=to.left(to.lastIndexOf("\\")+1)+fileInfo.fileName()+".zip";
            QFile file(m_archiveInfo->m_to);
            if(!file.exists())
                zipFileOrDirectory(&fileInfo);
        }
        else
        {
            zipFileOrDirectory(&fileInfo);
        }
    }

    m_fileActionFactory->closeOpenZipFiles();

    emit(finishedExecution());
    endThread();
}

void ZipThread::initializeZip() const
{
    Archive arch(m_archiveInfo);
    arch.initUnzip();
}

void ZipThread::zipFileOrDirectory(WinFileInfo const * const fileInfo) const
{
    if( m_archiveInfo->isValidDirectory(fileInfo) )
        zipDirectory(fileInfo->filePath());
    else
        zipFile(fileInfo->filePath());
}

void ZipThread::zipDirectory(QString directoryPathName) const
{
    WinFileInfoPtrList* pFilePtrList=WinFileInfo::listFiles(directoryPathName);
    if(pFilePtrList)
    {
        for(int i=0; i<pFilePtrList->count(); i++)
        {
            WinFileInfo* fileInfo=pFilePtrList->at(i);

            if( m_archiveInfo->isValidDirectory(fileInfo) &&
                (m_archiveInfo->m_zipFlags & ZipDialog::ZipRecursive) )
            {
                zipDirectory(fileInfo->filePath());
            }
            else if(fileInfo->isFile())
            {
                zipFile(fileInfo->filePath());
            }
        }
        pFilePtrList->clear();
        delete pFilePtrList;
    }
}

void ZipThread::zipFile(QString filePathName) const
{
    if( m_archiveInfo->doesFileFilterMatch(filePathName) )
    {
        const_cast<ZipThread*>(this)->updateActualFileInArchiveInfo(filePathName);
        const_cast<ZipThread*>(this)->checkWaitConditions();
        if( m_archiveInfo->isFileActionValid() )
            performFileAction(filePathName);
    }
}

void ZipThread::updateActualFileInArchiveInfo(QString sourceFilePath) const
{    
    QFile sourceFile(sourceFilePath);
    m_archiveInfo->m_actualFileLastModification=QFileInfo(sourceFile).lastModified().toTime_t();
    m_archiveInfo->m_actualFileSize=sourceFile.size();

    m_archiveInfo->m_destFileName=makeArchiveFilePath(sourceFilePath);
}

QString ZipThread::makeArchiveFilePath(QString sourceFilePath) const
{
    QString archiveFilePath=sourceFilePath.replace(m_archiveInfo->m_from, "");
    while( (archiveFilePath.at(0)=='\\') ||
           (archiveFilePath.at(0)=='/') )
        archiveFilePath=archiveFilePath.right(archiveFilePath.length()-1);
    return archiveFilePath;
}

void ZipThread::performFileAction(QString sourceFilePath) const
{
    FileActionHandler const * fah=m_fileActionFactory->getFileActionHandler(m_archiveInfo->m_fileAction);
    fah->doFileAction(sourceFilePath, makeArchiveFilePath(sourceFilePath));
}

