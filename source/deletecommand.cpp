/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for file deletion.
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
#include "deletecommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"

DeleteCommand::DeleteCommand()
:   files2Delete_(0)
{
}

DeleteCommand::~DeleteCommand()
{
    delete [] files2Delete_;
}

void DeleteCommand::execute()
{
    SHFILEOPSTRUCTW st;
    st.hwnd=MainWindow::getMainWindow()->winId();
    st.wFunc=FO_DELETE;
    st.fFlags=0x4000; // FOF_WANTNUKEWARNING for OS >= Vista
    st.pFrom=files2Delete_;
    st.pTo=0;
    ::SHFileOperationW(&st);
}

DeleteCommand* DeleteCommand::deleteCommandForActualSelection()
{
    DeleteCommand* dc=new DeleteCommand();
    dc->files2Delete_ = getSHFileOperationFromString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getActiveExplorer())->getSelectedFiles());
    return dc;
}

