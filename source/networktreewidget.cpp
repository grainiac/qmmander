/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A specialized version of QTreeWidget.
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
#include "networktreewidget.h"
#include "winfileinfo.h"
#include "networkresource.h"

Q_DECLARE_METATYPE ( WinFileInfo )
Q_DECLARE_METATYPE ( NetworkResource )

NetworkTreeWidget::NetworkTreeWidget(QWidget* parent)
:   QTreeWidget(parent)
{       
     setHeaderHidden(true);
     LPNETRESOURCE networkResource = NULL;
     setColumnCount(4);

     /*#define RESOURCEDISPLAYTYPE_GENERIC 0
     #define RESOURCEDISPLAYTYPE_DOMAIN 1
     #define RESOURCEDISPLAYTYPE_SERVER 2
     #define RESOURCEDISPLAYTYPE_SHARE 3
     #define RESOURCEDISPLAYTYPE_FILE 4
     #define RESOURCEDISPLAYTYPE_GROUP 5
     #define RESOURCEDISPLAYTYPE_NETWORK 6
     #define RESOURCEDISPLAYTYPE_ROOT 7
     #define RESOURCEDISPLAYTYPE_SHAREADMIN 8
     #define RESOURCEDISPLAYTYPE_DIRECTORY 9
     #define RESOURCEDISPLAYTYPE_TREE 10*/

     populateTree(networkResource, NULL);
}

QTreeWidgetItem* NetworkTreeWidget::getNetworkItem(NetworkResource* networkResource)
{
    QTreeWidgetItem* item=0;

    switch(networkResource->networkResource_.dwDisplayType)
    {
    case (RESOURCEDISPLAYTYPE_NETWORK):
        item=new QTreeWidgetItem(QStringList(networkResource->getDisplayName()));
        item->setIcon(0,QIcon(":/images/images/network16.png"));
        break;
    case (RESOURCEDISPLAYTYPE_DOMAIN):
        item=new QTreeWidgetItem(QStringList(networkResource->getDisplayName()));
        item->setIcon(0,QIcon(":/images/images/domain16.png"));
        break;
    case (RESOURCEDISPLAYTYPE_SERVER):
        item=new QTreeWidgetItem(QStringList(networkResource->getDisplayName()));
        item->setIcon(0,QIcon(":/images/images/computer16.png"));
        break;
    case (RESOURCEDISPLAYTYPE_SHARE):
        item=new QTreeWidgetItem(QStringList(networkResource->getDisplayName()));
        item->setIcon(0,QIcon(":/images/images/netfolder16.png"));
        break;
    case (RESOURCEDISPLAYTYPE_FILE):
    case (RESOURCEDISPLAYTYPE_GROUP):
    case (RESOURCEDISPLAYTYPE_GENERIC):
    default:
        break;
    }
    // Add NetworkResource object to items user data
    if(item)
        item->setData(0, Qt::UserRole, qVariantFromValue(*networkResource));
    return item;
}

void NetworkTreeWidget::populateTree(LPNETRESOURCE networkResource, QTreeWidgetItem* actualItem)
{
    DWORD dwResult, dwResultEnum;
    HANDLE hEnum;
    DWORD cbBuffer = 16000;     // 16K is a good size
    DWORD cEntries = (DWORD)-1; // enumerate all possible entries
    LPNETRESOURCE lpnrLocal;    // pointer to enumerated structures

    dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, // all network resources
                            RESOURCETYPE_ANY,   // all resources
                            0,  // enumerate all resources
                            networkResource,       // NULL first time the function is called
                            &hEnum);    // handle to the resource

    if(dwResult==NO_ERROR)
    {
        lpnrLocal=(LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);

        if(lpnrLocal!=NULL)
        {
            do
            {
                // Initialize the buffer.
                ZeroMemory(lpnrLocal, cbBuffer);

                // Call the WNetEnumResource function to continue the enumeration.
                dwResultEnum = WNetEnumResource(hEnum,  // resource handle
                                                &cEntries,      // defined locally as -1
                                                lpnrLocal,      // LPNETRESOURCE
                                                &cbBuffer);     // buffer size
                // If the call succeeds, loop through the structures.
                if(dwResultEnum==NO_ERROR)
                {
                    for (unsigned int i=0; i < cEntries; i++)
                    {
                        // If the NETRESOURCE structure represents a container resource, call the EnumerateFunc function recursively.
                        WinFileInfo* fileInfo=0;
                        QString provider=QString::fromUtf16((ushort*)lpnrLocal[i].lpProvider);
                        QString remoteName=QString::fromUtf16((ushort*)lpnrLocal[i].lpRemoteName);
                        QString comment=QString::fromUtf16((ushort*)lpnrLocal[i].lpComment);
                        QString itemTitle=remoteName;
                        if(comment!="")
                            itemTitle=itemTitle.remove('\\')+QString(" (")+comment+QString(")");
                        else
                            itemTitle=itemTitle.remove('\\');

                        QTreeWidgetItem* item=0;
                        NetworkResource* aNetworkResource=new NetworkResource(lpnrLocal[i]);

                        switch(lpnrLocal[i].dwDisplayType)
                        {
                        case (RESOURCEDISPLAYTYPE_NETWORK):
                            item=new QTreeWidgetItem(QStringList(provider));
                            item->setIcon(0,QIcon(":/images/images/network16.png"));
                            item->setData(0, Qt::UserRole, qVariantFromValue(*aNetworkResource));
                            insertTopLevelItem(0,item);
                            break;
                        case (RESOURCEDISPLAYTYPE_DOMAIN):
                            item=new QTreeWidgetItem(QStringList(remoteName));
                            item->setIcon(0,QIcon(":/images/images/domain16.png"));
                            item->setData(0, Qt::UserRole, qVariantFromValue(*aNetworkResource));
                            actualItem->insertChild(actualItem->childCount(), item);
                            break;
                        case (RESOURCEDISPLAYTYPE_SERVER):
                            fileInfo=new WinFileInfo(remoteName, true);
                            item=new QTreeWidgetItem(QStringList(itemTitle));
                            item->setIcon(0,QIcon(":/images/images/computer16.png"));
                            item->setData(0, Qt::UserRole, qVariantFromValue(*aNetworkResource));
                            item->setData(0, UserRole_2, qVariantFromValue(*fileInfo) );
                            actualItem->insertChild(actualItem->childCount(), item);
                            break;
                        case (RESOURCEDISPLAYTYPE_SHARE):
                            fileInfo=new WinFileInfo(remoteName, false);
                            item=new QTreeWidgetItem(QStringList(remoteName.right(remoteName.length()-remoteName.lastIndexOf('\\')-1)));
                            item->setIcon(0,QIcon(":/images/images/netfolder16.png"));
                            item->setData(0, Qt::UserRole, qVariantFromValue(*aNetworkResource));
                            item->setData(0, UserRole_2, qVariantFromValue(*fileInfo) );
                            actualItem->insertChild(actualItem->childCount(), item);
                            break;
                        case (RESOURCEDISPLAYTYPE_FILE):
                            break;
                        case (RESOURCEDISPLAYTYPE_GROUP):
                            break;
                        case (RESOURCEDISPLAYTYPE_GENERIC):
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            while (dwResultEnum != ERROR_NO_MORE_ITEMS);

            // Call the GlobalFree function to free the memory.
            GlobalFree((HGLOBAL) lpnrLocal);

            // Call WNetCloseEnum to end the enumeration.
            WNetCloseEnum(hEnum);
        }
    }
}

void NetworkTreeWidget::networkTreeItemClicked(QTreeWidgetItem* item, int collumn)
{
    QVariant variant=item->data(collumn, Qt::UserRole);

     if(variant.type()==QVariant::UserType)
    {
        NetworkResource netRes=variant.value<NetworkResource>();        
        if(netRes.networkResource_.dwDisplayType==RESOURCEDISPLAYTYPE_SHARE)
        {
            emit(networkShareItemClicked(item, collumn));
        }
    }
}

void NetworkTreeWidget::networkTreeItemDblClicked(QTreeWidgetItem* item, int collumn)
{
    QVariant variant=item->data(collumn, Qt::UserRole);

    if(variant.type()==QVariant::UserType)
    {
        NetworkResource netRes=variant.value<NetworkResource>();
        if(netRes.networkResource_.dwDisplayType==RESOURCEDISPLAYTYPE_SHARE)
        {
            emit(networkShareItemClicked(item, collumn));
        }
        else
        {
            if(netRes.networkResource_.dwDisplayType==RESOURCEDISPLAYTYPE_SERVER)
                emit(networkShareItemClicked(item, collumn));

            if(item->childCount()==0)
            {
                NETRESOURCE stNetRes=netRes;
                populateTree(&stNetRes, item);
            }
        }
    }
}
