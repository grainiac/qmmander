/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** A widget class providing a file view in the QTabWidget of a FileExplorer.
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
#ifndef FILEEXPLORERVIEW_H
#define FILEEXPLORERVIEW_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QUrl>
#include <windows.h>
#include "winfileinfo.h"
#include "mainwindow.h"

class QDirModel;
class QModelIndex;
class QPushButton;
class QMouseEvent;
class QListWidgetItem;
class QTreeWidgetItem;
class FileExplorer;
class FileExplorerTarget;
class FileExplorerHistory;

namespace Ui
{
    class FileExplorerView;
}

class FileExplorerView : public QWidget
{
    Q_OBJECT
public:
    FileExplorerView(const FileExplorer& fileExplorer, QWidget *parent = 0);
    FileExplorerView(const FileExplorerView& source);
    ~FileExplorerView();

    const FileExplorer* getFileExplorer() const;

    bool isActive() const;
    void checkActiveState() const;
    void emitDriveState() const;
    void emitViewState() const;
    void emitHistoryState() const;

    ListViewMode viewMode() const;
    void setViewMode(ListViewMode mode) const;
    void setDetailsMode() const;
    void setListMode() const;
    void setIconMode(bool bBigIcons=false) const;

    bool changeDir(QString path);
    bool changeDirNoHistory(QString path);
    void updateOnQuickFilterTextChanged() const;

    bool isDirectoryTreeCollapsed() const;
    void collapseDirectoryTree(bool collapse=true) const;

    WinFileInfoList getSelectedFiles() const;
    QString         getPathLine() const;

    void refresh() const;
    void setFileExplorer(FileExplorer* fileExplorer);

public slots:
    void filesWereDroppedIn(QTableWidgetItem* item, QList<QUrl> files, Qt::DropAction action);
    void filesWereDroppedIn(QListWidgetItem* item, QList<QUrl> files, Qt::DropAction action);
    void historyBackward();
    void historyForward();
    void driveListChanged();

signals:    
    void viewStateChanged(int mode, bool showDirectoryTree) const;
    void historyStateChanged(const FileExplorerView* view, int index, int historyCount) const;
    void driveChanged() const;

protected:
    void changeEvent(QEvent *e);    
    void paintEvent (QPaintEvent* e);
    void mousePressEvent(QMouseEvent* e);

private slots:
    void onButtonDirUp(bool state);
    void onButtonRootDir();
    void onButtonCopyPath2Clipboard();
    void focusActualView() const;
    void drivesComboBoxIndexChanged(QString newDrive);
    void treeItemChanged(const QModelIndex& index);
    void tableHeaderClicked(int collumn);
    void tableItemDblClicked(int row, int collumn);
    void handleClickedItem(WinFileInfo fileInfo);
    void tableItemSelectionChanged();
    void listItemSelectionChanged();
    void listItemDblClicked(QListWidgetItem* item);
    void networkTreeItemChanged(QTreeWidgetItem* item, int collumn);

private:
    void disableUIBeforeUpdate();
    void enableUIAfterUpdate();
    void sortFileInfoList();
    void checkDriveList();
    void setupDriveComboBox();
    void updateStatusLine();
    void updateStyleSheets();
    void updateViews();
    void updateViewActions();
    void updateDriveActions();
    void updateTreeView(QString actualPath);
    void updateTabTitle(QString actualPath);
    void updatePathEdit(QString actualPath);
    void updateHistory(QString actualPath);
    void processDroppedFiles(QList<QUrl> files, QString to, Qt::DropAction action);
    int getMatchingDriveComboBoxIndexFromPathLine();
    void setDriveComboBoxIndexFromPathLine();
    bool changeDirNeeded();

    Ui::FileExplorerView*    ui_;
    bool                     alreadyShown_;
    ListViewMode             viewMode_;
    FileExplorer*            fileExplorer_;
    FileExplorerTarget*      fileExplorerTarget_;
    FileExplorerHistory*     fileExplorerHistory_;
    bool                     performChangeDirWithoutHistoryUpdate_;
};

#endif // FILEEXPLORERVIEW_H
