/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
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
#ifndef CALLHELPERAPPCOMMAND_H
#define CALLHELPERAPPCOMMAND_H

#include "command.h"

class MainWindow;

class CallHelperAppCommand : public Command
{
public:
    virtual void execute();

    static CallHelperAppCommand* getWindowsCalculatorCommand();
    static CallHelperAppCommand* getDosBoxCommand();
    static CallHelperAppCommand* getNetworkDriveConnectDialogCommand();
    static CallHelperAppCommand* getNetworkDriveDisconnectDialogCommand();
    static CallHelperAppCommand* getFileInformationDialog();

private:
    void executeWindowsCalculator();
    void executeDosBox();
    void executeNetworkDriveConnectDialog();
    void executeNetworkDriveDisconnectDialog();
    void executeFileInformationDialog();
    MainWindow* getMainWindow();

    enum CommandType
    {
        WINDOWS_CALCULATOR_COMMAND=0,
        DOS_BOX_COMMAND,
        NETWORK_DRIVE_CONNECT_DIALOG,
        NETWORK_DRIVE_DISCONNECT_DIALOG,
        FILE_INFORMATION_DIALOG
    };

    CommandType type_;

protected:
    CallHelperAppCommand(){};
    CallHelperAppCommand(CommandType type);
};

#endif // CALLHELPERAPPCOMMAND_H
