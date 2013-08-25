/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** This is the MainWindow-Widget of the Qmmander Application.
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QTableWidgetItem>
#include <QEvent>
#include "winfileinfo.h"

Q_DECLARE_METATYPE( WinFileInfo );

class ArchiveCommand;
class ChangeFileDateTimeCommand;
class FileExplorer;
class FileExplorerView;
class DriveCheckerThread;
class QSplitter;
class QFileInfo;
class QStringList;
class UpdateQmmanderCommand;

namespace Ui
{
    class MainWindow;
}

enum ListViewMode { DETAILS=0, LIST, SMALLICONS, BIGICONS };

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:    
    MainWindow(const QSplashScreen* pSplashScreen, QWidget *parent = 0);
    ~MainWindow();

    const FileExplorer* const getActiveExplorer() const;
    const FileExplorer* const getInactiveExplorer() const;
    bool isLeftExplorer(const FileExplorer* const explorer) const;

    int getDriveCount() const;

    void setSplashScreenMessage(QString message, QColor color = Qt::black) const;

signals:
    void driveListChanged();

public slots:
    void actionArchiverToolbar(bool checked);
    void actionAttributesDate();
    void actionBigIconsView();
    void actionCalculator();
    void actionCheck4Updates();
    void actionCloseTab();
    void actionCompress();
    void actionConnectNetworkDrive();
    void actionCopy();
    void actionCopyTab();
    void actionDecompress();
    void actionDelete();
    void actionDelete2Trash();
    void actionDesktop();
    void actionDetailsView();
    void actionDisconnectNetworkDrive();
    void actionDosBox();
    void actionDriveChanged();
    void actionDrivesToolbar(bool checked);
    void actionFileProperties();
    void actionHistoryBackward();
    void actionHistoryForward();
    void actionListView();
    void actionMove();
    void actionNavigationToolbar(bool checked);
    void actionNewFolder();
    void actionNewTab();
    void actionNextTab();
    void actionNextExplorer();
    void actionPreviousTab();
    void actionQuit();
    void actionRefresh();
    void actionRename();
    void actionSearch();
    void actionSmallIconsView();
    void actionSplitMode();
    void actionSwapViews();
    void actionShowDirectoryTree();
    void actionShowAbout();
    void actionShowAboutQt();
    void actionTabToolbar(bool checked);
    void actionToolsToolbar(bool checked);
    void actionViewsToolbar(bool checked);

    void activeExplorerChanged(FileExplorer* activeExplorer);
    void driveCountChanged();    

    void reflectCurrentViewState(int mode, bool showDirectoryTree);
    void reflectDriveChanged();
    void reflectHistoryState(const FileExplorerView* view, int index, int historyCount);

    void archiveCommandFinished(ArchiveCommand* command);
    void changeFileDateTimeCommandFinished(ChangeFileDateTimeCommand* command);
    void updateQmmanderCommandFinished(UpdateQmmanderCommand* command);

protected:
    void changeEvent(QEvent *e);
    void paintEvent(QPaintEvent* e);
    void closeEvent(QCloseEvent *);
    bool event(QEvent *event);

private:
    void initializeIconCacheMap();
    QString getDesktopFolder();
    void processResult(QIODevice* source);
    void readSettings();
    void writeSettings();
    void setupDriveButtons();
    void switch2NextExplorer();

    Ui::MainWindow* ui_;
    bool check4Updates_;
    const QSplashScreen* splashScreen_;
    FileExplorer* leftExplorer_;
    FileExplorer* rightExplorer_;
    QSplitter* splitter_;
    bool alreadyShown_;
    bool isReflectingViewStates_;

    DriveCheckerThread* driveCheckerThread_;
};

#endif // MAINWINDOW_H

