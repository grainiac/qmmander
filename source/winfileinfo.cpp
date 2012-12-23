/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A class providing file information similar to QFileInfo.
**
** The following functions were taken from the Qt sources:
**
** QIcon   getWinIcon() const;
** QPixmap convertHIconToPixmap( const HICON icon);
** QImage  qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h);
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
#include "winfileinfo.h"

#include <QtCore>
#include <QIcon>
#include <QPixmapCache>

#include <winioctl.h>
#include <wchar.h>
#include <time.h>
#include <shlwapi.h>
#include "winnt.h"

#define SHGFI_ADDOVERLAYS 0x000000020

enum
{ TARGET_INVALID=-1,
  TARGET_SYMLINK,
  TARGET_JUNCTION,
  TARGET_MOUNTPOINT
};

void    getLinkTarget(QString fileName, DWORD fileAttributes, QString& target, int& targetType);
QImage  qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h);
QPixmap convertHIconToPixmap( const HICON icon);

WinFileInfo::WinFileInfo()
{
}

WinFileInfo::WinFileInfo(const Win32FileInfo& fi)
:   fileInfo_(fi),
    isSmbPath_(false),
    isArchiveFile_(false)
{
}

WinFileInfo::WinFileInfo(QString SMBPath, bool isSMBRoot)
:   smbPath_(SMBPath),
    isSmbPath_(true),
    isSmbRoot_(isSMBRoot),
    isArchiveFile_(false)
{
    FILETIME ft;
    ft.dwLowDateTime=0;
    ft.dwHighDateTime=0;

    Win32FileInfo fi;
    fi.dwFileAttributes=0;
    fi.ftCreationTime=ft;
    fi.ftLastAccessTime=ft;
    fi.ftLastWriteTime=ft;
    fi.nFileSizeHigh=0;
    fi.nFileSizeLow=0;
    fi.dwReserved0=0;
    fi.dwReserved1=0;

    fileInfo_=fi;
}

WinFileInfo::WinFileInfo(QString archive, QString fileName, bool isDir, quint64 fileSize, qint64 lastModTime)
:   archive_(archive),
    isSmbPath_(false),
    isArchiveFile_(true),
    isArchiveDirectory_(isDir)
{
    FILETIME ft;
    ULARGE_INTEGER ul;
    ul.QuadPart=lastModTime;
    ft.dwLowDateTime=ul.LowPart;
    ft.dwHighDateTime=ul.HighPart;

    Win32FileInfo stFi;
    stFi.dwFileAttributes=0;
    stFi.ftCreationTime=ft;
    stFi.ftLastAccessTime=ft;
    stFi.ftLastWriteTime=ft;
    ul.QuadPart=fileSize;
    stFi.nFileSizeHigh=ul.HighPart;
    stFi.nFileSizeLow=ul.LowPart;

    stFi.dwReserved0=0;
    stFi.dwReserved1=0;

    fileInfo_=stFi;

    wcscpy(fileInfo_.cFileName,fileName.toStdWString().c_str());
}

WinFileInfo::~WinFileInfo()
{
}

QIcon WinFileInfo::getWinIcon() const
{
    QIcon retIcon;
    QString fileExtension = suffix().toUpper();
    fileExtension.prepend( QLatin1String(".") );

    QString key;
    if (isFile() && !isExecutable() && !isSymLink())
        key = QLatin1String("qt_") + fileExtension;

    QPixmap pixmap;
    if (!key.isEmpty()) {
        QPixmapCache::find(key, pixmap);
    }

    if (!pixmap.isNull()) {
        retIcon.addPixmap(pixmap);
        if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
            retIcon.addPixmap(pixmap);
        return retIcon;
    }

    /* We don't use the variable, but by storing it statically, we
     * ensure CoInitialize is only called once. */
    static HRESULT comInit = CoInitialize(NULL);
    Q_UNUSED(comInit);

    SHFILEINFO info;
    unsigned long val = 0;

    //Get the small icon
    val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS);
    if (val) {
        if (isDir() && !isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
            QPixmapCache::find(key, pixmap);
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (QPixmapCache::find(key + QLatin1Char('l'), pixmap))
                    retIcon.addPixmap(pixmap);
                DestroyIcon(info.hIcon);
                return retIcon;
            }
        }
        if (pixmap.isNull()) {
            pixmap = convertHIconToPixmap(info.hIcon);
            if (!pixmap.isNull()) {
                retIcon.addPixmap(pixmap);
                if (!key.isEmpty())
                    QPixmapCache::insert(key, pixmap);
            }
            else {
              qWarning("QFileIconProviderPrivate::getWinIcon() no small icon found");
            }
        }
        DestroyIcon(info.hIcon);
    }

    //Get the big icon
    val = SHGetFileInfo((const WCHAR *)QDir::toNativeSeparators(filePath()).utf16(), 0, &info,
                        sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_ADDOVERLAYS);
    if (val) {
        if (isDir() && !isRoot()) {
            //using the unique icon index provided by windows save us from duplicate keys
            key = QString::fromLatin1("qt_dir_%1").arg(info.iIcon);
        }
        pixmap = convertHIconToPixmap(info.hIcon);
        if (!pixmap.isNull()) {
            retIcon.addPixmap(pixmap);
            if (!key.isEmpty())
                QPixmapCache::insert(key + QLatin1Char('l'), pixmap);
        }
        else {
            qWarning("QFileIconProviderPrivate::getWinIcon() no large icon found");
        }
        DestroyIcon(info.hIcon);
    }
    return retIcon;
}

void WinFileInfo::executeRegisteredShellActionForFile(QString filePath)
{
    QFileInfo fi(filePath);

    if ( (fi.suffix().toUpper()=="EXE") || (fi.suffix().toUpper()=="COM") || (fi.suffix().toUpper()=="BAT") )
    {
        QString workingDirectory=fi.filePath().remove(fi.fileName()).replace("/", "\\");
        workingDirectory=workingDirectory.left(workingDirectory.lastIndexOf("\\"));
        ShellExecute(0,
                     TEXT("open"),
                     filePath.toStdWString().c_str(),
                     TEXT(""),
                     workingDirectory.toStdWString().c_str(),
                     SW_SHOW);
    }
    else
    {
        SHELLEXECUTEINFO ShExecInfo;
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = NULL;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = NULL;
        ShExecInfo.lpFile = filePath.toStdWString().c_str();
        ShExecInfo.lpParameters = NULL;
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_SHOW;
        ShExecInfo.hInstApp = NULL;
        ShellExecuteEx(&ShExecInfo);
    }
}

void WinFileInfo::executeRegisteredShellAction()
{
    executeRegisteredShellActionForFile(filePath());
}

time_t WinFileInfo::timeFromSystemTime(const SYSTEMTIME& sysTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    tm.tm_year = sysTime.wYear - 1900;
    tm.tm_mon = sysTime.wMonth - 1;
    tm.tm_mday = sysTime.wDay;

    tm.tm_hour = sysTime.wHour;
    tm.tm_min = sysTime.wMinute;
    tm.tm_sec = sysTime.wSecond;

    return mktime(&tm);
}

void WinFileInfo::timetToFileTime(time_t t, LPFILETIME pft)
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >>32;
}

void WinFileInfo::setData(const Win32FileInfo& fi)
{
    fileInfo_=fi;
}

Win32FileInfo WinFileInfo::data()
{
    return fileInfo_;
}

void WinFileInfo::setPath2File(QString strPath)
{
    path_=QString(strPath);
}

QString WinFileInfo::fileName() const
{
    if(isDir() && isSMBPath())
        return smbPath_.right(smbPath_.length()-smbPath_.lastIndexOf('\\')-1);
    #ifdef UNICODE
        return QString::fromUtf16((ushort*)fileInfo_.cFileName);
    #else
        return QString::fromLocal8Bit(m_fileInfo.cFileName);
    #endif
}

QString WinFileInfo::archiveFileName() const
{
    #ifdef UNICODE
        QString strFileName=QString::fromUtf16((ushort*)fileInfo_.cFileName);
    #else
        QString strFileName=QString::fromLocal8Bit(m_fileInfo.cFileName);
    #endif
    int iLastSlashIndex=strFileName.lastIndexOf("/");
    if(iLastSlashIndex!=-1)
        return strFileName.right(strFileName.length()-iLastSlashIndex-1);
    else
        return strFileName;
}

QString WinFileInfo::path() const
{
    if(!isSMBPath())
        return path_;
    else
        return smbPath_;
}

QString WinFileInfo::suffix() const
{
    int lastDot = fileName().lastIndexOf(QLatin1Char('.'));
    if (lastDot == -1)
        return QLatin1String("");
    return fileName().mid(lastDot + 1);
}

QString WinFileInfo::archive() const
{
    return archive_;
}

QString WinFileInfo::alternateFileName() const
{
    #ifdef UNICODE
        return QString::fromUtf16((ushort*)fileInfo_.cAlternateFileName);
    #else
        return QString::fromLocal8Bit(m_fileInfo.cFileName);
    #endif
}

quint64 WinFileInfo::size() const
{
    return dword2qint64(fileInfo_.nFileSizeLow, fileInfo_.nFileSizeHigh);
}

qint32 WinFileInfo::attributes() const
{
    return fileInfo_.dwFileAttributes;
}

bool WinFileInfo::setAttributes(DWORD attributes)
{
    return SetFileAttributes(filePath().toStdWString().c_str(), attributes);
}

time_t WinFileInfo::creationTime() const
{
    return fileTime2IntegerTime(&fileInfo_.ftCreationTime);
}

time_t WinFileInfo::lastAccessTime() const
{
    return fileTime2IntegerTime(&fileInfo_.ftLastAccessTime);
}

time_t WinFileInfo::lastModificationTime() const
{
    return fileTime2IntegerTime(&fileInfo_.ftLastWriteTime);
}

time_t WinFileInfo::fileTime2IntegerTime(const FILETIME* pFT) const
{
    if(isArchiveFile())
    {
        // because PhysicsFS already returnes a time_t....
        return static_cast<time_t>(dword2qint64(pFT->dwLowDateTime, pFT->dwHighDateTime));
    }
    else
    {
        SYSTEMTIME st;
        FileTimeToSystemTime(pFT, &st);
        return timeFromSystemTime(st);
    }
}

bool WinFileInfo::isArchiveFile() const
{
    return isArchiveFile_;
}

bool WinFileInfo::isDir() const
{
   if( fileInfo_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ||
       isSmbPath_==true ||
      (isArchiveFile_ && isArchiveDirectory_) )
       return true;
   return false;
}

bool WinFileInfo::isFile() const
{
   if(isDir())
       return false;
   return true;
}

bool WinFileInfo::isExecutable() const
{
    if(suffix().toUpper()=="EXE" || suffix().toUpper()=="COM")
        return true;
    return false;
}

bool WinFileInfo::isShellLink() const
{
    if(suffix().toUpper()=="LNK")
        return true;
    return false;
}

bool WinFileInfo::isSymLink() const
{
    if(fileInfo_.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
    {
        // Get more information about link with getLinkTarget(...)
        return true;
    }
    return false;
}

bool WinFileInfo::isRoot() const
{
    if(!isSmbPath_)
    {
        return false;
    }
    else
    {
        return isSmbRoot_;
    }
}

bool WinFileInfo::isSMBPath() const
{
    return isSmbPath_;
}

QString WinFileInfo::resolveLnkFile()
{
    QFileInfo info(filePath());
    return info.symLinkTarget();
}

qint64 WinFileInfo::dword2qint64(DWORD low, DWORD high) const
{
    LARGE_INTEGER large;
    large.LowPart=low;
    large.HighPart=high;
    return large.QuadPart;
}

QString WinFileInfo::filePath() const
{
    QString filePath=path_+"\\"+fileName();
    if(!isSymLink())
        return filePath;
    else        
    {
        QString linkTarget;
        int     targetType;
        getLinkTarget(filePath, fileInfo_.dwFileAttributes, linkTarget, targetType);
        linkTarget.remove(0,4);
        return linkTarget;
    }
}

QString WinFileInfo::getDriveFromPath(QString path)
{
    int pos=0;
    if((pos=path.indexOf(":"))>-1)
    {
        return path.left(pos+1);
    }
    return QString("");
}

QString WinFileInfo::markArchiveNameInPath(QString path)
{
    QString newPath=path;
    int pos;
    if(newPath.toUpper().contains(".ZIP")) // if the path contains an archive mark it...
    {
        pos=newPath.toUpper().indexOf(".ZIP");
        pos=newPath.lastIndexOf("\\", pos);
        newPath.insert(pos+1, "<Archive>");
    }
    else if(newPath.toUpper().contains(".7Z"))
    {
        pos=newPath.toUpper().indexOf(".7Z");
        pos=newPath.lastIndexOf("\\", pos);
        newPath.insert(pos+1, "<Archive>");
    }
    return newPath;
}

QStringList WinFileInfo::getDriveList()
{
    char  buffer[108]; // 26*4+4extra
    memset(&buffer[0], 0, sizeof(char)*108);
    GetLogicalDriveStringsA(108, &buffer[0]);
    for(int i=0; i<108; i++)
        if(buffer[i]=='\0')
           buffer[i]='#';

    QString     drives(QByteArray::fromRawData(&buffer[0],108));
    QRegExp     drivesQuery("([A-Z]:\\\\#)", Qt::CaseSensitive, QRegExp::RegExp2);
    QStringList listDrives;

    int pos=0;
    while((pos=drivesQuery.indexIn(drives, pos))!=-1)
    {
        QString drive=drivesQuery.cap(1);
        drive=drive.left(drive.indexOf('\\'));
        listDrives.push_back(drive);
        pos+=drivesQuery.matchedLength();
    }
    return listDrives;
}

WinFileInfoPtrList* listFilesFor(QString path)
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.
    QString searchPath=path+"\\*";
    QMap<QString, WinFileInfo*> fileName2WinFileInfo;
    WinFileInfoPtrList* fileList=new WinFileInfoPtrList();

    // Find the first file in the directory.
    hFind = FindFirstFile(searchPath.toStdWString().c_str(), &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
        return fileList;

    // List all the files in the directory with some info about them.
    bool isSmbShareRootDir=WinFileInfo::isSmbShareRoot(path);
    do
    {
        WinFileInfo* fileInfo=new WinFileInfo(ffd);
        fileName2WinFileInfo[fileInfo->fileName()]=fileInfo;

        if(isSmbShareRootDir && QString::fromStdWString(ffd.cFileName)=="..")
            fileInfo->setPath2File(path.left(path.lastIndexOf('\\')));
        else
            fileInfo->setPath2File(path);

        fileList->push_back(fileInfo);
    }
    while (FindNextFile(hFind, &ffd) != 0);
    FindClose(hFind);

    // If this is a root-dir delete all dot and dotdot entrys
    WCHAR tempBuffer[MAX_PATH]=TEXT("");
    WCHAR* buffer;
    buffer = tempBuffer;
    PathCanonicalize(buffer, path.toStdWString().c_str());
    QString canonicalizedPath=QString::fromUtf16((ushort*)buffer);

    if( (canonicalizedPath.length()<=3 && canonicalizedPath.at(1)==':' ) || // root dir on a drive
            WinFileInfo::isSmbRoot(canonicalizedPath) ) // smb root
    {
        WinFileInfo* fileInfo=fileName2WinFileInfo["."];
        if(fileInfo)
        {
            fileList->removeAll(fileInfo);
            delete fileInfo;
        }
        fileInfo=fileName2WinFileInfo[".."];
        if(fileInfo)
        {
            fileList->removeAll(fileInfo);
            delete fileInfo;
        }
    }

    return fileList;
}

WinFileInfoPtrList* listFilesForSmbRoot(QString path)
{
    NETRESOURCE smbRootResource;
    WCHAR provider[256]=TEXT("Microsoft Windows Network\0");
    WCHAR remoteName[256]=TEXT("\0");
    wcscpy(remoteName, path.toStdWString().c_str());
    smbRootResource.lpProvider=provider;
    smbRootResource.lpRemoteName=remoteName;
    smbRootResource.lpLocalName=0;
    smbRootResource.lpComment=0;
    smbRootResource.dwDisplayType=2;
    smbRootResource.dwScope=2;
    smbRootResource.dwType=0;
    smbRootResource.dwUsage=2;

    WinFileInfoPtrList* fileList=new WinFileInfoPtrList();
    DWORD dwResult, dwResultEnum;
    HANDLE hEnum;
    DWORD cbBuffer = 16000;     // 16K is a good size
    DWORD cEntries = (DWORD)-1; // enumerate all possible entries
    LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures

    dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
                            RESOURCETYPE_ANY,   // all resources
                            0,  // enumerate all resources
                            &smbRootResource,       // NULL first time the function is called
                            &hEnum);    // handle to the resource

    if(dwResult==NO_ERROR)
    {
        lpnrLocal=(LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);

        if(lpnrLocal!=NULL)
        {
            do
            {
                // Initialize the buffer.
                ZeroMemory(lpnrLocal, cbBuffer);

                // Call the WNetEnumResource function to continue the enumeration.
                dwResultEnum = WNetEnumResource(hEnum,  // resource handle
                                                &cEntries,      // defined locally as -1
                                                lpnrLocal,      // LPNETRESOURCE
                                                &cbBuffer);     // buffer size
                // If the call succeeds, loop through the structures.
                if(dwResultEnum==NO_ERROR)
                {
                    for (unsigned int i=0; i < cEntries; i++)
                    {
                        QString remoteName=QString::fromUtf16((ushort*)lpnrLocal[i].lpRemoteName);
                        switch(lpnrLocal[i].dwDisplayType)
                        {
                        case (RESOURCEDISPLAYTYPE_NETWORK):
                           break;
                        case (RESOURCEDISPLAYTYPE_DOMAIN):
                            break;
                        case (RESOURCEDISPLAYTYPE_SERVER):
                            break;
                        case (RESOURCEDISPLAYTYPE_SHARE):
                            fileList->push_back(new WinFileInfo(remoteName, false));
                            break;
                        case (RESOURCEDISPLAYTYPE_FILE):
                            break;
                        case (RESOURCEDISPLAYTYPE_GROUP):
                            break;
                        case (RESOURCEDISPLAYTYPE_GENERIC):
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            while (dwResultEnum != ERROR_NO_MORE_ITEMS);

            // Call the GlobalFree function to free the memory.
            GlobalFree((HGLOBAL) lpnrLocal);

            // Call WNetCloseEnum to end the enumeration.
            WNetCloseEnum(hEnum);
        }
    }
    return fileList;
}

WinFileInfoPtrList* WinFileInfo::listFiles(QString path)
{
    if(isSmbRoot(path))
        return listFilesForSmbRoot(path);
    else
        return listFilesFor(path);
}

bool WinFileInfo::isSmbPath(QString path)
{
    return  path.at(0) == '\\' && path.at(1) == '\\';
}

bool WinFileInfo::isSmbRoot(QString path)
{
    return QFile(path).exists() &&
           path.at(0) == '\\' && path.at(1) == '\\' &&
           path.count('\\') == 2;
}

bool WinFileInfo::isSmbShareRoot(QString path)
{
    return QFile(path).exists() &&
           path.at(0) == '\\' && path.at(1) == '\\' &&
           path.count('\\') == 3;
}

WinFileInfoIndices WinFileInfo::sortFileInfoList(WinFileInfoPtrList const * const list,
                                                 SortOptions sortBy,
                                                 QString filter,
                                                 bool ascending)
{
    QList< QPair<QString, const WinFileInfo*> > dirNames2FileInfoPairList;
    QList< QPair<uint,    const WinFileInfo*> > dirDates2FileInfoPairList;
    QList< QPair<QString, const WinFileInfo*> > dirAttrs2FileInfoPairList;

    QList< QPair<QString, const WinFileInfo*> > fileNames2FileInfoPairList;
    QList< QPair<uint,    const WinFileInfo*> > fileDates2FileInfoPairList;
    QList< QPair<qint64,  const WinFileInfo*> > fileSizes2FileInfoPairList;
    QList< QPair<QString, const WinFileInfo*> > fileDescs2FileInfoPairList;
    QList< QPair<QString, const WinFileInfo*> > fileAttrs2FileInfoPairList;

    WinFileInfoIndices listSorted;
    QList< QPair<QString, const WinFileInfo*> >::iterator it;

    int indexOfDotDot=-1;
    switch(sortBy)
    {
        case WFI_FILENAME:
            for(int i=0; i<list->size(); i++)
            {
                WinFileInfo* fileInfo=list->at(i);
                if(fileInfo->isDir())
                {
                    if(fileInfo->fileName()=="..")
                        indexOfDotDot=i;
                    else
                        dirNames2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(fileInfo->fileName().toUpper(), fileInfo));
                }
                else
                    fileNames2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(fileInfo->fileName().toUpper(), fileInfo));
            }
            qSort(dirNames2FileInfoPairList.begin(), dirNames2FileInfoPairList.end());
            qSort(fileNames2FileInfoPairList.begin(), fileNames2FileInfoPairList.end());

            if(ascending)
            {
                for(int i=0; i<dirNames2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=0; i<fileNames2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileNames2FileInfoPairList.at(i).second)));
            }
            else
            {
                for(int i=dirNames2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=fileNames2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileNames2FileInfoPairList.at(i).second)));
            }
            if(indexOfDotDot>-1)
                listSorted.push_front(indexOfDotDot);
            break;
        case WFI_SIZE:
            for(int i=0; i<list->size(); i++)
            {
                WinFileInfo* fileInfo=list->at(i);
                if(fileInfo->isDir())
                {
                    if(fileInfo->fileName()=="..")
                        indexOfDotDot=i;
                    else
                       dirNames2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(fileInfo->fileName(), fileInfo));
                }
                else
                    fileSizes2FileInfoPairList.push_back(QPair<qint64,  const WinFileInfo*>(fileInfo->size(), fileInfo));
            }
            qSort(dirNames2FileInfoPairList.begin(), dirNames2FileInfoPairList.end());
            qSort(fileSizes2FileInfoPairList.begin(), fileSizes2FileInfoPairList.end());

            if(ascending)
            {
                for(int i=0; i<dirNames2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=0; i<fileSizes2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileSizes2FileInfoPairList.at(i).second)));
            }
            else
            {
                for(int i=0; i<dirNames2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=fileSizes2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileSizes2FileInfoPairList.at(i).second)));
            }
            if(indexOfDotDot>-1)
                listSorted.push_front(indexOfDotDot);
            break;
        case WFI_DATE:
            for(int i=0; i<list->size(); i++)
            {
                WinFileInfo* fileInfo=list->at(i);
                if(fileInfo->isDir())
                    dirDates2FileInfoPairList.push_back(QPair<uint,    const WinFileInfo*>(fileInfo->lastModificationTime(), fileInfo));
                else
                    fileDates2FileInfoPairList.push_back(QPair<uint,    const WinFileInfo*>(fileInfo->lastModificationTime(), fileInfo));
            }
            qSort(dirDates2FileInfoPairList.begin(), dirDates2FileInfoPairList.end());
            qSort(fileDates2FileInfoPairList.begin(), fileDates2FileInfoPairList.end());

            if(ascending)
            {
                for(int i=0; i<dirDates2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirDates2FileInfoPairList.at(i).second)));
                for(int i=0; i<fileDates2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileDates2FileInfoPairList.at(i).second)));
            }
            else
            {
                for(int i=dirDates2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirDates2FileInfoPairList.at(i).second)));
                for(int i=fileDates2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileDates2FileInfoPairList.at(i).second)));
            }
            break;
        case WFI_DESCRIPTION:
            for(int i=0; i<list->size(); i++)
            {
                WinFileInfo* fileInfo=list->at(i);
                if(fileInfo->isDir())
                {
                    if(fileInfo->fileName()=="..")
                        indexOfDotDot=i;
                    else
                       dirNames2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(fileInfo->fileName().toUpper(), fileInfo));
                }
                else
                    fileDescs2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(getFileTypeDescripton(*fileInfo).toUpper(), fileInfo));
            }
            qSort(dirNames2FileInfoPairList.begin(), dirNames2FileInfoPairList.end());
            qSort(fileDescs2FileInfoPairList.begin(), fileDescs2FileInfoPairList.end());

            if(ascending)
            {
                for(int i=0; i<dirNames2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=0; i<fileDescs2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileDescs2FileInfoPairList.at(i).second)));
            }
            else
            {
                for(int i=dirNames2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirNames2FileInfoPairList.at(i).second)));
                for(int i=fileDescs2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileDescs2FileInfoPairList.at(i).second)));
            }
            if(indexOfDotDot>-1)
                listSorted.push_front(indexOfDotDot);
            break;
        case WFI_ATTRIBUTES:
            for(int i=0; i<list->size(); i++)
            {
                WinFileInfo* fileInfo=list->at(i);
                if(fileInfo->isDir())
                {
                    if(fileInfo->fileName()=="..")
                        indexOfDotDot=i;
                    else
                        dirAttrs2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(getAttributeString(*fileInfo), fileInfo));
                }
                else
                    fileAttrs2FileInfoPairList.push_back(QPair<QString, const WinFileInfo*>(getAttributeString(*fileInfo), fileInfo));
            }
            qSort(dirAttrs2FileInfoPairList.begin(), dirAttrs2FileInfoPairList.end());
            qSort(fileAttrs2FileInfoPairList.begin(), fileAttrs2FileInfoPairList.end());

            if(ascending)
            {
                for(int i=0; i<dirAttrs2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirAttrs2FileInfoPairList.at(i).second)));
                for(int i=0; i<fileAttrs2FileInfoPairList.size(); i++)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileAttrs2FileInfoPairList.at(i).second)));
            }
            else
            {
                for(int i=dirAttrs2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(dirAttrs2FileInfoPairList.at(i).second)));
                for(int i=fileAttrs2FileInfoPairList.size()-1; i>=0; i--)
                    listSorted.push_back(list->indexOf(const_cast<WinFileInfo*>(fileAttrs2FileInfoPairList.at(i).second)));
            }
            if(indexOfDotDot>-1)
                listSorted.push_front(indexOfDotDot);
            break;
    }

    // if a filterstring is active use it
    if(filter!="")
    {        
        WinFileInfoIndices listSortedAndFiltered;
        for(WinFileInfoIndices::iterator it=listSorted.begin(); it!=listSorted.end(); it++)
        {
            WinFileInfo* fileInfo=list->at(*it);
            if(fileInfo->fileName().contains(filter, Qt::CaseInsensitive))
                listSortedAndFiltered.push_back(*it);
        }
        return listSortedAndFiltered;
    }

    return listSorted;
}

QString WinFileInfo::getFreeSpaceOnDrive(QString path)
{
    QString drive=WinFileInfo::getDriveFromPath(path);
    DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;
    GetDiskFreeSpace (drive.toStdWString().c_str(), &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters);

    quint64 freeBytes=static_cast<quint64>(dwFreeClusters)*
                      static_cast<quint64>(dwBytesPerSect)*
                      static_cast<quint64>(dwSectPerClust);

    qreal sizeKB=static_cast<qreal>(freeBytes)/static_cast<qreal>(1024);
    qreal sizeMB=sizeKB/static_cast<qreal>(1024);
    qreal sizeGB=sizeMB/static_cast<qreal>(1024);
    qreal sizeTB=sizeGB/static_cast<qreal>(1024);
    QString freeSpaceOnDrive;
    if(sizeTB<1.0)
        if(sizeGB<1.0)
            if(sizeMB<1.0)
                freeSpaceOnDrive=QString("%1.2f KB").arg(sizeKB);
            else
                freeSpaceOnDrive=QString("%1.1f MB").arg(sizeMB);
         else
             freeSpaceOnDrive=QString("%1 GB").arg(sizeGB, 0, 'f', 2);
    else
        freeSpaceOnDrive=QString("%1 TB").arg(sizeTB, 0, 'f', 2);
    return freeSpaceOnDrive;
}

QString WinFileInfo::getFileTypeDescripton(const WinFileInfo& fi)
{
    if(fi.isDir())
        return QObject::tr("Folder");

    QString suffix=fi.suffix();
    if(suffix.length()==0)
        return QObject::tr("File");

    // Windows Platform: Infos zum Dateityp aus der Registry holen
    QString registrySuffixEntry="HKEY_CLASSES_ROOT\\."+suffix;
    QSettings settings(registrySuffixEntry, QSettings::NativeFormat);

    QString registryValue=settings.value("Default", "0").toString();
    QString registrySuffixDescription="HKEY_CLASSES_ROOT\\"+registryValue;
    QSettings settings2(registrySuffixDescription, QSettings::NativeFormat);
    QString fileTypeInfo=settings2.value("Default", "0").toString();
    if(fileTypeInfo!=QString("0")) // Zu diesem Dateityp gibt es Registry-Infos
        return fileTypeInfo;
    else // ansonsten eine selbstgebaute Beschreibung
        return suffix+"-"+QObject::tr("File");
}

QString WinFileInfo::getAttributeString(const WinFileInfo& fi)
{
    // Windows-Platform: Attribute besorgen
    /*
    FILE_ATTRIBUTE_READONLY                1      A file that is read-only. Applications can read the file, but cannot write to it or delete it.
                                                  This attribute is not honored on directories. For more information, see "You cannot view or change
                                                  the Read-only or the System attributes of folders in Windows Server 2003, in Windows XP, or in  Windows Vista".
    FILE_ATTRIBUTE_HIDDEN                  2      The file or directory is hidden. It is not included in an ordinary directory listing.
    FILE_ATTRIBUTE_SYSTEM                  4      A file or directory that the operating system uses a part of, or uses exclusively.
    FILE_ATTRIBUTE_DIRECTORY               16	  The handle that identifies a directory.
    FILE_ATTRIBUTE_ARCHIVE                 32 	  A file or directory that is an archive file or directory. Applications typically use this attribute
                                                  to mark files for backup or removal.
    FILE_ATTRIBUTE_DEVICE                  64	  This value is reserved for system use.
    FILE_ATTRIBUTE_NORMAL                  128	  A file that does not have other attributes set. This attribute is valid only when used alone.
    FILE_ATTRIBUTE_TEMPORARY	           256    A file that is being used for temporary storage. File systems avoid writing data back to mass storage
                                                  if sufficient cache memory is available, because typically, an application deletes a temporary file
                                                  after the handle is closed. In that scenario, the system can entirely avoid writing the data. Otherwise,
                                                  the data is written after the handle is closed.
    FILE_ATTRIBUTE_SPARSE_FILE	           512    A file that is a sparse file.
    FILE_ATTRIBUTE_REPARSE_POINT           1024   A file or directory that has an associated reparse point, or a file that is a symbolic link.
    FILE_ATTRIBUTE_COMPRESSED	           2048   A file or directory that is compressed. For a file, all of the data in the file is compressed. For
                                                  a directory, compression is the default for newly created files and subdirectories.
    FILE_ATTRIBUTE_OFFLINE	           4096   The data of a file is not available immediately. This attribute indicates that the file data is physically
                                                  moved to offline storage. This attribute is used by Remote Storage, which is the hierarchical storage
                                                  management software. Applications should not arbitrarily change this attribute.
    FILE_ATTRIBUTE_NOT_CONTENT_INDEXED	   8192   The file or directory is not to be indexed by the content indexing service.
    FILE_ATTRIBUTE_ENCRYPTED               16384  A file or directory that is encrypted. For a file, all data streams in the file are encrypted. For
                                                  a directory, encryption is the default for newly created files and subdirectories.
    FILE_ATTRIBUTE_VIRTUAL	           65536  This value is reserved for system use.
    */

    QString atributes;

    if(fi.attributes() & FILE_ATTRIBUTE_ARCHIVE)  atributes+="A";
    if(fi.attributes() & FILE_ATTRIBUTE_HIDDEN)   atributes+="H";
    if(fi.attributes() & FILE_ATTRIBUTE_READONLY) atributes+="R";
    if(fi.attributes() & FILE_ATTRIBUTE_SYSTEM)   atributes+="S";

    return atributes;
}

QDataStream& WinFileInfo::serialize(QDataStream& out) const
{
    out << fileInfo_.cAlternateFileName
        << fileInfo_.cFileName
        << (quint32)fileInfo_.dwFileAttributes
        << (quint32)fileInfo_.dwReserved0
        << (quint32)fileInfo_.dwReserved1
        << (quint32)fileInfo_.ftCreationTime.dwHighDateTime
        << (quint32)fileInfo_.ftCreationTime.dwLowDateTime
        << (quint32)fileInfo_.ftLastAccessTime.dwHighDateTime
        << (quint32)fileInfo_.ftLastAccessTime.dwLowDateTime
        << (quint32)fileInfo_.ftLastWriteTime.dwHighDateTime
        << (quint32)fileInfo_.ftLastWriteTime.dwLowDateTime
        << (quint32)fileInfo_.nFileSizeHigh
        << (quint32)fileInfo_.nFileSizeLow
        << path_.toStdString().c_str()
        << smbPath_.toStdString().c_str()
        << isSmbPath_
        << isSmbRoot_
        << isArchiveFile_
        << isArchiveDirectory_;
    return out;
}

QDataStream& WinFileInfo::deserialize(QDataStream& in)
{
    QString strAlternateFileName;
    QString strFileName;
    quint32 dwFileAttributes;
    quint32 dwReserved0;
    quint32 dwReserved1;
    quint32 dwHighDateTimeC;
    quint32 dwLowDateTimeC;
    quint32 dwHighDateTimeLA;
    quint32 dwLowDateTimeLA;
    quint32 dwHighDateTimeLW;
    quint32 dwLowDateTimeLW;
    quint32 nFileSizeHigh;
    quint32 nFileSizeLow;

    in >> strAlternateFileName
       >> strFileName
       >> dwFileAttributes
       >> dwReserved0 >> dwReserved1
       >> dwHighDateTimeC >> dwLowDateTimeC
       >> dwHighDateTimeLA >> dwLowDateTimeLA
       >> dwHighDateTimeLW >> dwLowDateTimeLW
       >> nFileSizeHigh >> nFileSizeLow
       >> path_
       >> smbPath_
       >> isSmbPath_
       >> isSmbRoot_
       >> isArchiveFile_
       >> isArchiveDirectory_;
/*typedef struct _WIN32_FIND_DATA {
  DWORD    dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  DWORD    nFileSizeHigh;
  DWORD    nFileSizeLow;
  DWORD    dwReserved0;
  DWORD    dwReserved1;
  TCHAR    cFileName[MAX_PATH];
  TCHAR    cAlternateFileName[14];
}WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;*/

    fileInfo_.dwFileAttributes=dwFileAttributes;
    fileInfo_.ftCreationTime.dwHighDateTime=dwHighDateTimeC;
    fileInfo_.ftCreationTime.dwLowDateTime=dwLowDateTimeC;
    fileInfo_.ftLastAccessTime.dwHighDateTime=dwHighDateTimeLA;
    fileInfo_.ftLastAccessTime.dwLowDateTime=dwLowDateTimeLA;
    fileInfo_.ftLastWriteTime.dwHighDateTime=dwHighDateTimeLW;
    fileInfo_.ftLastWriteTime.dwLowDateTime=dwLowDateTimeLW;
    fileInfo_.nFileSizeHigh=nFileSizeHigh;
    fileInfo_.nFileSizeLow=nFileSizeLow;
    fileInfo_.dwReserved0=dwReserved0;
    fileInfo_.dwReserved1=dwReserved1;
    wcscpy(&fileInfo_.cFileName[0], strFileName.toStdWString().c_str());
    wcscpy(&fileInfo_.cAlternateFileName[0], strAlternateFileName.toStdWString().c_str());

    return in;
}

QDataStream& operator<<(QDataStream &out, const WinFileInfo& instance)
{
    return instance.serialize(out);
}

QDataStream& operator>>(QDataStream &in, WinFileInfo& instance)
{
    return instance.deserialize(in);
}

QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage   = w * h * 4;

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    if (image.isNull())
        return image;

    // Get bitmap bits
    uchar *data = new uchar[bmi.bmiHeader.biSizeImage];

    if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {
        // Create image and copy data into image.
        for (int y=0; y<h; ++y) {
            void *dest = (void *) image.scanLine(y);
            void *src = data + y * image.bytesPerLine();
            memcpy(dest, src, image.bytesPerLine());
        }
    } else {
        qWarning("qt_fromWinHBITMAP(), failed to get bitmap bits");
    }
    delete [] data;

    return image;
}

QPixmap convertHIconToPixmap( const HICON icon)
{
    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    ICONINFO iconinfo;
    bool result = GetIconInfo(icon, &iconinfo); //x and y Hotspot describes the icon center
    if (!result)
        qWarning("convertHIconToPixmap(), failed to GetIconInfo()");

    int w = iconinfo.xHotspot * 2;
    int h = iconinfo.yHotspot * 2;

    BITMAPINFOHEADER bitmapInfo;
    bitmapInfo.biSize        = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biWidth       = w;
    bitmapInfo.biHeight      = h;
    bitmapInfo.biPlanes      = 1;
    bitmapInfo.biBitCount    = 32;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biSizeImage   = 0;
    bitmapInfo.biXPelsPerMeter = 0;
    bitmapInfo.biYPelsPerMeter = 0;
    bitmapInfo.biClrUsed       = 0;
    bitmapInfo.biClrImportant  = 0;
    DWORD* bits;

    HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
    DrawIconEx( hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);
    QImage image = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

    for (int y = 0 ; y < h && !foundAlpha ; y++) {
        QRgb *scanLine= reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < w ; x++) {
            if (qAlpha(scanLine[x]) != 0) {
                foundAlpha = true;
                break;
            }
        }
    }
    if (!foundAlpha) {
        //If no alpha was found, we use the mask to set alpha values
        DrawIconEx( hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
        QImage mask = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0 ; y < h ; y++){
            QRgb *scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
            QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb *>(mask.scanLine(y));
            for (int x = 0; x < w ; x++){
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0; //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000; // set the alpha channel to 255
            }
        }
    }
    //dispose resources created by iconinfo call
    DeleteObject(iconinfo.hbmMask);
    DeleteObject(iconinfo.hbmColor);

    SelectObject(hdc, oldhdc); //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(image);
}


// needed for MSVC 2010 builds...
#define IO_REPARSE_TAG_RESERVED_ZERO 0
#define IO_REPARSE_TAG_SYMBOLIC_LINK IO_REPARSE_TAG_RESERVED_ZERO

typedef struct _REPARSE_DATA_BUFFER {
  ULONG  ReparseTag;
  USHORT  ReparseDataLength;
  USHORT  Reserved;
  union {
    struct {
      USHORT  SubstituteNameOffset;
      USHORT  SubstituteNameLength;
      USHORT  PrintNameOffset;
      USHORT  PrintNameLength;
      WCHAR  PathBuffer[1];
      } SymbolicLinkReparseBuffer;
    struct {
      USHORT  SubstituteNameOffset;
      USHORT  SubstituteNameLength;
      USHORT  PrintNameOffset;
      USHORT  PrintNameLength;
      WCHAR  PathBuffer[1];
      } MountPointReparseBuffer;
    struct {
      UCHAR  DataBuffer[1];
    } GenericReparseBuffer;
  };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
// END needed for MSVC 2010 builds...

void getLinkTarget(QString fileName, DWORD fileAttributes, QString& target, int& targetType)
{
    HANDLE   fileHandle;

    BYTE	 reparseBuffer[17000];
    PBYTE	 reparseData;
    PREPARSE_GUID_DATA_BUFFER reparseInfo = (PREPARSE_GUID_DATA_BUFFER) reparseBuffer;
    PREPARSE_DATA_BUFFER msReparseInfo = (PREPARSE_DATA_BUFFER) reparseBuffer;
    DWORD	returnedLength;
    TCHAR	printName[1024];  // print name
    TCHAR	substName[1024];  // substitute name
    int         iTargetType=TARGET_INVALID;

    if(fileAttributes & FILE_ATTRIBUTE_DIRECTORY )
    {
        fileHandle = CreateFile( fileName.toStdWString().c_str(), 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OPEN_REPARSE_POINT , 0 );

    }
    else
    {
        fileHandle = CreateFile( fileName.toStdWString().c_str(), 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_OPEN_REPARSE_POINT, 0 );
    }

    if( fileHandle != INVALID_HANDLE_VALUE)
    {
        if( fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT )
        {
            if( DeviceIoControl( fileHandle, FSCTL_GET_REPARSE_POINT,
                                 NULL, 0, reparseInfo, sizeof( reparseBuffer ),
                                 &returnedLength, NULL ))
            {
                if( IsReparseTagMicrosoft( reparseInfo->ReparseTag ))
                {
                    switch( reparseInfo->ReparseTag )
                    {
                    case 0x80000000|IO_REPARSE_TAG_SYMBOLIC_LINK: // symbolic links
                        reparseData=(PBYTE) &msReparseInfo->SymbolicLinkReparseBuffer.PathBuffer;
                        wcsncpy ( printName,
                                  (PWCHAR) (reparseData + msReparseInfo->SymbolicLinkReparseBuffer.PrintNameOffset),
                                  msReparseInfo->SymbolicLinkReparseBuffer.PrintNameLength );
                        printName[msReparseInfo->SymbolicLinkReparseBuffer.PrintNameLength] = 0;
                        wcsncpy( substName,
                                  (PWCHAR) (reparseData + msReparseInfo->SymbolicLinkReparseBuffer.SubstituteNameOffset),
                                  msReparseInfo->SymbolicLinkReparseBuffer.SubstituteNameLength );
                        substName[msReparseInfo->SymbolicLinkReparseBuffer.SubstituteNameLength] = 0;
                        iTargetType=TARGET_SYMLINK;
                        break;
                    case IO_REPARSE_TAG_MOUNT_POINT: // mount points and junctions
                        reparseData = (PBYTE) &msReparseInfo->MountPointReparseBuffer.PathBuffer;
                        wcsncpy( printName,
                                  (PWCHAR) (reparseData + msReparseInfo->MountPointReparseBuffer.PrintNameOffset),
                                  msReparseInfo->MountPointReparseBuffer.PrintNameLength );
                        printName[msReparseInfo->MountPointReparseBuffer.PrintNameLength] = 0;
                        wcsncpy( substName,
                                  (PWCHAR) (reparseData + msReparseInfo->MountPointReparseBuffer.SubstituteNameOffset),
                                  msReparseInfo->MountPointReparseBuffer.SubstituteNameLength );
                        substName[msReparseInfo->MountPointReparseBuffer.SubstituteNameLength] = 0;
                        break;
                    }
                }
            }
        }
        CloseHandle( fileHandle );
    }

    target=QString::fromUtf16((ushort*)substName);
    if(target.contains("\\??\\"))
    {
        if(target.contains(':')) // junction
            targetType=TARGET_JUNCTION;
        else                        // mount point
            targetType=TARGET_MOUNTPOINT;
    }
}
