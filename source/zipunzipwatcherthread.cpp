/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This thread periodically updates the progress dialog of a zip/unzip operation.
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
#include "zipunzipwatcherthread.h"
#include "archiveinfo.h"
#include "unzipthread.h"
#include "zipthread.h"
#include "zipunzipprogressdialog.h"

ZipUnzipWatcherThread::ZipUnzipWatcherThread(ZipUnzipProgressDialog* pParent, ArchiveInfo* pAI)
:   QThread(0),
    m_pAI(pAI),
    m_pThread(pParent->workerThreadHandle()),
    m_iLoopCount(0),
    m_iElapsedSeconds(0)
{    
    connect(this, SIGNAL(triggerUpdate()), pParent, SLOT(update()));
    connect(this, SIGNAL(ask4FileAction()), pParent, SLOT(ask4FileAction()));
}

void ZipUnzipWatcherThread::run()
{
    forever
    {
        sendUpdateTrigger2ProgressDialog();
        askProgressDialog4FileActionDialog();
        updateTimesInProgressDialogAfterOneSecond();
    }
}

void ZipUnzipWatcherThread::sendUpdateTrigger2ProgressDialog()
{
    emit(triggerUpdate());
}

void ZipUnzipWatcherThread::askProgressDialog4FileActionDialog()
{    
    if(m_pAI->m_trueZipModeFalseUnzipMode)
    {
        ZipThread* pThread=dynamic_cast<ZipThread*>(m_pThread);
        if(pThread && !m_pAI->m_isUserInputRequested && pThread->waitsForFileAction())
            emitAsk4FileAction();
    }
    else
    {
        UnzipThread* pThread=dynamic_cast<UnzipThread*>(m_pThread);
        if(pThread && !m_pAI->m_isUserInputRequested && pThread->waitsForFileAction())
            emitAsk4FileAction();
    }
}

void ZipUnzipWatcherThread::emitAsk4FileAction()
{
    m_pAI->m_isUserInputRequested=true;
    emit(ask4FileAction());
}

void ZipUnzipWatcherThread::updateTimesInProgressDialogAfterOneSecond()
{
    m_iLoopCount++;
    if(m_iLoopCount==20)
    {
        m_iElapsedSeconds++;
        updateTimes();
        m_iLoopCount=0;
    }
    msleep(50);
}

void ZipUnzipWatcherThread::updateTimes()
{
    int iH=m_iElapsedSeconds/3600;
    int iM=(m_iElapsedSeconds%3600)/60;
    int iS=((m_iElapsedSeconds%3600)%60000);
    m_pAI->m_timeElapsed=QString("%1:%2:%3").arg(iH,2,10,QChar('0')).arg(iM,2,10,QChar('0')).arg(iS,2,10,QChar('0'));

    int iRemainingSeconds=static_cast<int>(static_cast<qreal>(m_iElapsedSeconds)/
                                           static_cast<qreal>((m_pAI->m_bytesProcessed)/1024.)*(static_cast<qreal>(m_pAI->m_totalSize)/1024.)+.5)
                          -m_iElapsedSeconds;
    iH=iRemainingSeconds/3600;
    iM=(iRemainingSeconds%3600)/60;
    iS=((iRemainingSeconds%3600)%60000);
    m_pAI->m_timeRemaining=QString("%1:%2:%3").arg(iH,2,10,QChar('0')).arg(iM,2,10,QChar('0')).arg(iS,2,10,QChar('0'));

    int iKiBPerSecond=m_pAI->m_bytesProcessed-m_pAI->m_bytesProcessedOld;
    iKiBPerSecond/=1024;

    m_pAI->m_speed=QString("%1 KiB/s").arg(iKiBPerSecond);
    m_pAI->m_bytesProcessedOld=m_pAI->m_bytesProcessed;
}
