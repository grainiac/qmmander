/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Strategies what to do with a FileAction for Zip und Unzip operations.
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
*************************************************** *************************/
#include "fileactionhandlers.h"
#include "archive.h"
#include "archiveinfo.h"

FileActionHandler::FileActionHandler(const FileActionFactory* fileActionFactory)
:   m_fileActionFactory(fileActionFactory)
{
}

const FileActionFactory* FileActionHandler::getFileActionFactory() const
{
    return m_fileActionFactory;
}

UnzipFileActionHandler::UnzipFileActionHandler(const FileActionFactory* fileActionFactory)
:   FileActionHandler(fileActionFactory)
{
}

void UnzipFileActionHandler::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    ArchiveInfo* archiveInfo=const_cast<ArchiveInfo*>(getFileActionFactory()->getArchiveInfo());
    Archive arch(archiveInfo);
    arch.extractFile(sourceFilePath, destinationfilePath);
    archiveInfo->resetSingleExecutableFileAction();
    archiveInfo->m_filesProcessed++;
}

QString UnzipFileActionHandler::getAutoRenamedFileName(QString filePath) const
{
    QFile file(filePath);
    bool bExists=file.exists();
    QString renamedFilePathName=file.fileName();
    QString strSuffix=QFileInfo(file).suffix();
    int iCount=1;

    while(bExists)
    {
        if(strSuffix.length())
        {
            if(iCount==1)
            {
                int iPos=renamedFilePathName.indexOf(strSuffix)-1;
                renamedFilePathName=renamedFilePathName.left(iPos);
                renamedFilePathName+=QString("(%1).").arg(iCount)+strSuffix;
            }
            else
            {
                QString strSearch4=QString("(%1).%2").arg(iCount-1).arg(strSuffix);
                int iPos=renamedFilePathName.indexOf(strSearch4);
                renamedFilePathName=renamedFilePathName.left(iPos);
                renamedFilePathName+=QString("(%1).").arg(iCount)+strSuffix;
            }
        }
        else
        {
            if(iCount==1)
            {
                renamedFilePathName+=QString("(%1)").arg(iCount);
            }
            else
            {
                QString strSearch4=QString("(%1)").arg(iCount-1);
                int iPos=renamedFilePathName.indexOf(strSearch4);
                renamedFilePathName=renamedFilePathName.left(iPos);
                renamedFilePathName+=QString("(%1).").arg(iCount);
            }
        }
        QFile file(renamedFilePathName);
        strSuffix=QFileInfo(file).suffix();
        bExists=file.exists();
        iCount++;
    }
    return renamedFilePathName;
}

void UFAHDefault::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void UFAHRenameDestinationManual::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    QFile file(destinationfilePath);
    if(!file.exists())
        UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void UFAHRenameSourceManual::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    QFile file(destinationfilePath);
    if(!file.exists())
        UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void UFAHRenameDestinationAuto::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    QString strOldName=destinationfilePath;
    QString strNewName=getAutoRenamedFileName(strOldName);
    QFile file(strOldName);
    file.rename(strNewName);
    UnzipFileActionHandler::doFileAction(sourceFilePath, strOldName);
}

void UFAHRenameSourceAuto::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    QString strOldName=destinationfilePath;
    QString strNewName=getAutoRenamedFileName(strOldName);
    UnzipFileActionHandler::doFileAction(sourceFilePath, strNewName);
}

void UFAHOverWriteOlderAndSameAge::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    ArchiveInfo* archiveInfo=const_cast<ArchiveInfo*>(getFileActionFactory()->getArchiveInfo());
    if(archiveInfo->m_actualFileLastModification >= archiveInfo->m_destFileLastModification)
        UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void UFAHCopyLargerOverWriteSmaller::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    ArchiveInfo* archiveInfo=const_cast<ArchiveInfo*>(getFileActionFactory()->getArchiveInfo());
    if(archiveInfo->m_actualFileSize > archiveInfo->m_destFileSize)
        UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void UFAHCopySmallerOverWriteLarger::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    ArchiveInfo* archiveInfo=const_cast<ArchiveInfo*>(getFileActionFactory()->getArchiveInfo());
    if(archiveInfo->m_actualFileSize < archiveInfo->m_destFileSize)
        UnzipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}


ZipFileActionHandler::ZipFileActionHandler(const FileActionFactory* fileActionFactory)
:   FileActionHandler(fileActionFactory)
{
}

ZipFileActionHandler::~ZipFileActionHandler()
{
}

void ZipFileActionHandler::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    dynamic_cast<const ZipFileActionFactory*>(getFileActionFactory())->
            getArchive()->archiveFile(sourceFilePath, destinationfilePath);
    ArchiveInfo* archiveInfo=const_cast<ArchiveInfo*>(getFileActionFactory()->getArchiveInfo());
    archiveInfo->resetSingleExecutableFileAction();
    archiveInfo->m_filesProcessed++;
}

void ZFAHDefault::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
    ZipFileActionHandler::doFileAction(sourceFilePath, destinationfilePath);
}

void ZFAHRenameDestinationManual::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHRenameSourceManual::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHRenameDestinationAuto::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHRenameSourceAuto::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHOverWriteOlderAndSameAge::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHCopyLargerOverWriteSmaller::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}

void ZFAHCopySmallerOverWriteLarger::doFileAction(QString sourceFilePath, QString destinationfilePath) const
{
}
