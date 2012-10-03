/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Helper functions for Win32 SHFileOperation.
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
#ifndef SHFILEOPERATIONS_H
#define SHFILEOPERATIONS_H

#include "winfileinfo.h"

TCHAR* getSHFileOperationFromString(WinFileInfoList list);
TCHAR* getSHFileOperationFromString(QStringList files);
TCHAR* getSHFileOperationCopyToString(QString strFilePath);
TCHAR* getSHFileOperationMoveToString(WinFileInfoList listOfFiles2Move, QString strDestPath);
TCHAR* getSHFileOperationMoveToString(QStringList listOfFiles2Move, QString strDestPath);

#endif // SHFILEOPERATIONS_H
