/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for fily copying.
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
#include "copycommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"

CopyCommand::CopyCommand()
:   sourceFiles_(0),
    destinationDirectory_(0)
{
}

CopyCommand::~CopyCommand()
{
    delete [] sourceFiles_;
    delete [] destinationDirectory_;
}

void CopyCommand::execute()
{
    SHFILEOPSTRUCTW st;
    st.hwnd   = MainWindow::getMainWindow()->winId();
    st.wFunc  = FO_COPY;
    st.fFlags = 0x00000000;
    st.pFrom  = sourceFiles_;
    st.pTo    = destinationDirectory_;
    ::SHFileOperationW(&st);
}

CopyCommand* CopyCommand::copyCommandForActualSelection()
{
    CopyCommand* cc=new CopyCommand();
    cc->sourceFiles_ = getSHFileOperationFromString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getActiveExplorer())->getSelectedFiles());
    cc->destinationDirectory_ = getSHFileOperationCopyToString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getInactiveExplorer())->getSelectedPath());
    return cc;
}

