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
#include "archiverthread.h"
#include "archiveinfo.h"
#include "zipunzipprogressdialog.h"

ArchiverThread::ArchiverThread(ZipUnzipProgressDialog  const * const parent,
                               ArchiveInfo* const  archiveInfo)
:   QThread(0),
    m_archiveInfo(archiveInfo),
    m_sleepUntilFileAction(false),
    m_threadExecutionPaused(false),
    m_threadTerminationInProgress(false)
{
    connect(this, SIGNAL(finished()), parent, SLOT(workerThreadFinished()));
}

ArchiverThread::~ArchiverThread()
{
}

void ArchiverThread::pause(bool pauseExtraction)
{
    m_threadExecutionPaused=pauseExtraction;
}

void ArchiverThread::endThread()
{
    exit();
}

void ArchiverThread::initializeTermination()
{
    m_threadTerminationInProgress=true;
}

bool ArchiverThread::isTerminationSet() const
{
    return m_threadTerminationInProgress;
}

void ArchiverThread::terminateThread()
{
    this->terminate();
    emit(finished());
}

void ArchiverThread::checkWaitConditions()
{
    waitIfPaused();
    waitIfTerminationWasRequested();
    if( m_archiveInfo->askUser4FileAction() )
        wait4FileAction();
}

void ArchiverThread::waitIfPaused() const
{
    while(m_threadExecutionPaused)
        msleep(50);
}

void ArchiverThread::waitIfTerminationWasRequested() const
{
    while(m_threadTerminationInProgress)
        sleep(50);
}

void ArchiverThread::wait4FileAction()
{
    m_sleepUntilFileAction=true;

    while(m_archiveInfo->m_fileAction==FA_NoAction)
        msleep(20);

    m_sleepUntilFileAction=false;
}

bool ArchiverThread::waitsForFileAction() const
{
    return m_sleepUntilFileAction;
}
