/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for file renaming.
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
#include "renamecommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "shfileoperations.h"
#include "renamedialog.h"
#include <QMessageBox>
#include <QObject>

void RenameCommand::execute()
{
    MainWindow* parentWidget=const_cast<MainWindow*>(MainWindow::getMainWindow());
    WinFileInfoList selectedFiles=parentWidget->getActiveExplorer()->getSelectedFiles();
    if(selectedFiles.count()==1)
    {
        WinFileInfo fi=selectedFiles.at(0);
        RenameDialog dlg(fi, parentWidget);
        if(dlg.exec()==QDialog::Accepted)
        {
            QString newFileName=dlg.getFileName();
            if(newFileName!=fi.fileName())
            {
                if(fi.isFile())
                {
                    QFile file(fi.filePath());
                    if(!file.rename(fi.path()+"\\"+newFileName))
                        QMessageBox::information(parentWidget, QObject::tr("Error"), QObject::tr("Couldn't rename file %1!").arg(fi.fileName()));
                    else
                        parentWidget->getActiveExplorer()->refresh();
                }
                else if(fi.isDir())
                {
                    QDir dir(fi.filePath());
                    if(!dir.rename(fi.filePath(), fi.path()+"\\"+newFileName))
                        QMessageBox::information(parentWidget, QObject::tr("Error"), QObject::tr("Couldn't rename directory %1!").arg(fi.fileName()));
                    else
                        parentWidget->getActiveExplorer()->refresh();
                }
            }
        }
    }
    else
        QMessageBox::information(parentWidget, QObject::tr("Rename"), "<HTML><p>"+QObject::tr("Please select <b>one</b> file!")+"</p></HTML>");

}

RenameCommand* RenameCommand::renameCommandForActualSelection()
{
    return new RenameCommand();
}
