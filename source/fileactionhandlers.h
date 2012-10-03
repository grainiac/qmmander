/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
****************************************************************************/
#ifndef FILEACTIONHANDLERS_H
#define FILEACTIONHANDLERS_H

#include <QtCore>
#include "fileactionfactories.h"
#include "archive.h"

class FileActionHandler
{
public:
    FileActionHandler(const FileActionFactory* fileActionFactory);
    virtual ~FileActionHandler() {};

    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const = 0;

    const FileActionFactory* getFileActionFactory() const;

private:
    const FileActionFactory* m_fileActionFactory;
};

class UnzipFileActionHandler : public FileActionHandler
{
public:
    UnzipFileActionHandler(const FileActionFactory* fileActionFactory);
    virtual ~UnzipFileActionHandler() {};

    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const = 0;

    QString getAutoRenamedFileName(QString filePath) const;
};

class ZipFileActionHandler : public FileActionHandler
{
public:
    ZipFileActionHandler(const FileActionFactory* fileActionFactory);
    virtual ~ZipFileActionHandler();

    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const = 0;    
};


//
// Concrete UnzipFileActionHanders

class UFAHDefault : public UnzipFileActionHandler
{
public:
    UFAHDefault(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHDefault() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHRenameDestinationManual : public UnzipFileActionHandler
{
public:
    UFAHRenameDestinationManual(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHRenameDestinationManual() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHRenameSourceManual : public UnzipFileActionHandler
{
public:
    UFAHRenameSourceManual(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHRenameSourceManual() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHRenameDestinationAuto : public UnzipFileActionHandler
{
public:
    UFAHRenameDestinationAuto(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHRenameDestinationAuto() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHRenameSourceAuto : public UnzipFileActionHandler
{
public:
    UFAHRenameSourceAuto(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHRenameSourceAuto() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHOverWriteOlderAndSameAge : public UnzipFileActionHandler
{
public:
    UFAHOverWriteOlderAndSameAge(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHOverWriteOlderAndSameAge() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHCopyLargerOverWriteSmaller : public UnzipFileActionHandler
{
public:
    UFAHCopyLargerOverWriteSmaller(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHCopyLargerOverWriteSmaller() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class UFAHCopySmallerOverWriteLarger : public UnzipFileActionHandler
{
public:
    UFAHCopySmallerOverWriteLarger(const FileActionFactory* fileActionFactory) : UnzipFileActionHandler(fileActionFactory) {};
    virtual ~UFAHCopySmallerOverWriteLarger() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};


//
// Concrete ZipFileActionHanders

class ZFAHDefault : public ZipFileActionHandler
{
public:
    ZFAHDefault(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHDefault() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHRenameDestinationManual : public ZipFileActionHandler
{
public:
    ZFAHRenameDestinationManual(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHRenameDestinationManual() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHRenameSourceManual : public ZipFileActionHandler
{
public:
    ZFAHRenameSourceManual(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHRenameSourceManual() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHRenameDestinationAuto : public ZipFileActionHandler
{
public:
    ZFAHRenameDestinationAuto(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHRenameDestinationAuto() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHRenameSourceAuto : public ZipFileActionHandler
{
public:
    ZFAHRenameSourceAuto(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHRenameSourceAuto() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHOverWriteOlderAndSameAge : public ZipFileActionHandler
{
public:
    ZFAHOverWriteOlderAndSameAge(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHOverWriteOlderAndSameAge() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHCopyLargerOverWriteSmaller : public ZipFileActionHandler
{
public:
    ZFAHCopyLargerOverWriteSmaller(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHCopyLargerOverWriteSmaller() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

class ZFAHCopySmallerOverWriteLarger : public ZipFileActionHandler
{
public:
    ZFAHCopySmallerOverWriteLarger(const FileActionFactory* fileActionFactory) : ZipFileActionHandler(fileActionFactory) {};
    virtual ~ZFAHCopySmallerOverWriteLarger() {};
    virtual void doFileAction(QString sourceFilePath, QString destinationfilePath) const;
};

#endif // FILEACTIONHANDLERS_H
