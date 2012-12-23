/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for file deletion to recycle bin.
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
#include "deletetorecyclebincommand.h"

#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"

DeleteToRecycleBinCommand::DeleteToRecycleBinCommand()
:   files2Delete_(0)
{
}

DeleteToRecycleBinCommand::~DeleteToRecycleBinCommand()
{
    delete [] files2Delete_;
}

void DeleteToRecycleBinCommand::execute()
{
    SHFILEOPSTRUCTW st;
    st.hwnd=(HWND)MainWindow::getMainWindow()->winId();
    st.wFunc=FO_DELETE;
    st.fFlags=FOF_ALLOWUNDO | ~FOF_NOCONFIRMATION;
    st.pFrom=files2Delete_;
    st.pTo=0;
    ::SHFileOperationW(&st);
}

DeleteToRecycleBinCommand* DeleteToRecycleBinCommand::deleteCommandForActualSelection()
{
    DeleteToRecycleBinCommand* dc=new DeleteToRecycleBinCommand();
    dc->files2Delete_ = getSHFileOperationFromString(const_cast<FileExplorer*>(MainWindow::getMainWindow()->getActiveExplorer())->getSelectedFiles());
    return dc;
}
