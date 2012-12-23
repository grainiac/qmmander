/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A specialized version of QTableWidget.
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
#ifndef FILETABLEWIDGET_H
#define FILETABLEWIDGET_H

#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include <QUrl>
#include "winfileinfo.h"

class MainWindow;
class FileExplorerView;

class FileTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    FileTableWidget(QWidget* parent=0);

    void setParentFileExplorerView(const FileExplorerView* const view);

    WinFileInfoList getSelectedFiles();
    QList<QUrl> getURLList(WinFileInfoList list);
    QString selectionInfosFromTableWidget();
    void fillFileTable(const WinFileInfoPtrList& files, const WinFileInfoIndices& visibleIndices);

signals:
    void filesDropped(QTableWidgetItem* pItem, QList<QUrl> listFiles, Qt::DropAction dropAction);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent* event);
    void paintEvent (QPaintEvent* event);

private slots:
    void headerSectionResized(int logicalIndex, int oldSize, int newSize);

private:
    void saveCollumnWidth(int collumn, int width);
    int getCollumnWidth(int collumn);
    MainWindow* getMainWindow();

    FileExplorerView const * m_parentView;
    QTableWidgetItem* m_itemFrom;
    bool m_alreadyShown;
};

#endif // FILETABLEWIDGET_H
