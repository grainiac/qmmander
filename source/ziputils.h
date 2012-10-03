/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Helper functions for zip archive access through the Zip Utils.
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
#ifndef ZIPUTILS_H
#define ZIPUTILS_H

#include <QtCore>
#include "ZipUtils/modify.h"

namespace ZipUtils
{

void   qDebugZipErrors(ZRESULT returnCode);
bool   fileExists(QString archivePathName, QString archiveFilePath, QString archivePassword);
qint64 fileSize(QString archivePathName, QString archiveFilePath, QString archivePassword, bool bCompressedSize);
time_t fileLastModificationTime(QString archivePathName, QString archiveFilePath, QString archivePassword);
void   extractFile(QString archivePathName, QString archiveFilePath, QString archivePassword, QString destFilePath);

}

#endif // ZIPUTILS_H
