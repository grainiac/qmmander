/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A specialized version of QListWidget.
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
#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QListWidget>
#include <QUrl>
#include "winfileinfo.h"
#include "mainwindow.h"

class FileListWidget : public QListWidget
{
    Q_OBJECT
public:
    FileListWidget(QWidget* pParent=0);

    WinFileInfoList getSelectedFiles();
    QList<QUrl> getURLList(WinFileInfoList list);
    QString selectionInfosFromListWidget();
    void fillFileList(const WinFileInfoPtrList& files, const WinFileInfoIndices& visibleIndices, ListViewMode mode);

signals:
    void filesDropped(QListWidgetItem* pItem, QList<QUrl> listFiles, Qt::DropAction dropAction);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent* e);
private:
    QListWidgetItem* itemFrom_;
};

#endif // FILELISTWIDGET_H
