/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Command for (un)zipping files.
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
#ifndef ARCHIVECOMMAND_H
#define ARCHIVECOMMAND_H

#include "command.h"
#include <QObject>

class ArchiveInfo;
class MainWindow;
class ZipUnzipProgressDialog;

class ArchiveCommand : public QObject, public Command
{
   Q_OBJECT
public:    
    virtual void execute();

    static ArchiveCommand* archiveCommandForCompression();
    static ArchiveCommand* archiveCommandForDecompression();

signals:
    void archiveCommandFinished(ArchiveCommand* deleteMe);

protected:
    ArchiveCommand(bool mode);

private slots:
    void ZipUnzipOperationFinished();

private:
    void zipMode();
    void unzipMode();
    ArchiveInfo* getZipUnzipFilesAndDestination(bool trueZipModeFalseUnzipMode);
    void doArchiveOperation();

    bool mode_;
    ArchiveInfo* archiveInfo_;
    ZipUnzipProgressDialog* progressDlg_;
    MainWindow* mainWindow_;
};

#endif // ARCHIVECOMMAND_H
