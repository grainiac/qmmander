/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** This thread performs the adjustment of file attributes and dates.
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
#ifndef ATTRIBUTESANDDATECHANGERTHREAD_H
#define ATTRIBUTESANDDATECHANGERTHREAD_H

#include <QThread>
#include <QProgressDialog>
#include "winfileinfo.h"
#include "attributesdatedialog.h"

class AttributesAndDateChangerThread : public QThread
{
Q_OBJECT
public:
    explicit AttributesAndDateChangerThread(const WinFileInfoList& files,
                                            const FileProperties& properties,
                                            const QProgressDialog& dlg,
                                            QObject *parent = 0);
    ~AttributesAndDateChangerThread();

    const QProgressDialog& getProgressDialog() const;

public slots:
    void cancelOperation();

signals:
    void processMessage(QString message) const;
    void fileRangeMessage(int min, int max) const;
    void operationFinished(AttributesAndDateChangerThread* pThat) const;

protected:
    void run();    

private:
    void countFiles(QString path) const;
    void processSubDirectory(const WinFileInfo& fileInfo) const;
    void processFileOrDirectory(const WinFileInfo& fileInfo) const;
    void changeAttributes(const WinFileInfo& fileInfo) const;
    void changeTimes(const WinFileInfo& fileInfo) const;
    bool timesNeedChange() const;
    bool isProcessableDir(const WinFileInfo& fileInfo) const;

    void sendProcessMessage(const WinFileInfo& fileInfo) const;

    WinFileInfoPtrList* files_;
    FileProperties properties_;
    const QProgressDialog& progressDialog_;

    mutable int dirCounter_;
    mutable int fileCounter_;
    mutable int sumOfFilesAndDirectories_;
    mutable int actualElement_;
    mutable QTime processTimer_;
};

#endif // ATTRIBUTESANDDATECHANGERTHREAD_H
