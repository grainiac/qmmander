/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Command for calling helper apps.
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
#include "callhelperappcommand.h"
#include "FileExplorer.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QObject>

CallHelperAppCommand::CallHelperAppCommand(CommandType type)
:   type_(type)
{
}

void CallHelperAppCommand::execute()
{
    switch(type_)
    {
    case WINDOWS_CALCULATOR_COMMAND:
        executeWindowsCalculator();
        break;
    case DOS_BOX_COMMAND:
        executeDosBox();
        break;
    case NETWORK_DRIVE_CONNECT_DIALOG:
        executeNetworkDriveConnectDialog();
        break;
    case NETWORK_DRIVE_DISCONNECT_DIALOG:
        executeNetworkDriveDisconnectDialog();
        break;
    case FILE_INFORMATION_DIALOG:
        executeFileInformationDialog();
        break;
    }
}

CallHelperAppCommand * CallHelperAppCommand::getWindowsCalculatorCommand()
{
    return new CallHelperAppCommand(WINDOWS_CALCULATOR_COMMAND);
}

CallHelperAppCommand * CallHelperAppCommand::getDosBoxCommand()
{
    return new CallHelperAppCommand(DOS_BOX_COMMAND);
}

CallHelperAppCommand * CallHelperAppCommand::getNetworkDriveConnectDialogCommand()
{
    return new CallHelperAppCommand(NETWORK_DRIVE_CONNECT_DIALOG);
}

CallHelperAppCommand * CallHelperAppCommand::getNetworkDriveDisconnectDialogCommand()
{
    return new CallHelperAppCommand(NETWORK_DRIVE_DISCONNECT_DIALOG);
}

CallHelperAppCommand* CallHelperAppCommand::getFileInformationDialog()
{
    return new CallHelperAppCommand(FILE_INFORMATION_DIALOG);
}

void CallHelperAppCommand::executeWindowsCalculator()
{
    ShellExecute((HWND)getMainWindow()->winId(), TEXT("open"), TEXT("calc.exe"), TEXT(""), TEXT(""), SW_SHOW);
}

void CallHelperAppCommand::executeDosBox()
{
    ShellExecute(0,TEXT("open"), TEXT("cmd"), TEXT("/C \"start \"Qmmander DOS-Box\"\""),
                 const_cast<FileExplorer*>(getMainWindow()->getActiveExplorer())->getSelectedPath().toStdWString().c_str(),
                 SW_SHOW);
}

void CallHelperAppCommand::executeNetworkDriveConnectDialog()
{
    WNetConnectionDialog((HWND)getMainWindow()->winId(), RESOURCETYPE_DISK);
}

void CallHelperAppCommand::executeNetworkDriveDisconnectDialog()
{
    WNetDisconnectDialog((HWND)getMainWindow()->winId(), RESOURCETYPE_DISK);
}

void CallHelperAppCommand::executeFileInformationDialog()
{
    WinFileInfoList selectedFiles=const_cast<FileExplorer*>(getMainWindow()->getActiveExplorer())->getSelectedFiles();
    if(selectedFiles.count()==1)
    {
        WinFileInfo fi=selectedFiles.at(0);
        SHELLEXECUTEINFO sei;
        sei.cbSize = sizeof(SHELLEXECUTEINFO);
        sei.fMask = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOCLOSEPROCESS;
        sei.hwnd = 0;
        sei.lpVerb = QString("properties").toStdWString().c_str();
        sei.lpFile = fi.filePath().toStdWString().c_str();
        sei.lpParameters= 0;
        sei.nShow = SW_SHOWNORMAL;
        sei.hInstApp = 0;
        sei.lpIDList = 0;
        sei.lpClass = 0;
        sei.hkeyClass = 0;
        sei.dwHotKey = 0;
        sei.hIcon = 0;
        sei.hProcess = 0;
        sei.lpDirectory = 0;
        ShellExecuteEx(&sei);
    }
    else
        QMessageBox::information(getMainWindow(), QObject::tr("Properties"), "<HTML><p>"+QObject::tr("Please select <b>one</b> file!")+"</p></HTML>");
}

MainWindow* CallHelperAppCommand::getMainWindow()
{
    return const_cast<MainWindow*>(MainWindow::getMainWindow());
}
