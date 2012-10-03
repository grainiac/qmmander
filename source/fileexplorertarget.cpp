/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** File model for the FileExplorerView.
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
#include "fileexplorertarget.h"
#include "archive.h"

FileExplorerTarget::FileExplorerTarget()
:   files_(new WinFileInfoPtrList())
{
}

FileExplorerTarget::FileExplorerTarget(const FileExplorerTarget& source)
:   files_(new WinFileInfoPtrList())
{
    for(int i=0; i<source.files_->count(); ++i)
    {
        WinFileInfo* fi=new WinFileInfo();
        *fi=*source.files_->at(i);
        files_->push_back(fi);
    }
}

FileExplorerTarget& FileExplorerTarget::operator=(const FileExplorerTarget& rhs)
{
   if(this!=&rhs)
   {
       for(int i=0; i<rhs.files_->count(); ++i)
       {
           WinFileInfo* fi=new WinFileInfo();
           *fi=*rhs.files_->at(i);
           files_->push_back(fi);
       }
   }
   return *this;
}

FileExplorerTarget::~FileExplorerTarget()
{
  delete files_;
}

bool FileExplorerTarget::changeDir(QString path)
{
    if(WinFileInfo::isSmbPath(path) && path.endsWith(".."))
       path=path.left(path.lastIndexOf('\\'));
    QFile filePath(path);
    if(!filePath.exists())
        return false;

    path_=path;
    extractArchiveInformation();

    if(files_)
        delete files_;
    if(archiveName_.length()>0)
        files_=Archive::listFiles(path_, archiveName_, pathInArchive_);
    else
        files_=WinFileInfo::listFiles(path_);

    buildCanonicalPath();

    return true;
}

void FileExplorerTarget::extractArchiveInformation()
{
    int position=path_.indexOf("<Archive>");
    if(position!=-1)
    {
        archiveName_=path_.right(path_.length()-(position+9));
        path_=path_.left(position);
        position=archiveName_.indexOf("\\");
        if(position!=-1)
        {
            pathInArchive_=archiveName_.right(archiveName_.length()-(position+1));
            archiveName_=archiveName_.left(position);
        }
    }
}

void FileExplorerTarget::sortFiles(WinFileInfo::SortOptions sortBy, QString filter, bool sortAscending)
{
    if(files_)
        sortedIndices_=WinFileInfo::sortFileInfoList(files_, sortBy, filter, sortAscending);
}

void FileExplorerTarget::buildCanonicalPath()
{
    QDir binDir(path_);
    canonicalPath_ = binDir.canonicalPath();

    if(archiveName_.length()>0)
    {
        canonicalPath_+="\\"+archiveName_;
        if(pathInArchive_>0)
            canonicalPath_+="\\"+pathInArchive_;
    }
    canonicalPath_=canonicalPath_.replace("/", "\\");
}

QString FileExplorerTarget::getCanonicalPath() const
{
    return canonicalPath_;
}

const WinFileInfoPtrList& FileExplorerTarget::getFiles() const
{
    return *files_;
}

const WinFileInfoIndices& FileExplorerTarget::getSortedFileIndices() const
{
    return sortedIndices_;
}
