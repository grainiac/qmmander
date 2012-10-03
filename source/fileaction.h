/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Actions on files that can be taken during an unzip or zip operation.
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
#ifndef FILEACTION_H
#define FILEACTION_H

enum  FileAction{ FA_NoAction                   = 0x00000000,
                  FA_OverWrite                  = 0x00000001,
                  FA_OverWriteAll               = 0x00000002,
                  FA_Skip                       = 0x00000004,
                  FA_SkipAll                    = 0x00000008,
                  FA_RenameSource               = 0x00000010,
                  FA_RenameDest                 = 0x00000020,
                  FA_RenameSourceAuto           = 0x00000040,
                  FA_RenameDestAuto             = 0x00000080,
                  FA_OverWriteOlderAndSameAge   = 0x00000100,
                  FA_CopyLargerOverWriteSmaller = 0x00000200,
                  FA_CopySmallerOverWriteLarger = 0x00000400  };

Q_DECLARE_FLAGS(FileActions, FileAction)

#endif // FILEACTION_H
