/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for file moving.
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
#include "movecommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"

MoveCommand::MoveCommand()
:   sourceFiles_(0),
    destinationFiles_(0)
{
}

MoveCommand::~MoveCommand()
{
    delete [] sourceFiles_;
    delete [] destinationFiles_;
}

void MoveCommand::execute()
{
    SHFILEOPSTRUCTW st;
    st.hwnd   = MainWindow::getMainWindow()->winId();
    st.wFunc  = FO_MOVE;
    st.fFlags = FOF_MULTIDESTFILES | FOF_ALLOWUNDO;
    st.pFrom  = sourceFiles_;
    st.pTo    = destinationFiles_;
    ::SHFileOperationW(&st);
}

MoveCommand* MoveCommand::moveCommandForActualSelection()
{
    MoveCommand* mc=new MoveCommand();
    mc->sourceFiles_ = getSHFileOperationFromString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getActiveExplorer())->getSelectedFiles());
    mc->destinationFiles_ = getSHFileOperationMoveToString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getActiveExplorer())->getSelectedFiles(),
                                                           const_cast<FileExplorer*>(MainWindow::getMainWindow()->getInactiveExplorer())->getSelectedPath());
    return mc;
}

