/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
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
#ifndef NETWORKTREEWIDGET_H
#define NETWORKTREEWIDGET_H

#include <QTreeWidget>
#include "networkresource.h"

#define UserRole_2 33

class NetworkTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    NetworkTreeWidget( QWidget * parent = 0 );

signals:
   void networkShareItemClicked(QTreeWidgetItem* item, int collumn);

public slots:
    void networkTreeItemClicked(QTreeWidgetItem* item, int collumn);
    void networkTreeItemDblClicked(QTreeWidgetItem* item, int collumn);

private:
    QTreeWidgetItem* getNetworkItem(NetworkResource* networkResource);
    void populateTree(LPNETRESOURCE networkResource, QTreeWidgetItem* actualItem=0);
};

#endif // NETWORKTREEWIDGET_H
