/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Base thread class for archiving operations that require iteration over
** a set of items and user interaction if an item already exists at the
** target destination. Look at the derived class for the interaction process.
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
#ifndef ARCHIVERTHREAD_H
#define ARCHIVERTHREAD_H

#include <QThread>
#include "fileactionfactories.h"
#include "unzipdialog.h"
#include "winfileinfo.h"

class ArchiveInfo;
class ZipUnzipProgressDialog;

class ArchiverThread : public QThread
{
    Q_OBJECT
public:
    ArchiverThread(ZipUnzipProgressDialog  const * const parent,
                   ArchiveInfo* const  archiveInfo);
    virtual ~ArchiverThread();

    virtual void performFileAction(QString archiveFilePath) const = 0;
    void checkWaitConditions();

    void endThread();
    void initializeTermination();
    bool isTerminationSet() const;
    void terminateThread();
    bool waitsForFileAction() const;
    bool waitsForRenameAction() const;

public slots:
    void pause(bool pauseExtraction);

signals:
    void finishedExecution();

private:            
    void waitIfPaused() const;
    void waitIfTerminationWasRequested() const;
    void wait4FileAction();

    ArchiveInfo* const m_archiveInfo;
    bool m_sleepUntilFileAction;
    bool m_threadExecutionPaused;
    bool m_threadTerminationInProgress;
};

#endif // ARCHIVERTHREAD_H
