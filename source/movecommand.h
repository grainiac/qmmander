/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
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
#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "command.h"
#include "windows.h"

class MoveCommand : public Command
{
public:
   virtual ~MoveCommand();
   virtual void execute();

   static MoveCommand* moveCommandForActualSelection();

protected:
    MoveCommand();

private:
    TCHAR* sourceFiles_;
    TCHAR* destinationFiles_;
};

#endif // MOVECOMMAND_H
