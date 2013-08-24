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
#ifndef ZIPUNZIPWATCHERTHREAD_H
#define ZIPUNZIPWATCHERTHREAD_H

#include <QThread>

class ArchiveInfo;
//class UnzipThread;
//class ZipThread;
class ZipUnzipProgressDialog;

class ZipUnzipWatcherThread : public QThread
{
Q_OBJECT
public:    
    ZipUnzipWatcherThread(ZipUnzipProgressDialog* pParent, ArchiveInfo* pAI);

signals:
    void triggerUpdate();
    void ask4FileAction();

protected:
    void run();

private:
    void sendUpdateTrigger2ProgressDialog();
    void askProgressDialog4FileActionDialog();    
    void emitAsk4FileAction();
    void updateTimesInProgressDialogAfterOneSecond();
    void updateTimes();

    ArchiveInfo* m_pAI;
    QThread* m_pThread;
    int m_iLoopCount;
    int m_iElapsedSeconds;
};

#endif // ZIPUNZIPWATCHERTHREAD_H
