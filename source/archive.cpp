/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A small wrapper around the PhysicsFS API.
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
#include "archive.h"
#include "archiveinfo.h"
#include "PhysFS/physfs.h"
#include "ziputils.h"

void qDebugPhysicsFS(QString physFSFunctionWhereErrorOccured, QString errorMsg);

Archive::Archive(ArchiveInfo * const archivInfo, int bufferSizeInKiBytes)
:   m_archiveInfo(archivInfo),
    m_zipHandle(0),
    m_archiveFormatIs7Zip(false)
{
    if(bufferSizeInKiBytes<=0 || bufferSizeInKiBytes>1024) // size of buffer [64KB-1MB]
        m_bufferSize=1024*64;
    else
        m_bufferSize=1024*bufferSizeInKiBytes;
    m_buffer=new char[m_bufferSize];
}

Archive::~Archive()
{
    delete [] m_buffer;
}

int Archive::initUnzip()
{
    for(int i=0; i<m_archiveInfo->m_selectedFiles.count(); i++)
    {
        WinFileInfo fileInfo=m_archiveInfo->m_selectedFiles.at(i);
        if(fileInfo.suffix().toUpper()=="7Z" ||
           fileInfo.suffix().toUpper()=="ZIP" )
        {
            if(!initPhysFS())
                return 0;

            QString archiveNamePathName=QString("%1\\%2").arg(fileInfo.path())
                                       .arg(fileInfo.fileName());

            int iRet=PHYSFS_addToSearchPath(archiveNamePathName.toStdString().c_str(), 0);
            if(!iRet)
            {
                qDebugPhysicsFS("PHYSFS_addToSearchPath in Archive::initUnzip()", PHYSFS_getLastError());
                return 0;
            }

            if(fileInfo.suffix().toUpper()=="7Z")
                m_archiveFormatIs7Zip=true;

            scanArchive("");

            if(!deinitPhysFS())
                return 0;
        }
        else // a file or directory in an archive
        {
            if(!initPhysFS())
                return 0;

            if(fileInfo.archive().toUpper().contains(".7Z"))
                m_archiveFormatIs7Zip=true;

            QString archiveNamePathName=fileInfo.path()+fileInfo.archive();
            int iRet=PHYSFS_addToSearchPath(archiveNamePathName.toStdString().c_str(), 0);
            if(!iRet)
            {
                qDebugPhysicsFS("PHYSFS_addToSearchPath in Archive::initUnzip()", PHYSFS_getLastError());
                return 0;
            }

            if(fileInfo.isDir())
                scanArchive(fileInfo.fileName());
            else
                scanArchiveFile(fileInfo.fileName());

            if(!deinitPhysFS())
                return 0;
        }
    }
    return 1;
}

int Archive::initZip()
{
    m_archivePathName="";
    if(m_archiveInfo->m_selectedFiles.count()>0)
    {
        for(int i=0; i<m_archiveInfo->m_selectedFiles.count(); i++)
        {
            WinFileInfo fileInfo=m_archiveInfo->m_selectedFiles.at(i);
            if( m_archiveInfo->isValidDirectory(&fileInfo) )
                scanDirectory(fileInfo.filePath());
            else
                scanFile(fileInfo);
        }
        if(m_archiveInfo->m_fileCount>0)
            return 1;
    }
    return 0;
}

void Archive::scanDirectory(QString directoryPath)
{
    WinFileInfoPtrList* elements=WinFileInfo::listFiles(directoryPath);
    for(int i=0; i<elements->count(); i++)
    {
        WinFileInfo* fileInfo=elements->at(i);
        if( m_archiveInfo->isValidDirectory(fileInfo) &&
           (m_archiveInfo->m_zipFlags & ZipDialog::ZipRecursive))
        {
            scanDirectory(fileInfo->filePath());
        }
        else
        {
            scanFile(*fileInfo);
        }
    }
    delete elements;
}

void Archive::scanFile(const WinFileInfo& fileInfo)
{
    m_archiveInfo->m_totalSize+=fileInfo.size();
    m_archiveInfo->m_fileCount++;
}

void Archive::scanArchive(QString archiveDirectoryPath)
{
    char **rc = PHYSFS_enumerateFiles(archiveDirectoryPath.toStdString().c_str());
    char **i;

    for (i = rc; *i != NULL; i++)
    {
        QString strFile=(archiveDirectoryPath.length()>0) ?
                        QString("%1/%2").arg(archiveDirectoryPath).arg(*i) :
                        QString("%1").arg(*i);
        bool isDir=!!PHYSFS_isDirectory(strFile.toStdString().c_str());
        if(isDir)
            scanArchive(strFile);
        else
            scanArchiveFile(strFile);
    }
    PHYSFS_freeList(rc);
}

void Archive::scanArchiveFile(QString archiveFilePath)
{
    PHYSFS_File* file=PHYSFS_openRead(archiveFilePath.toStdString().c_str());

    if(!file)
    {
        QString strInfo=QString("PHYSFS_openRead on file \"%1\" in Archive::scanArchiveFile()").arg(archiveFilePath);
        qDebugPhysicsFS(strInfo, PHYSFS_getLastError());
    }
    else
    {
        m_archiveInfo->m_totalSize+=PHYSFS_fileLength(file);
        m_archiveInfo->m_fileCount++;
        if(!PHYSFS_close(file))
            qDebugPhysicsFS("PHYSFS_close in Archive::scanArchiveFile()", PHYSFS_getLastError());
    }
}

void Archive::archiveFile(QString sourceFilePath, QString archiveFilePath, QString archivePassword)
{
    if(m_archivePathName=="")
    {
        m_archivePathName=m_archiveInfo->m_to;
        //m_zipHandle=CreateZip(m_archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());

        if(m_zipHandle)
        {
            CloseZip(m_zipHandle);
            m_zipHandle=0;
        }

        QFile file(m_archivePathName);
        bool bAppendToExistingZip=file.exists();

        if(!bAppendToExistingZip)
            m_zipHandle=CreateZip(m_archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    }
    else if(m_archivePathName!=m_archiveInfo->m_to)
    {
        m_archivePathName=m_archiveInfo->m_to;

        if(m_zipHandle)
        {
            CloseZip(m_zipHandle);
            m_zipHandle=0;
        }

        QFile file(m_archivePathName);
        bool bAppendToExistingZip=file.exists();

        if(!bAppendToExistingZip)
            m_zipHandle=CreateZip(m_archivePathName.toStdWString().c_str(), archivePassword.toStdString().c_str());
    }

    ZRESULT res=0;
    if(!m_zipHandle)
        res = AddFileToZip(m_archivePathName.toStdWString().c_str(),
                           archiveFilePath.toStdWString().c_str(),
                           sourceFilePath.toStdWString().c_str());
    else
        res = ZipAdd(m_zipHandle, archiveFilePath.toStdWString().c_str(), sourceFilePath.toStdWString().c_str());

    m_archiveInfo->m_bytesProcessed+=QFile(sourceFilePath).size();
    ZipUtils::qDebugZipErrors(res);
}

void Archive::closeOpenZipHandle()
{
    CloseZip(m_zipHandle);
    m_zipHandle=0;
}

void Archive::removeFile(QString path2Archive, QString archiveFilePath)
{
    const TCHAR* archive = path2Archive.toStdWString().c_str();
    const TCHAR* file2RemoveInArchive = archiveFilePath.toStdWString().c_str();
    ZRESULT res = RemoveFileFromZip(archive, file2RemoveInArchive);
    ZipUtils::qDebugZipErrors(res);
}

void Archive::extractFile(QString archiveFilePath, QString destinationFilePath)
{    
    extractFileWithPhysFS(archiveFilePath, destinationFilePath);
    /*if(m_bArchiveFormatIs7Zip)
        extractFileWithPhysFS(archiveFilePath, destinationFilePath);
    else
        extractFileWithZipUtils(archiveFilePath, destinationFilePath);*/
}

void Archive::extractFileWithPhysFS(QString archiveFilePath, QString destinationFilePath)
{
    QFile file(destinationFilePath);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug("Archive: Can't open output file for writing!");
    }
    else
    {
        PHYSFS_file* pArchiveFile=openArchiveFile(archiveFilePath);
        if(pArchiveFile)
        {
            int iReadCount=0;
            int iReadCountTotal=0;

            do
            {
                iReadCount=PHYSFS_read(pArchiveFile, (void*)m_buffer, sizeof(char), m_bufferSize);
                if(iReadCount==-1)
                    qDebugPhysicsFS("PHYSFS_read in Archive::extractFile()", PHYSFS_getLastError());
                else
                    file.write(m_buffer, iReadCount);
                iReadCountTotal+=iReadCount;
            }
            while(iReadCount==m_bufferSize);

            m_archiveInfo->m_bytesProcessed+=iReadCountTotal;

            closeArchiveFile(pArchiveFile);
            file.close();

            setFileTimes(file.fileName(), m_archiveInfo->m_actualFileLastModification);
        }
    }
}

void Archive::extractFileWithZipUtils(QString archiveFilePath, QString destinationFilePath)
{
    QString archive=QString("%1%2").arg(m_archiveInfo->m_path2Archive)
                                   .arg(m_archiveInfo->m_actualArchive);

    qDebug(QString("%1 --- %2").arg(archiveFilePath).arg(destinationFilePath).toStdString().c_str());
    ZipUtils::extractFile(archive,
                          archiveFilePath,
                          "",
                          destinationFilePath);
}

void Archive::setFileTimes(QString filePath, time_t time)
{
    HANDLE hFile;
    if ((hFile = CreateFile(filePath.toStdWString().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,0 , OPEN_EXISTING,0,0)) != INVALID_HANDLE_VALUE)
    {
        qint64 ll = Int32x32To64(time, 10000000) + 116444736000000000LL;
        FILETIME ft;
        ft.dwLowDateTime = (DWORD)ll;
        ft.dwHighDateTime = ll >> 32;

        SetFileTime(hFile, &ft, &ft, &ft);
        CloseHandle(hFile);
    }
}

WinFileInfoPtrList* Archive::listFiles(QString path2Archive, QString archiveName, QString pathInArchive)
{
    Archive arch(0);
    if(!arch.initPhysFS())
        return 0;

    WinFileInfoPtrList* elements=new WinFileInfoPtrList();

    QString archiveNamePathName=path2Archive+archiveName;
    int iRet=PHYSFS_addToSearchPath(archiveNamePathName.toStdString().c_str(), 0);
    if(!iRet)
    {
        qDebugPhysicsFS("PHYSFS_addToSearchPath in Archive::listFiles()", PHYSFS_getLastError());
        arch.deinitPhysFS();
        return 0;
    }

    char **rc = PHYSFS_enumerateFiles(pathInArchive.toStdString().c_str());
    char **i;

    for (i = rc; *i != NULL; i++)
    {
        QString strFile=(pathInArchive.length()>0) ?
                        QString("%1/%2").arg(pathInArchive).arg(*i) :
                        QString("%1").arg(*i);

        WinFileInfo* pFI=0;
        bool    bIsDir=!!PHYSFS_isDirectory(strFile.toStdString().c_str());
        bool    bFileReadError=false;
        quint64 ui64Size=0;
        qint64 i64LastModTime=PHYSFS_getLastModTime(strFile.toStdString().c_str());

        if(!bIsDir)
        {
            PHYSFS_File* file=PHYSFS_openRead(strFile.toStdString().c_str());
            if(!file)
            {
                QString strInfo=QString("PHYSFS_openRead on file \"%1\" in Archive::listFiles()").arg(strFile);
                qDebugPhysicsFS(strInfo, PHYSFS_getLastError());
                bFileReadError=true;
            }
            else
            {
                ui64Size=PHYSFS_fileLength(file);
                if(!PHYSFS_close(file))
                    qDebugPhysicsFS("PHYSFS_close in Archive::listFiles()", PHYSFS_getLastError());
            }
        }

        if(!bFileReadError)
        {
            pFI=new WinFileInfo(archiveName,
                            strFile,
                            bIsDir,
                            ui64Size,
                            i64LastModTime);
            pFI->setPath2File(path2Archive+"\\");
            elements->push_back(pFI);
        }
    }
    PHYSFS_freeList(rc);

    // add a ".." entry for folder-up navigation
    LARGE_INTEGER t;
    QDateTime time;
    t.HighPart=0;
    t.LowPart=time.toTime_t();
    WinFileInfo* pFI=new WinFileInfo("",
                                     "..",
                                     true,
                                     0,
                                     t.QuadPart);
    elements->push_back(pFI);

    if(!arch.deinitPhysFS())
    {
        elements->clear();
        delete elements;
        return 0;
    }

    return elements;
}

bool Archive::exists(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    return ZipUtils::fileExists(archivePathName, archiveFilePath, archivePassword);
}

qint64 Archive::fileSize(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    return ZipUtils::fileSize(archivePathName, archiveFilePath, archivePassword, false);
}

qint64 Archive::fileSizeCompressed(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    return ZipUtils::fileSize(archivePathName, archiveFilePath, archivePassword, true);
}

time_t Archive::fileLastModificationTime(QString archivePathName, QString archiveFilePath, QString archivePassword)
{
    return ZipUtils::fileLastModificationTime(archivePathName, archiveFilePath, archivePassword);
}

PHYSFS_File* Archive::openArchiveFile(QString archiveFilePath, bool trueWriteModeDefaultReadMode)
{
    if(!initPhysFS())
        return 0;

    QString archiveFilePathName=QString("%1%2").arg(m_archiveInfo->m_path2Archive)
                                               .arg(m_archiveInfo->m_actualArchive);

    if(m_archiveInfo->m_trueZipModeFalseUnzipMode)
    {
        int iRet=PHYSFS_mount(archiveFilePathName.toStdString().c_str(), "", 0);
        if(!iRet)
        {
            qDebugPhysicsFS("PHYSFS_mount in Archive::openArchiveFile()", PHYSFS_getLastError());
            return 0;
        }
    }
    else
    {
        int iRet=PHYSFS_addToSearchPath(archiveFilePathName.toStdString().c_str(), 0);
        if(!iRet)
        {
            qDebugPhysicsFS("PHYSFS_addToSearchPath in Archive::openArchiveFile()", PHYSFS_getLastError());
            return 0;
        }
    }
    PHYSFS_File* file=0;
    if(trueWriteModeDefaultReadMode)
    {
        file=PHYSFS_openWrite(archiveFilePath.toStdString().c_str());
        if(!file)
        {
            QString strInfo=QString("PHYSFS_openWrite on file %1 in Archive::openArchiveFile()#1").arg(archiveFilePath);
            qDebugPhysicsFS(strInfo, PHYSFS_getLastError());
            return 0;
        }
    }
    else
    {
        file=PHYSFS_openRead(archiveFilePath.toStdString().c_str());
        if(!file)
        {
            QString strInfo=QString("PHYSFS_openRead on file %1 in Archive::openArchiveFile()#2").arg(archiveFilePath);
            qDebugPhysicsFS(strInfo, PHYSFS_getLastError());
            return 0;
        }
    }
    return file;
}

void Archive::closeArchiveFile(PHYSFS_File* file)
{
    if(!PHYSFS_close(file))
        qDebugPhysicsFS("PHYSFS_close in Archive::closeArchiveFile()", PHYSFS_getLastError());

    deinitPhysFS();
}

int Archive::initPhysFS()
{
    int iRet=PHYSFS_init(QCoreApplication::arguments().at(0).toStdString().c_str());
    if(!iRet)
        qDebugPhysicsFS("PHYSFS_init", "ERROR: Cannot initialize PhysicsFS.");
    return iRet;
}

int Archive::deinitPhysFS()
{
    int iRet=PHYSFS_deinit();
    if(!iRet)
        qDebugPhysicsFS("PHYSFS_deinit", "ERROR: Cannot deinitialize PhysicsFS.");
    return iRet;
}

void qDebugPhysicsFS(QString physFSFunctionWhereErrorOccured, QString errorMsg)
{
    if(errorMsg!="")
    {
        QString strOutput=QString("PhysFS-Function: %1 PhysFS-Error: %2").arg(physFSFunctionWhereErrorOccured).arg(errorMsg);
        qDebug(strOutput.toStdString().c_str());
    }
}
