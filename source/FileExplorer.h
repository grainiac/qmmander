/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A widget class which represents a FileExplorer (one side of the
** splitted UI) of Qmmander. A FileExplorer can hold 1-n FileExplorerViews
** in its QTabWidget.
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
#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include <QtWidgets/QWidget>
#include "winfileinfo.h"

class FileExplorerView;

class QDirModel;
class QTableWidget;
class QFileInfo;
class QModelIndex;

typedef QList<QFileInfo> QFileInfoList;

namespace Ui
{
    class FileExplorer;
}

class FileExplorer : public QWidget
{
    Q_OBJECT
public:
    FileExplorer(QWidget *parent = 0);
    ~FileExplorer();

    void setActive(bool isActive);
    bool isActive();
    void setStatusLine(QString text);
    void setTabTitle(QString text);
    void copyTab(FileExplorerView* source);
    void closeTab();    
    void newTab();      

    QString getQuickFilterText() const;
    bool    getQuickFilterState() const;

    WinFileInfoList getSelectedFiles() const;
    QString         getSelectedPath() const;
    void refresh() const;

    const FileExplorerView* getActiveView() const;
    QString swapViews(QString pathLineOfViewIn);

    void showDirectoryTreeAction();

    void emitFileExplorerActivated();

signals:
    void fileExplorerActivated(FileExplorer* explorer);

public slots:
    void setNextTabActive();
    void setPreviousTabActive();

protected:
    void changeEvent(QEvent *e);

private slots:
    void onQuickFilterTextChanged(QString text);
    void onButtonQuickFilterToggled(bool checked);
    void closeTabButtonPressed(int tabIndex2Close);
    void currentTabIndexChanged(int index);

private:
    Ui::FileExplorer* ui_;
    bool isActive_;
};

#endif // FILEEXPLORER_H
