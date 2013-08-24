/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** NetworkResource stores information about a resource in a windows network.
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
#include "networkresource.h"

NetworkResource::NetworkResource()
{
    ZeroMemory(&networkResource_, sizeof(NETRESOURCE));
}

NetworkResource::NetworkResource(const NETRESOURCE& netResource)
{
    networkResource_ = netResource;
    localName_  = QString::fromUtf16((ushort*)netResource.lpLocalName);
    remoteName_ = QString::fromUtf16((ushort*)netResource.lpRemoteName);
    comment_    = QString::fromUtf16((ushort*)netResource.lpComment);
    provider_   = QString::fromUtf16((ushort*)netResource.lpProvider);
}

NetworkResource::operator const NETRESOURCE&()
{
    networkResource_.lpLocalName  = (WCHAR*) localName_.utf16();
    networkResource_.lpRemoteName = (WCHAR*) remoteName_.utf16();
    networkResource_.lpComment    = (WCHAR*) comment_.utf16();
    networkResource_.lpProvider   = (WCHAR*) provider_.utf16();
    return networkResource_;
}

QString NetworkResource::getDisplayName()
{
    if(remoteName_.isEmpty())
        return comment_;
    return remoteName_.right(remoteName_.length()-remoteName_.lastIndexOf('\\')-1);
}
