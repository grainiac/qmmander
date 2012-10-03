/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#include "unzipthread.h"
#include "archive.h"
#include "archiveinfo.h"
#include "fileactionhandlers.h"
#include "zipunzipprogressdialog.h"

UnzipThread::UnzipThread(ZipUnzipProgressDialog  const * const parent,
                         ArchiveInfo* const  archiveInfo,
                         UnzipFileActionFactory const * const fileActionFactory)
:   ArchiverThread(parent, archiveInfo),
    m_archiveInfo(archiveInfo),
    m_fileActionFactory(fileActionFactory)    
{   
}

UnzipThread::~UnzipThread()
{    
}

void UnzipThread::run()
{    
    initializeUnzip();

    for(int i=0; i<m_archiveInfo->m_selectedFiles.count(); i++)
    {
        WinFileInfo fileInfo=m_archiveInfo->m_selectedFiles.at(i); // an archive
        if(fileInfo.suffix().toUpper()=="7Z" ||
           fileInfo.suffix().toUpper()=="ZIP" )
        {
            updateActualArchiveInArchiveInfo(fileInfo.path(), fileInfo.fileName());
            unzipArchive(m_archiveInfo->m_path2Archive, fileInfo.fileName());
        }
        else // a file or directory in an archive
        {
            updateActualArchiveInArchiveInfo(fileInfo.path(), fileInfo.archive());
            if(fileInfo.isDir())
                unzipArchive(fileInfo.path(), fileInfo.archive(), fileInfo.fileName());
            else
                unzipFile(fileInfo.fileName());
        }
    }

    emit(finishedExecution());
    endThread();
}

void UnzipThread::initializeUnzip() const
{
    Archive arch(m_archiveInfo);
    arch.initUnzip();
}

void UnzipThread::updateActualArchiveInArchiveInfo(QString path2Archive, QString archiveName) const
{
    m_archiveInfo->m_path2Archive=path2Archive.endsWith("\\") ? path2Archive : path2Archive+"\\";
    m_archiveInfo->m_actualArchive=archiveName;
}

void UnzipThread::unzipArchive(QString path2Archive, QString archiveName, QString pathInArchive) const
{    
    m_archiveInfo->m_actualArchive=archiveName;

    WinFileInfoPtrList* fileInfoPtrList=Archive::listFiles(path2Archive, archiveName, pathInArchive);
    if(fileInfoPtrList)
    {
        for(int i=0; i<fileInfoPtrList->count(); i++)
        {
            WinFileInfo* fileInfo=fileInfoPtrList->at(i);

            if(fileInfo->isDir() && fileInfo->fileName()!="..") // Process a directory in archive
            {                
                QString destinationDirectory=getOutputDirectory();
                if(m_archiveInfo->m_unzipFlags & UnzipDialog::UnzipPath)
                    destinationDirectory+=QString("\\%1").arg(fileInfo->fileName());

                QDir dirDest(destinationDirectory);
                if(!dirDest.exists())
                    dirDest.mkpath(destinationDirectory);

                QString newPathInArchive=QString("%1").arg(fileInfo->fileName());
                unzipArchive(path2Archive, archiveName, newPathInArchive);
            }
            else if(fileInfo->isFile())  // Process a file in archive
            {
                QString fileName=fileInfo->fileName();
                m_archiveInfo->m_path2ActualFile=fileName.left(fileName.lastIndexOf('/')+1);
                m_archiveInfo->m_actualFileName=fileName.right(fileName.length()-fileName.lastIndexOf('/')-1);

                unzipFile(fileInfo->fileName());                
            }
        }
        fileInfoPtrList->clear();
        delete fileInfoPtrList;
    }
}

void UnzipThread::unzipFile(QString archiveFilePath) const
{
    checkThatOuputPathExist(archiveFilePath);
    if( m_archiveInfo->doesFileFilterMatch(archiveFilePath) )
    {        
        updateActualFileInArchiveInfo(archiveFilePath);
        const_cast<UnzipThread*>(this)->checkWaitConditions();
        if( m_archiveInfo->isFileActionValid() )
            performFileAction(archiveFilePath);
    }
}

void UnzipThread::checkThatOuputPathExist(QString archiveFilePath) const
{
    QString destinationDirectory=QString("%1\\%2").arg(getOutputDirectory()).arg(archiveFilePath);
    destinationDirectory=cleanPathName(destinationDirectory);
    destinationDirectory=destinationDirectory.left(destinationDirectory.lastIndexOf("\\")+1);
    QDir dirDest(destinationDirectory);
    if(!dirDest.exists() && (m_archiveInfo->m_unzipFlags & UnzipDialog::UnzipPath))
        dirDest.mkpath(destinationDirectory);
}

QString UnzipThread::getOutputDirectory() const
{
    QString destinationDirectory=m_archiveInfo->m_to;

    if(m_archiveInfo->m_unzipFlags & UnzipDialog::UnzipInSeparateSubDirs)
        destinationDirectory+=QString("\\%2").arg(m_archiveInfo->m_actualArchive);

    return destinationDirectory;
}

void UnzipThread::updateActualFileInArchiveInfo(QString archiveFilePath) const
{
    QFile outputFile(makeOutFileName(archiveFilePath));
    m_archiveInfo->m_destFileName=outputFile.fileName();
    m_archiveInfo->m_destFileSize=outputFile.size();
    m_archiveInfo->m_destFileLastModification=QFileInfo(outputFile).lastModified().toTime_t();

    Archive arch(m_archiveInfo);
    QString archive=m_archiveInfo->m_path2Archive+m_archiveInfo->m_actualArchive;
    m_archiveInfo->m_actualFileLastModification=arch.fileLastModificationTime(archive, archiveFilePath);
    m_archiveInfo->m_actualFileSize=arch.fileSize(archive, archiveFilePath);
}

QString UnzipThread::makeOutFileName(QString archiveFilePath) const
{
    QString destinationDirectory=getOutputDirectory();
    QString outputFilePath;

    if(m_archiveInfo->m_unzipFlags & UnzipDialog::UnzipPath)
    {
        outputFilePath=QString("%1\\%2").arg(destinationDirectory).arg(archiveFilePath);
    }
    else
    {
        outputFilePath=archiveFilePath.right(archiveFilePath.length()-archiveFilePath.lastIndexOf("/")-1);
        outputFilePath=QString("%1\\%2").arg(destinationDirectory).arg(outputFilePath);
    }
    return cleanPathName(outputFilePath);
}

void UnzipThread::performFileAction(QString archiveFilePath) const
{
    const FileActionHandler* fah=m_fileActionFactory->getFileActionHandler(m_archiveInfo->m_fileAction);
    fah->doFileAction(archiveFilePath, m_archiveInfo->m_destFileName);
}

QString UnzipThread::cleanPathName(QString pathName) const
{
    pathName.replace('\\','/'); //ToDo: Use QRegExp...
    pathName.replace("//","\\");
    pathName.replace('/','\\');
    return pathName;
}
