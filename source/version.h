/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** This file holds the Qmmander version.
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
#ifndef VERSION_H
#define VERSION_H

#define VERSION_MAJOR 0    // major changes or release "earned" the new number, interface may change.
#define VERSION_MINOR 1    // only minor enhancements, no interface changes ( if there are interfaces :) )
#define VERSION_PATCH 0    // fixlevel
#define VERSION_BUILD 9    // build number, will be incremented for every change in the numbers before.

#define VERSION_STRING "0.1.0-b2-9\0"

#define COPYRIGHT_STRING "Copyright © 2009-2011 Alex Skoruppa"

// Version string format:
// ======================
//
// "VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH-LITERAL-VERSION_BUILD"
//
// The patch level is followed by a literal which indicates the state of development.
// Valid states are:
// "pre":        This is a pre alpha state, means the development on this branch has just begun.
// "a1".."aN":   Alpha versions. Early public testing versions, often unstable.
// "b1".."bN":   Beta versions. Quite stable but there may be some feature changes in the future.
// "rc1".."rcN": Release candidate versions. Feature complete.
//
//  Release versions have NO literals appended!

#endif // VERSION_H
