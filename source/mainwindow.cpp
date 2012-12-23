/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#include "mainwindow.h"
#include "version.h"
#include "ui_mainwindow.h"
#include "FileExplorer.h"
#include "FileExplorerView.h"
#include <QDebug>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QDirModel>
#include <QLocale>
#include <QTranslator>
#include <QSettings>
#include <QtWidgets/QMessageBox>
#include <QNetworkAccessManager>
#include <QtWidgets/QFileSystemModel>
#include <QKeyEvent>
#include "drivecheckerthread.h"
#include "shlobj.h"

#include "aboutcommand.h"
#include "archivecommand.h"
#include "callhelperappcommand.h"
#include "changefiledatetimecommand.h"
#include "copycommand.h"
#include "deletecommand.h"
#include "deletetorecyclebincommand.h"
#include "movecommand.h"
#include "newfoldercommand.h"
#include "renamecommand.h"
#include "updateqmmandercommand.h"

#define SHGFP_TYPE_CURRENT 0

QFileSystemModel*      MainWindow::fileSystemModel_=0;
MainWindow*            MainWindow::mainWindow_=0;
TableItemPrototypes    MainWindow::tableItemPrototypeMap_;
FileTypeIcons          MainWindow::iconCacheMap_;
QNetworkAccessManager* MainWindow::netAccessManager_=new QNetworkAccessManager();

MainWindow::MainWindow(const QSplashScreen* pSplashScreen, QWidget *parent)
:   QMainWindow(parent),
    ui_(new Ui::MainWindow),
    splashScreen_(pSplashScreen),
    leftExplorer_(0),
    rightExplorer_(0),
    alreadyShown_(false)
{    
    mainWindow_=this;

    // Setup drive checker thread
    driveCheckerThread_=new DriveCheckerThread(this);
    connect(driveCheckerThread_, SIGNAL(driveCountChanged()), this, SLOT(driveCountChanged()));

    qRegisterMetaType<WinFileInfo>("WinFileInfo");
    qRegisterMetaTypeStreamOperators<WinFileInfo>("WinFileInfo");

    ui_->setupUi(this);

    // Empty item for QTableWidgets prototype-map
    MainWindow::getTableItemPrototypes()->insert("__EMPTY_ITEM_LA__", QTableWidgetItem(""));
    (*MainWindow::getTableItemPrototypes())["__EMPTY_ITEM_LA__"].setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    MainWindow::getTableItemPrototypes()->insert("__EMPTY_ITEM_RA__", QTableWidgetItem(""));
    (*MainWindow::getTableItemPrototypes())["__EMPTY_ITEM_RA__"].setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);

    initializeIconCacheMap();

    splitter_      = new QSplitter(this);
    splitter_->setAttribute(Qt::WA_DeleteOnClose);
    setSplashScreenMessage("Loading left file explorer...");
    leftExplorer_  = new FileExplorer(splitter_);
    leftExplorer_->setAttribute(Qt::WA_DeleteOnClose);
    setSplashScreenMessage("Loading right file explorer...");
    rightExplorer_ = new FileExplorer( splitter_);
    rightExplorer_->setAttribute(Qt::WA_DeleteOnClose);

    splitter_->setHandleWidth(3);
    ui_->horizontalLayout->addWidget(splitter_);

    actionDrivesToolbar(false);
    actionNavigationToolbar(false);
    actionArchiverToolbar(false);
    actionTabToolbar(false);
    actionToolsToolbar(false);
    actionViewsToolbar(false);

    readSettings();

    if(check4Updates_)
        actionCheck4Updates();
}

MainWindow::~MainWindow()
{
    driveCheckerThread_->terminate();
    delete driveCheckerThread_;
    if(fileSystemModel_)
        delete fileSystemModel_;
    delete ui_;
}

void MainWindow::initializeIconCacheMap()
{
    // Load folder icons
    iconCacheMap_["ICON_FOLDER_WITH_LINK16"]=QIcon(QPixmap(":/images/images/folder_link16.png"));
    iconCacheMap_["ICON_FOLDER_WITH_LINK16"]=QIcon(QPixmap(":/images/images/folder_link16.png"));
    iconCacheMap_["ICON_FOLDER_WITH_LINK32"]=QIcon(QPixmap(":/images/images/folder_link32.png"));

    // Load drive icons
    iconCacheMap_["DRIVE_REMOVABLE_FDD"]=QIcon(QPixmap(":/images/images/drivefdd16.png"));
    iconCacheMap_["DRIVE_REMOVABLE_STICK"]=QIcon(QPixmap(":/images/images/drivestick16.png"));
    iconCacheMap_["DRIVE_FIXED"]=QIcon(QPixmap(":/images/images/drivehdd16.png"));
    iconCacheMap_["DRIVE_FIXED_SYS"]=QIcon(QPixmap(":/images/images/drivehddsys16.png"));
    iconCacheMap_["DRIVE_REMOTE"]=QIcon(QPixmap(":/images/images/drivenet16.png"));
    iconCacheMap_["DRIVE_CDROM"]=QIcon(QPixmap(":/images/images/driveoptical16.png"));

    iconCacheMap_["DRIVE_REMOVABLE_FDD48"]=QIcon(QPixmap(":/images/images/drivefdd48.png"));
    iconCacheMap_["DRIVE_REMOVABLE_STICK48"]=QIcon(QPixmap(":/images/images/drivestick48.png"));
    iconCacheMap_["DRIVE_FIXED48"]=QIcon(QPixmap(":/images/images/drivehdd48.png"));
    iconCacheMap_["DRIVE_FIXED_SYS48"]=QIcon(QPixmap(":/images/images/drivehddsys48.png"));
    iconCacheMap_["DRIVE_REMOTE48"]=QIcon(QPixmap(":/images/images/drivenet48.png"));
    iconCacheMap_["DRIVE_CDROM48"]=QIcon(QPixmap(":/images/images/drivedvd48.png"));
}

const MainWindow* MainWindow::getMainWindow()
{
    return mainWindow_;
}

QNetworkAccessManager& MainWindow::getNetworkAccessManager()
{
    return *netAccessManager_;
}

void MainWindow::setSplashScreenMessage(QString message, QColor color) const
{
    if(splashScreen_)
    {
        const_cast<QSplashScreen*>(splashScreen_)->showMessage(message, Qt::AlignBottom | Qt::AlignLeft, color);
        qApp->processEvents();
    }
}

const QFileSystemModel* MainWindow::getFileSystemModel()
{
    if(!fileSystemModel_)
    {
        fileSystemModel_=new QFileSystemModel();
        fileSystemModel_->setResolveSymlinks(true);
        QDir::Filters filters=QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
        fileSystemModel_->setFilter(filters);
    }
    return fileSystemModel_;
}

TableItemPrototypes* MainWindow::getTableItemPrototypes()
{
    return &tableItemPrototypeMap_;
}

FileTypeIcons* MainWindow::getFileTypeIcons()
{
    return &iconCacheMap_;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui_->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::paintEvent (QPaintEvent* e)
{
    QMainWindow::paintEvent(e);
    if(!alreadyShown_)
    {
        alreadyShown_=true;
        // The left explorer should be active at startup        
        leftExplorer_->setActive(true);
        setupDriveButtons();
        emit(driveListChanged());
        driveCheckerThread_->start(QThread::LowPriority);
    }
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    writeSettings();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::PreviousChild))
        {
            return true;
        }
    }
    return QWidget::event(event);
}

void MainWindow::setupDriveButtons()
{
    QList<QAction*> listActions=ui_->drivesToolBar->actions();
    for(int i=0; i<listActions.count(); i++)
    {
        QAction* pAction=listActions.at(i);
        if(pAction->text()!="Desktop")
        {
            delete pAction;
        }
    }

    ui_->drivesToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QStringList driveList=WinFileInfo::getDriveList();
    QAction* pAction=0;
    for(int i=0; i<driveList.count(); i++)
    {
        QString strDriveName=driveList.at(i);

        const QIcon* icon=0;
        quint32 uRet=GetDriveType(strDriveName.toStdWString().c_str());
        switch(uRet)
        {
        case DRIVE_UNKNOWN:
        case DRIVE_NO_ROOT_DIR:
            break;
        case DRIVE_REMOVABLE:
            if(strDriveName=="A:" || strDriveName=="B:")
                icon=&iconCacheMap_["DRIVE_REMOVABLE_FDD48"];
            else
                icon=&iconCacheMap_["DRIVE_REMOVABLE_STICK48"];
            break;
        case DRIVE_FIXED:
            if(strDriveName=="C:")
                icon=&iconCacheMap_["DRIVE_FIXED_SYS48"];
            else
                icon=&iconCacheMap_["DRIVE_FIXED48"];
            break;
        case DRIVE_REMOTE:
            icon=&iconCacheMap_["DRIVE_REMOTE48"];
            break;
        case DRIVE_CDROM:
            icon=&iconCacheMap_["DRIVE_CDROM48"];
            break;
        case DRIVE_RAMDISK:
            icon=&iconCacheMap_["DRIVE_FIXED48"];
            break;
        }

        pAction=new QAction(*icon,strDriveName,this);
        pAction->setCheckable(true);
        connect(pAction, SIGNAL(triggered()), this, SLOT(actionDriveChanged()));
        pAction->setText(strDriveName);
        pAction->setIconText(strDriveName);
        ui_->drivesToolBar->addAction(pAction);
    }    
}

void MainWindow::driveCountChanged()
{   
    setupDriveButtons();
    emit(driveListChanged());
}

void MainWindow::actionArchiverToolbar(bool checked)
{
    if(checked)
        ui_->archiverToolBar->show();
    else
        ui_->archiverToolBar->hide();
}

void MainWindow::actionAttributesDate()
{
    ChangeFileDateTimeCommand* command=ChangeFileDateTimeCommand::changeFileDateTimeCommandForActualSelection();
    connect(command, SIGNAL(changeFileDateTimeCommandFinished(ChangeFileDateTimeCommand*)), this, SLOT(changeFileDateTimeCommandFinished(ChangeFileDateTimeCommand*)));
    command->execute();
}

void MainWindow::actionBigIconsView()
{
    if(!isReflectingViewStates_)
    {
        getActiveExplorer()->getActiveView()->setIconMode(true);
    }
}

void MainWindow::actionCompress()
{
    ArchiveCommand* archiveCommand=ArchiveCommand::archiveCommandForCompression();
    connect(archiveCommand, SIGNAL(archiveCommandFinished(ArchiveCommand*)), this, SLOT(archiveCommandFinished(ArchiveCommand*)));
    archiveCommand->execute();
}

void MainWindow::actionDecompress()
{
    ArchiveCommand* archiveCommand=ArchiveCommand::archiveCommandForDecompression();
    connect(archiveCommand, SIGNAL(archiveCommandFinished(ArchiveCommand*)), this, SLOT(archiveCommandFinished(ArchiveCommand*)));
    archiveCommand->execute();
}

void MainWindow::archiveCommandFinished(ArchiveCommand* command)
{
    delete command;
}

void MainWindow::actionCopy()
{
    QSharedPointer<CopyCommand> copyCommand(CopyCommand::copyCommandForActualSelection());
    copyCommand->execute();
    getInactiveExplorer()->refresh();
}

void MainWindow::actionCalculator()
{
    QSharedPointer<CallHelperAppCommand> appCommand(CallHelperAppCommand::getWindowsCalculatorCommand());
    appCommand->execute();
}

void MainWindow::actionCheck4Updates()
{
    UpdateQmmanderCommand* updateCommand=UpdateQmmanderCommand::getUpdateCommand();
    connect(updateCommand, SIGNAL(updateQmmanderCommandFinished(UpdateQmmanderCommand*)), this, SLOT(updateQmmanderCommandFinished(UpdateQmmanderCommand*)));
    updateCommand->execute();
}

void MainWindow::updateQmmanderCommandFinished(UpdateQmmanderCommand* command)
{
    delete command;
}

void MainWindow::actionCopyTab()
{
    FileExplorerView* view2Copy=const_cast<FileExplorerView*>(getActiveExplorer()->getActiveView());
    const_cast<FileExplorer*>(getInactiveExplorer())->copyTab(view2Copy);
}

void MainWindow::actionCloseTab()
{
    const_cast<FileExplorer*>(getActiveExplorer())->closeTab();
}

void MainWindow::actionConnectNetworkDrive()
{
    QSharedPointer<CallHelperAppCommand> appCommand(CallHelperAppCommand::getNetworkDriveConnectDialogCommand());
    appCommand->execute();
}

void MainWindow::actionDisconnectNetworkDrive()
{
    QSharedPointer<CallHelperAppCommand> appCommand(CallHelperAppCommand::getNetworkDriveDisconnectDialogCommand());
    appCommand->execute();
}

void MainWindow::actionDosBox()
{
    QSharedPointer<CallHelperAppCommand> appCommand(CallHelperAppCommand::getDosBoxCommand());
    appCommand->execute();
}

void MainWindow::actionDesktop()
{
    const FileExplorer* explorer=getActiveExplorer();
    const_cast<FileExplorerView*>(explorer->getActiveView())->changeDir(getDesktopFolder());
}

QString MainWindow::getDesktopFolder()
{
    wchar_t desktopFolder[MAX_PATH];
    SHGetFolderPath( 0, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, desktopFolder );
    QString strDestop=QString::fromUtf16((ushort*)desktopFolder);
    return strDestop;
}

void MainWindow::actionDelete()
{
    QSharedPointer<DeleteCommand> deleteCommand(DeleteCommand::deleteCommandForActualSelection());
    deleteCommand->execute();
    getActiveExplorer()->refresh();
}

void MainWindow::actionDelete2Trash()
{
    QSharedPointer<DeleteToRecycleBinCommand> deleteCommand(DeleteToRecycleBinCommand::deleteCommandForActualSelection());
    deleteCommand->execute();
    getActiveExplorer()->refresh();
}

void MainWindow::actionDetailsView()
{
    if(!isReflectingViewStates_)
    {
        getActiveExplorer()->getActiveView()->setDetailsMode();
    }
}

void MainWindow::actionDrivesToolbar(bool checked)
{
    if(checked)
        ui_->drivesToolBar->show();
    else
        ui_->drivesToolBar->hide();
}

void MainWindow::actionDriveChanged()
{
    QAction* action=dynamic_cast<QAction*>(QObject::sender());
    if(action)
    {
        QString path=action->text();
        const FileExplorer* explorer=getActiveExplorer();
        const_cast<FileExplorerView*>(explorer->getActiveView())->changeDir(path);
    }
}

int MainWindow::getDriveCount() const
{
    QList<QAction*> listActions=ui_->drivesToolBar->actions();
    return listActions.count()-1; // minus "Desktop"-action
}

void MainWindow::activeExplorerChanged(FileExplorer* activeExplorer)
{
    if(activeExplorer==leftExplorer_)
        rightExplorer_->setActive(false);
    else
        leftExplorer_->setActive(false);
}

void MainWindow::actionFileProperties()
{
    QSharedPointer<CallHelperAppCommand> appCommand(CallHelperAppCommand::getFileInformationDialog());
    appCommand->execute();
}

void MainWindow::actionHistoryBackward()
{
    const_cast<FileExplorerView*>(getActiveExplorer()->getActiveView())->historyBackward();
}

void MainWindow::actionHistoryForward()
{
    const_cast<FileExplorerView*>(getActiveExplorer()->getActiveView())->historyForward();
}

void MainWindow::actionListView()
{
    if(!isReflectingViewStates_)
    {
        getActiveExplorer()->getActiveView()->setListMode();
    }
}

void MainWindow::actionMove()
{
    QSharedPointer<MoveCommand> moveCommand(MoveCommand::moveCommandForActualSelection());
    moveCommand->execute();
    leftExplorer_->refresh();
    rightExplorer_->refresh();
}

void MainWindow::actionNavigationToolbar(bool checked)
{
    if(checked)
        ui_->navigationToolBar->show();
    else
        ui_->navigationToolBar->hide();
}

void MainWindow::actionNewFolder()
{
    QSharedPointer<NewFolderCommand> newFolderCommand(NewFolderCommand::getNewFolderCommand());
    newFolderCommand->execute();
}

void MainWindow::actionNewTab()
{
    const_cast<FileExplorer*>(getActiveExplorer())->newTab();
}

void MainWindow::actionNextTab()
{
    const_cast<FileExplorer*>(getActiveExplorer())->setNextTabActive();
}

void MainWindow::actionNextExplorer()
{
    switch2NextExplorer();
}

void MainWindow::switch2NextExplorer()
{
    if(getActiveExplorer()==leftExplorer_)
    {
        leftExplorer_->setActive(false);
        rightExplorer_->setActive(true);
    }
    else
    {
        leftExplorer_->setActive(true);
        rightExplorer_->setActive(false);
    }
}

void MainWindow::actionPreviousTab()
{
    const_cast<FileExplorer*>(getActiveExplorer())->setPreviousTabActive();
}

void MainWindow::actionQuit()
{
    qApp->quit();
}

void MainWindow::actionRefresh()
{
    const_cast<FileExplorer*>(getActiveExplorer())->refresh();
}

void MainWindow::actionRename()
{
    QSharedPointer<RenameCommand> renameCommand(RenameCommand::renameCommandForActualSelection());
    renameCommand->execute();
}

void MainWindow::actionShowAbout()
{
    QSharedPointer<AboutCommand> aboutCommand(AboutCommand::getAboutCommand());
    aboutCommand->execute();
}

void MainWindow::actionShowAboutQt()
{
    QMessageBox::aboutQt(this, tr("Qt version"));
}

void MainWindow::changeFileDateTimeCommandFinished(ChangeFileDateTimeCommand *command)
{
    delete command;
}

void MainWindow::actionSearch()
{
    // TODO
}

void MainWindow::actionShowDirectoryTree()
{
    if(!isReflectingViewStates_)
    {
        const_cast<FileExplorer*>(getActiveExplorer())->showDirectoryTreeAction();
    }
}

void MainWindow::actionSplitMode()
{
    if(splitter_->orientation()==Qt::Horizontal)
        splitter_->setOrientation(Qt::Vertical);
    else
        splitter_->setOrientation(Qt::Horizontal);
}

void MainWindow::actionSwapViews()
{
    QString pathOut=leftExplorer_->swapViews(rightExplorer_->getActiveView()->getPathLine());
    rightExplorer_->swapViews(pathOut);
}

void MainWindow::actionSmallIconsView()
{
    if(!isReflectingViewStates_)
    {
        getActiveExplorer()->getActiveView()->setIconMode(false);
    }
}

void MainWindow::actionTabToolbar(bool checked)
{
    if(checked)
        ui_->tabToolBar->show();
    else
        ui_->tabToolBar->hide();
}

void MainWindow::actionToolsToolbar(bool checked)
{
    if(checked)
        ui_->toolsToolBar->show();
    else
        ui_->toolsToolBar->hide();
}

void MainWindow::actionViewsToolbar(bool checked)
{
    if(checked)
        ui_->viewsToolBar->show();
    else
        ui_->viewsToolBar->hide();
}

void MainWindow::reflectCurrentViewState(int mode, bool showDirectoryTree)
{
    isReflectingViewStates_=true;
    switch(mode)
    {
    case LIST:
        ui_->actionList->setChecked(true);
        ui_->actionBigIconsView->setChecked(false);
        ui_->actionSmallIconsView->setChecked(false);
        ui_->actionDetails->setChecked(false);
        break;
    case BIGICONS:
        ui_->actionList->setChecked(false);
        ui_->actionBigIconsView->setChecked(true);
        ui_->actionSmallIconsView->setChecked(false);
        ui_->actionDetails->setChecked(false);
        break;
    case SMALLICONS:
        ui_->actionList->setChecked(false);
        ui_->actionBigIconsView->setChecked(false);
        ui_->actionSmallIconsView->setChecked(true);
        ui_->actionDetails->setChecked(false);
        break;
    case DETAILS:
    default:
        ui_->actionList->setChecked(false);
        ui_->actionBigIconsView->setChecked(false);
        ui_->actionSmallIconsView->setChecked(false);
        ui_->actionDetails->setChecked(true);
    }
    ui_->actionShow_directory_tree->setChecked(showDirectoryTree);
    isReflectingViewStates_=false;
}

void MainWindow::reflectDriveChanged()
{
    QList<QAction*> listActions=ui_->drivesToolBar->actions();
    QString path=const_cast<FileExplorer*>(getActiveExplorer())->getActiveView()->getPathLine();
    QString drive=WinFileInfo::getDriveFromPath(path);
    QString desktop=getDesktopFolder();

    for(int i=0; i<listActions.count(); i++)
    {
        QAction* action=listActions.at(i);
        if(action)
        {
            if(action->text()=="Desktop" && path==desktop)
                action->setChecked(true);
            else if(action->text()==drive && path!=desktop)
                action->setChecked(true);
            else
                action->setChecked(false);
        }
    }
}

void MainWindow::reflectHistoryState(const FileExplorerView* view, int index, int historyCount)
{
    if(getActiveExplorer()==view->getFileExplorer())
    {
        if(historyCount<1)
        {
            ui_->actionBackward->setEnabled(false);
            ui_->actionForward->setEnabled(false);
        }
        else
        {
            ui_->actionBackward->setEnabled(true);
            ui_->actionForward->setEnabled(true);
            if(index==0)
                ui_->actionBackward->setEnabled(false);
            if(index==historyCount-1)
                ui_->actionForward->setEnabled(false);
        }
    }
}

const FileExplorer* const MainWindow::getActiveExplorer() const
{
    if(leftExplorer_->isActive())
        return leftExplorer_;
    else
        return rightExplorer_;
}

const FileExplorer* const MainWindow::getInactiveExplorer() const
{
    if(getActiveExplorer()==rightExplorer_)
        return leftExplorer_;
    else
        return rightExplorer_;
}

bool MainWindow::isLeftExplorer(const FileExplorer* const explorer) const
{    
    if(explorer==leftExplorer_)
        return true;
    return false;
}

void MainWindow::readSettings()
{
    QSettings settings("ASKsoft", "Qmmander");

    restoreGeometry(settings.value("geometry", QByteArray()).toByteArray());
    restoreState(settings.value("layout", QByteArray()).toByteArray());

    // save toolbar states
    bool isVisible=settings.value("tb_navigations_visible", true).toBool();
    ui_->actionNavigation_Toolbar->setVisible(isVisible);
    QPoint pos=settings.value("tb_navigations_pos", QPoint(0,0)).toPoint();
    ui_->navigationToolBar->move(pos);

    isVisible=settings.value("tb_archiver_visible", true).toBool();
    ui_->actionArchiver_Toolbar->setVisible(isVisible);
    pos=settings.value("tb_archiver_pos", QPoint(0,0)).toPoint();
    ui_->archiverToolBar->move(pos);

    isVisible=settings.value("tb_tab_visible", true).toBool();
    ui_->actionTab_Toolbar->setVisible(isVisible);
    pos=settings.value("tb_tab_pos", QPoint(0,0)).toPoint();
    ui_->tabToolBar->move(pos);

    isVisible=settings.value("tb_views_visible", true).toBool();
    ui_->actionViews_Toolbar->setVisible(isVisible);
    pos=settings.value("tb_views_pos", QPoint(0,0)).toPoint();
    ui_->viewsToolBar->move(pos);

    isVisible=settings.value("tb_tools_visible", true).toBool();
    ui_->actionTools_Toolbar->setVisible(isVisible);
    pos=settings.value("tb_tools_pos", QPoint(0,0)).toPoint();
    ui_->toolsToolBar->move(pos);

    isVisible=settings.value("tb_drives_visible", true).toBool();
    ui_->actionDrives_Toolbar->setVisible(isVisible);
    pos=settings.value("tb_drives_pos", QPoint(0,0)).toPoint();
    ui_->drivesToolBar->move(pos);

    // URL to Update Server
    check4Updates_=settings.value("check_4_updates", true).toBool();
}

void MainWindow::writeSettings()
{
    QSettings settings("ASKsoft", "Qmmander");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("layout", saveState());

    // save toolbar states
    settings.setValue("tb_navigations_visible", ui_->actionNavigation_Toolbar->isVisible());
    settings.setValue("tb_navigations_pos", ui_->navigationToolBar->pos());
    settings.setValue("tb_archiver_visible", ui_->actionArchiver_Toolbar->isVisible());
    settings.setValue("tb_archiver_pos", ui_->archiverToolBar->pos());
    settings.setValue("tb_tab_visible", ui_->actionTab_Toolbar->isVisible());
    settings.setValue("tb_tab_pos", ui_->tabToolBar->pos());
    settings.setValue("tb_views_visible", ui_->actionViews_Toolbar->isVisible());
    settings.setValue("tb_views_pos", ui_->viewsToolBar->pos());
    settings.setValue("tb_tools_visible", ui_->actionTools_Toolbar->isVisible());
    settings.setValue("tb_tools_pos", ui_->toolsToolBar->pos());
    settings.setValue("tb_drives_visible", ui_->actionDrives_Toolbar->isVisible());
    settings.setValue("tb_drives_pos", ui_->drivesToolBar->pos());

    // save the update server url
    settings.setValue("check_4_updates", check4Updates_);
    settings.setValue("update_server", QUrl("http://qmmander.googlecode.com/files/QmmanderUpdate.xml"));
}

