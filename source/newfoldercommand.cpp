/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Command for creation of a new folder.
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
#include "newfoldercommand.h"
#include <QInputDialog>
#include <QDir>
#include <QLineEdit>
#include <QObject>
#include <QString>
#include <QMessageBox>
#include "mainwindow.h"
#include "FileExplorer.h"

NewFolderCommand::NewFolderCommand()
{
}

void NewFolderCommand::execute()
{
   MainWindow* mainWindow=const_cast<MainWindow*>(MainWindow::getMainWindow());
   bool ok;
   QString folderName = QInputDialog::getText(mainWindow,
                                              QObject::tr("Create new folder"),
                                              QObject::tr("Folder:"),
                                              QLineEdit::Normal,
                                              "",
                                              &ok);
   if(ok && !folderName.isEmpty())
   {
       FileExplorer* explorer=const_cast<FileExplorer*>(mainWindow->getActiveExplorer());
       QString newFolder=explorer->getSelectedPath()+"\\"+folderName;
       QDir dir;
       if(dir.mkdir(newFolder))
           explorer->refresh();
       else
           QMessageBox::information(mainWindow,
                                    QObject::tr("Error"),
                                    QObject::tr("Couldn't create folder %1!").arg(newFolder));
   }
}

NewFolderCommand* NewFolderCommand::getNewFolderCommand()
{
    return new NewFolderCommand();
}
