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
#ifndef DELETECOMMAND_H
#define DELETECOMMAND_H

#include "command.h"
#include "windows.h"

class DeleteCommand : public Command
{
public:
   virtual ~DeleteCommand();
   virtual void execute();

   static DeleteCommand* deleteCommandForActualSelection();

protected:
    DeleteCommand();

private:
    TCHAR* files2Delete_;
};

#endif // DELETECOMMAND_H
