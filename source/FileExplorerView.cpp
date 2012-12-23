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
#include "FileExplorerView.h"
#include "ui_FileExplorerView.h"
#include "FileExplorer.h"
#include "fileexplorerhistory.h"
#include "fileexplorertarget.h"
#include "archive.h"
#include "dropcommand.h"

#include <QDirModel>
#include <QDateTime>
#include <QScrollBar>
#include <QSettings>
#include <QPushButton>
#include <QClipboard>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QPixmapCache>
#include <QFileSystemModel>

FileExplorerView::FileExplorerView(const FileExplorer& fileExplorer, QWidget *parent)
:   QWidget(parent),
    ui_(new Ui::FileExplorerView),
    alreadyShown_(false),
    viewMode_(DETAILS),
    fileExplorer_(const_cast<FileExplorer*>(&fileExplorer)),
    fileExplorerTarget_(new FileExplorerTarget()),
    fileExplorerHistory_(new FileExplorerHistory()),
    performChangeDirWithoutHistoryUpdate_(false)
{
    ui_->setupUi(this);

    ui_->listWidget->hide();

    ui_->tableWidget->setParentFileExplorerView(this);
    ui_->tableWidget->setColumnCount(5);
    ui_->tableWidget->setCornerButtonEnabled(false);
    ui_->tableWidget->setShowGrid(false);
    ui_->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
    ui_->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Size")));
    ui_->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Last modified")));
    ui_->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Type")));
    ui_->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Attributes")));
    ui_->tableWidget->horizontalHeader()->setSectionsClickable(true);
    ui_->tableWidget->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    MainWindow::getMainWindow()->setSplashScreenMessage(tr("Loading filesystem model for tree view..."));

    QFileSystemModel* pModel=const_cast<QFileSystemModel*>(MainWindow::getFileSystemModel());
    QModelIndex root=pModel->setRootPath("C:");
    ui_->treeView->setModel(pModel);
    ui_->treeView->setRootIndex(root);
    ui_->treeView->setHeaderHidden(true);
    ui_->treeView->setColumnHidden(1, true);
    ui_->treeView->setColumnHidden(2, true);
    ui_->treeView->setColumnHidden(3, true);

    // Add buttons for drives    
    setupDriveComboBox();

    // Connect the TableWidget-Header with slot for sorting
    connect(ui_->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableHeaderClicked(int)));

    // Connect driveChanged signal
    connect(this, SIGNAL(driveChanged()), MainWindow::getMainWindow(), SLOT(reflectDriveChanged()));

    // Connect driveListChanged signal
    connect(MainWindow::getMainWindow(), SIGNAL(driveListChanged()), this, SLOT(driveListChanged()));
}

FileExplorerView::FileExplorerView(const FileExplorerView& source)
:   ui_(new Ui::FileExplorerView()),
    alreadyShown_(false),
    viewMode_(source.viewMode_),
    fileExplorer_(source.fileExplorer_),
    fileExplorerTarget_(new FileExplorerTarget(*source.fileExplorerTarget_)),
    fileExplorerHistory_(new FileExplorerHistory()), // fresh history even when copied
    performChangeDirWithoutHistoryUpdate_(false)
{
    ui_->setupUi(this);

    if(viewMode_==DETAILS)
        ui_->listWidget->hide();
    else
        ui_->tableWidget->hide();

    ui_->lineEditPath->setText(source.ui_->lineEditPath->text());

    ui_->tableWidget->setParentFileExplorerView(&source);
    ui_->tableWidget->setColumnCount(5);
    ui_->tableWidget->setCornerButtonEnabled(false);
    ui_->tableWidget->setShowGrid(false);
    ui_->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
    ui_->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Size")));
    ui_->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem(tr("Last modified")));
    ui_->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Type")));
    ui_->tableWidget->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Attributes")));
    ui_->tableWidget->horizontalHeader()->setSectionsClickable(true);
    ui_->tableWidget->horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

    QAbstractItemModel* pModel=dynamic_cast<QAbstractItemModel*>(const_cast<QFileSystemModel*>(MainWindow::getFileSystemModel()));
    ui_->treeView->setModel(pModel);
    ui_->treeView->setHeaderHidden(true);
    ui_->treeView->setColumnHidden(1, true);
    ui_->treeView->setColumnHidden(2, true);
    ui_->treeView->setColumnHidden(3, true);

    // Connect the TableWidget-Header with slot for sorting
    connect(ui_->tableWidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(tableHeaderClicked(int)));

    // Connect driveChanged signal
    connect(this, SIGNAL(driveChanged()), MainWindow::getMainWindow(), SLOT(reflectDriveChanged()));

    // Add buttons for drives
    setupDriveComboBox();
}

FileExplorerView::~FileExplorerView()
{
    delete ui_;
}

bool FileExplorerView::isActive() const
{
    if(fileExplorer_->isActive() && fileExplorer_->getActiveView()==this)
        return true;
    return false;
}

const FileExplorer* FileExplorerView::getFileExplorer() const
{
    return fileExplorer_;
}

void FileExplorerView::driveListChanged()
{
    setupDriveComboBox();
}

void FileExplorerView::setupDriveComboBox()
{        
    delete ui_->comboBox;
    ui_->comboBox = new QComboBox(this);
    ui_->comboBox->setObjectName(QString::fromUtf8("comboBox"));
    ui_->comboBox->setMinimumSize(QSize(0, 0));
    ui_->comboBox->setMaximumSize(QSize(65, 16777215));
    ui_->comboBox->setMinimumContentsLength(255);

    ui_->comboBox->view()->setFixedWidth(65);
    ui_->horizontalLayoutNavigation->insertWidget(1,ui_->comboBox);

    QStringList driveList=WinFileInfo::getDriveList();    
    FileTypeIcons* pFileTypeIcons=MainWindow::getFileTypeIcons();    

    for(int i=0; i<driveList.count(); i++)
    {
        QString strDriveName=driveList.at(i);        

        const QIcon* pIcon=0;
        quint32 uRet=GetDriveType(strDriveName.toStdWString().c_str());

        switch(uRet)
        {
        case DRIVE_UNKNOWN:
        case DRIVE_NO_ROOT_DIR:
            break;
        case DRIVE_REMOVABLE:
            if(strDriveName=="A:" || strDriveName=="B:")
                pIcon=&(*pFileTypeIcons)["DRIVE_REMOVABLE_FDD"];
            else
                pIcon=&(*pFileTypeIcons)["DRIVE_REMOVABLE_STICK"];
            break;
        case DRIVE_FIXED:
            if(strDriveName=="C:")
                pIcon=&(*pFileTypeIcons)["DRIVE_FIXED_SYS"];
            else
                pIcon=&(*pFileTypeIcons)["DRIVE_FIXED"];
            break;
        case DRIVE_REMOTE:
            pIcon=&(*pFileTypeIcons)["DRIVE_REMOTE"];
            break;
        case DRIVE_CDROM:
            pIcon=&(*pFileTypeIcons)["DRIVE_CDROM"];
            break;
        case DRIVE_RAMDISK:
            pIcon=&(*pFileTypeIcons)["DRIVE_FIXED"];
            break;
        }
        if(pIcon && ui_->comboBox->findText(strDriveName)==-1)
            ui_->comboBox->addItem(*pIcon,strDriveName);
    }

    connect(ui_->comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(drivesComboBoxIndexChanged(QString)));

    QString drive;
    int iIndex;
    if(!alreadyShown_)
    {
        drive="C:";
        iIndex=ui_->comboBox->findText(drive);
    }
    else
    {
        drive=WinFileInfo::getDriveFromPath(getPathLine());
        iIndex=ui_->comboBox->findText(drive);
        if(iIndex==-1)
        {
            drive="C:";
            iIndex=ui_->comboBox->findText(drive);
            changeDir(drive);
        }
    }

    if(iIndex!=-1)
        ui_->comboBox->setCurrentIndex(iIndex);
}

void FileExplorerView::drivesComboBoxIndexChanged(QString newDrive)
{
    if(alreadyShown_)
    {
        QFile newPath(newDrive);
        if(newPath.exists() && changeDirNeeded())
        {
            QModelIndex root=const_cast<QFileSystemModel*>(MainWindow::getFileSystemModel())->setRootPath(newDrive);
            ui_->treeView->setRootIndex(root);
            if(newDrive!="")
                changeDir(newDrive);
        }
        else
        {
            setDriveComboBoxIndexFromPathLine();
        }
    }
}

int FileExplorerView::getMatchingDriveComboBoxIndexFromPathLine()
{
    QString drive=WinFileInfo::getDriveFromPath(getPathLine());
    return ui_->comboBox->findText(drive);
}

bool FileExplorerView::changeDirNeeded()
{
    int newIndex=getMatchingDriveComboBoxIndexFromPathLine();
    if(newIndex!=-1 && ui_->comboBox->currentIndex()!=newIndex)
        return true;
    return false;
}

void FileExplorerView::setDriveComboBoxIndexFromPathLine()
{
    int newIndex=getMatchingDriveComboBoxIndexFromPathLine();
    if(newIndex!=-1)
        ui_->comboBox->setCurrentIndex(newIndex);
}

void FileExplorerView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui_->retranslateUi(this);
        break;
    default:
        break;
    }
}

void FileExplorerView::paintEvent (QPaintEvent* e)
{
    QWidget::paintEvent(e);
    if(!alreadyShown_)
    {
        alreadyShown_=true;

        MainWindow* pMainWindow=0;
        foreach (QWidget *widget, QApplication::allWidgets())
        {
            pMainWindow=dynamic_cast<MainWindow*>(widget);
            if(pMainWindow)
            {                
                connect(this->ui_->pushButtonSwitchViews, SIGNAL(clicked()), pMainWindow, SLOT(actionSwapViews()));
                connect(this, SIGNAL(viewStateChanged(int,bool)), pMainWindow, SLOT(reflectCurrentViewState(int,bool)));
                connect(this, SIGNAL(historyStateChanged(const FileExplorerView*,int,int)), pMainWindow, SLOT(reflectHistoryState(const FileExplorerView*,int,int)));
                connect(this->ui_->tableWidget, SIGNAL(filesDropped(QTableWidgetItem*,QList<QUrl>,Qt::DropAction)),
                        this, SLOT(filesWereDroppedIn(QTableWidgetItem*,QList<QUrl>,Qt::DropAction)));
                connect(this->ui_->listWidget, SIGNAL(filesDropped(QListWidgetItem*,QList<QUrl>,Qt::DropAction)),
                        this, SLOT(filesWereDroppedIn(QListWidgetItem*,QList<QUrl>,Qt::DropAction)));
            }
        }        
        collapseDirectoryTree();
        if(isActive())
            emitViewState();
    }
    emitHistoryState();
}

void FileExplorerView::treeItemChanged(const QModelIndex& index)
{
    if(index.isValid())
    {
        QString strPath=const_cast<QFileSystemModel*>(MainWindow::getFileSystemModel())->filePath(index);
        changeDir(strPath);
    }
}

bool FileExplorerView::changeDirNoHistory(QString path)
{
    performChangeDirWithoutHistoryUpdate_=true;
    bool result=changeDir(path);
    performChangeDirWithoutHistoryUpdate_=false;
    return result;
}

bool FileExplorerView::changeDir(QString path)
{
    if(!fileExplorerTarget_->changeDir(path))
    {
        updateDriveActions();
        return false;
    }

    disableUIBeforeUpdate();

    updateStyleSheets();
    updateTreeView(path);
    sortFileInfoList();
    updateViews();

    QString newPath = fileExplorerTarget_->getCanonicalPath();

    updatePathEdit(newPath);
    updateHistory(newPath);

    enableUIAfterUpdate();

    updateViewActions();
    updateDriveActions();

    updateTabTitle(newPath);
    updateStatusLine();

    if(isActive() && !fileExplorer_->getQuickFilterState())
    {
        checkActiveState();
        ui_->tableWidget->setFocus();
    }

    // Check highlighting of the drive buttons
    checkDriveList();

    return true;
}

void FileExplorerView::disableUIBeforeUpdate()
{
    qApp->setOverrideCursor(Qt::WaitCursor);
    setEnabled(false);
    repaint();
}

void FileExplorerView::enableUIAfterUpdate()
{
    setEnabled(true);
    repaint();
    qApp->restoreOverrideCursor();
}

void FileExplorerView::updateStyleSheets()
{
    if (fileExplorer_->getQuickFilterState())
    {
        ui_->tableWidget->setStyleSheet("QTableView {background-color: rgb(201,219,229);}");
        ui_->listWidget->setStyleSheet("QListView {background-color: rgb(201,219,229);}");
    }
    else
    {
        ui_->tableWidget->setStyleSheet("QTableView {background-color: rgb(255,255,255);}");
        ui_->listWidget->setStyleSheet("QListView {background-color: rgb(255,255,255);}");
    }
}

void FileExplorerView::updateTreeView(QString actualPath)
{
    QModelIndex index=MainWindow::getFileSystemModel()->index(actualPath);
    if(index.isValid())
        ui_->treeView->setCurrentIndex(index);
}

void FileExplorerView::updateViews()
{
    if(viewMode_==DETAILS)
    {
        ui_->tableWidget->fillFileTable(fileExplorerTarget_->getFiles(),
                                        fileExplorerTarget_->getSortedFileIndices());
    }
    else if(viewMode_>DETAILS)
    {
        ui_->listWidget->fillFileList(fileExplorerTarget_->getFiles(),
                                      fileExplorerTarget_->getSortedFileIndices(),
                                      viewMode_);
    }
}

void FileExplorerView::updatePathEdit(QString actualPath)
{
    ui_->lineEditPath->setText(actualPath);
}

void FileExplorerView::updateHistory(QString actualPath)
{
    if(performChangeDirWithoutHistoryUpdate_==false)
        fileExplorerHistory_->updateHistory(actualPath);
    emitHistoryState();
}

void FileExplorerView::updateTabTitle(QString actualPath)
{
    QString newTabTitle=actualPath.right(actualPath.length()-actualPath.lastIndexOf("\\")-1);
    int iLength=newTabTitle.length();
    if(iLength==0)
        newTabTitle=actualPath;
    fileExplorer_->setTabTitle(newTabTitle);
}

void FileExplorerView::updateStatusLine()
{
    if(viewMode_==DETAILS)
        fileExplorer_->setStatusLine(ui_->tableWidget->selectionInfosFromTableWidget());
    else if(viewMode_>DETAILS)
        fileExplorer_->setStatusLine(ui_->listWidget->selectionInfosFromListWidget());
}

void FileExplorerView::updateViewActions()
{
    emit(viewStateChanged(viewMode(), !isDirectoryTreeCollapsed()));
}

void FileExplorerView::updateDriveActions()
{
    if(fileExplorer_->isActive())
        emit(driveChanged());
}

void FileExplorerView::tableItemSelectionChanged()
{
    updateStatusLine();
}

void FileExplorerView::listItemSelectionChanged()
{
    updateStatusLine();
}

void FileExplorerView::sortFileInfoList()
{
    QString filter="";
    if( fileExplorer_->getQuickFilterState())
        filter=fileExplorer_->getQuickFilterText();

    QHeaderView* pHeader=ui_->tableWidget->horizontalHeader();
    int  sortBySection=pHeader->sortIndicatorSection();
    bool sortAscending=(pHeader->sortIndicatorOrder()==Qt::AscendingOrder) ? true : false;

    fileExplorerTarget_->sortFiles((WinFileInfo::SortOptions) sortBySection, filter, sortAscending);
}

void FileExplorerView::tableHeaderClicked(int collumn)
{
    sortFileInfoList();
    ui_->tableWidget->fillFileTable(fileExplorerTarget_->getFiles(),
                                   fileExplorerTarget_->getSortedFileIndices());
}

void FileExplorerView::listItemDblClicked(QListWidgetItem* item)
{
    // das QFileInfo-Objekt aus dem QVariant holen...
    QVariant variant=item->data(Qt::UserRole);

    if(variant.type()==QVariant::UserType)
    {
        WinFileInfo FileInfo=variant.value<WinFileInfo>();
        if(FileInfo.isDir() && !FileInfo.isArchiveFile())
            handleClickedItem(variant.value<WinFileInfo>());
    }
}

void FileExplorerView::tableItemDblClicked(int row, int collumn)
{
    if(collumn==0) // erste Spalte geklickt...
    {
        QTableWidgetItem* pItem=ui_->tableWidget->item(row, collumn);
        if(pItem)
        {
            // das QFileInfo-Objekt aus dem QVariant holen...
            QVariant variant=pItem->data(Qt::UserRole);
            if(variant.type()==QVariant::UserType)
                handleClickedItem(variant.value<WinFileInfo>());
        }
    }
}

void FileExplorerView::handleClickedItem(WinFileInfo fileInfo)
{
    if(fileInfo.isDir() && !fileInfo.isArchiveFile())
    {
        changeDir(fileInfo.filePath());
    }
    else if(fileInfo.isShellLink())
    {
        changeDir(fileInfo.resolveLnkFile());
    }
    else if(fileInfo.suffix().toUpper()=="7Z" ||
            fileInfo.suffix().toUpper()=="ZIP")
    {
        QString archiveRoot=fileInfo.path()+QString("\\<Archive>")+fileInfo.fileName();
        changeDir(archiveRoot);
    }
    else if(fileInfo.isDir() && fileInfo.isArchiveFile())
    {
        if(fileInfo.fileName()=="..")
            onButtonDirUp(false);
        else
        {
            QString archivePath=fileInfo.path()+QString("<Archive>")+fileInfo.archive()+QString("\\")+fileInfo.fileName();
            changeDir(archivePath);
        }
    }
    else
    {
        fileInfo.executeRegisteredShellAction();
    }
}

void FileExplorerView::networkTreeItemChanged(QTreeWidgetItem* item, int collumn)
{
    if(item)
    {
        // get the QFileInfo-object out of the QVariant...
        QVariant variant=item->data(collumn, UserRole_2);

        if(variant.type()==QVariant::UserType)
        {
            WinFileInfo fileInfo=variant.value<WinFileInfo>();
            if(fileInfo.isDir())
                changeDir(fileInfo.path());
        }
    }
}

void FileExplorerView::filesWereDroppedIn(QTableWidgetItem* item, QList<QUrl> files, Qt::DropAction action)
{
    QString to=getPathLine(); // default destination is the currently shown dir in the active view
    if(item)
    {
        // get the QFileInfo-object out of the QVariant...
        QVariant variant=item->data(Qt::UserRole);
        if(variant.type()==QVariant::UserType)
        {
            WinFileInfo FileInfo=variant.value<WinFileInfo>();
            if(FileInfo.isDir() && FileInfo.fileName()!="..")
            {
                to=FileInfo.filePath(); // if the files were dropped on a directory then this will be the new destination
            }
        }
    }
    processDroppedFiles(files, to, action);
}

void FileExplorerView::filesWereDroppedIn(QListWidgetItem* item, QList<QUrl> files, Qt::DropAction action)
{
    QString to=getPathLine(); // default destination is the currently shown dir in the active view
    if(item)
    {
        // get the QFileInfo-object out of the QVariant...
        QVariant variant=item->data(Qt::UserRole);
        if(variant.type()==QVariant::UserType)
        {
            WinFileInfo FileInfo=variant.value<WinFileInfo>();
            if(FileInfo.isDir() && FileInfo.fileName()!="..")
            {
                to=FileInfo.filePath(); // if the files were dropped on a directory then this will be the new destination
            }
        }
    }
    processDroppedFiles(files, to, action);
}

void FileExplorerView::processDroppedFiles(QList<QUrl> files, QString to, Qt::DropAction action)
{
    QSharedPointer<DropCommand> dropCommand(DropCommand::dropCommandForActualSelection(files, to, action));
    dropCommand->execute();
}

void FileExplorerView::checkDriveList()
{
   // Get actual drive name
    QString strDrive=ui_->lineEditPath->text();
    strDrive=strDrive.left(strDrive.indexOf(":")+1);

    int iIndex=ui_->comboBox->findText(strDrive);
    ui_->comboBox->setCurrentIndex(iIndex);
}

void FileExplorerView::onButtonDirUp(bool state)
{
    QString strPath=ui_->lineEditPath->text();
    int iPos=0;
    if((iPos=strPath.lastIndexOf("\\"))>-1)
    {
        QString strNewPath=strPath.left(iPos);
        if(!state)
            changeDir(WinFileInfo::markArchiveNameInPath(strNewPath));
    }
}

void FileExplorerView::onButtonRootDir()
{
    QString strPath=ui_->lineEditPath->text();

    QRegExp rexLongerPath("^\\\\\\\\\\w+\\\\\\w+\\\\");
    QRegExp rexBasePath("^\\\\\\\\\\w+\\\\\\w+");
    int iPos=0;
    if((iPos = rexLongerPath.indexIn(strPath, iPos))!=-1)
    {        
        strPath=rexLongerPath.capturedTexts().at(0);
        strPath=strPath.left(strPath.length()-1);
        changeDir(strPath);
    }
    else if((iPos = rexBasePath.indexIn(strPath, iPos))!=-1)
    {
        strPath=rexBasePath.capturedTexts().at(0);
        changeDir(strPath);
    }
    else
        changeDir(WinFileInfo::getDriveFromPath(ui_->lineEditPath->text()));
}

void FileExplorerView::onButtonCopyPath2Clipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui_->lineEditPath->text());
}

void FileExplorerView::updateOnQuickFilterTextChanged() const
{
    const_cast<FileExplorerView*>(this)->changeDir(WinFileInfo::markArchiveNameInPath(ui_->lineEditPath->text()));
}

bool FileExplorerView::isDirectoryTreeCollapsed() const
{
    QList<int> listSizes=ui_->splitter->sizes();
    if(listSizes.at(0)==0)
        return true;
    return false;
}

void FileExplorerView::collapseDirectoryTree(bool collapse) const
{
    QList<int> listSizes=ui_->splitter->sizes();
    int iSplitterSize=listSizes.at(0)+listSizes.at(1);
    int iTreeWidth=0;
    int iTableWidth=0;

    if(collapse) // tree is visible -> collapse it
    {
        iTableWidth=iSplitterSize;
    }
    else         // collapsed tree -> expand it
    {
        iTreeWidth=iSplitterSize/3;
        iTableWidth=iSplitterSize-iTreeWidth;
    }

    QList<int> listNewSizes;
    listNewSizes.push_back(iTreeWidth);
    listNewSizes.push_back(iTableWidth);
    ui_->splitter->setSizes(listNewSizes);
}

void FileExplorerView::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);
    if(e->button()==Qt::LeftButton)
    {
        fileExplorer_->setActive(true);
    }
}

void FileExplorerView::checkActiveState() const
{    
    if(isActive())
    {
        ui_->lineEditPath->setStyleSheet("QLineEdit {color: white; background: rgb(132,150,255);}");
    }
    else
    {
        ui_->lineEditPath->setStyleSheet("QLineEdit {color: black; background: white;}");
    }
}

void FileExplorerView::emitDriveState() const
{
   emit(driveChanged());
}

void FileExplorerView::emitHistoryState() const
{
    emit(historyStateChanged(this, fileExplorerHistory_->getActualIndex(), fileExplorerHistory_->getHistoryCount()));

}

void FileExplorerView::emitViewState() const
{
    focusActualView();
    emit(viewStateChanged(viewMode(), !isDirectoryTreeCollapsed()));
}

void FileExplorerView::focusActualView() const
{
    if(isActive())
    {
        if(viewMode_==DETAILS)
            ui_->tableWidget->setFocus();
        else
            ui_->listWidget->setFocus();
    }
}

void FileExplorerView::setDetailsMode() const
{    
    ui_->listWidget->hide();
    const_cast<FileExplorerView*>(this)->ui_->tableWidget->fillFileTable(fileExplorerTarget_->getFiles(),
                                                                        fileExplorerTarget_->getSortedFileIndices());
    const_cast<FileExplorerView*>(this)->viewMode_=DETAILS;
    ui_->tableWidget->show();
    // Update the view actions
    emitViewState();
}

void FileExplorerView::setListMode() const
{
    ui_->tableWidget->hide();
    ui_->listWidget->setViewMode(QListView::ListMode);
    ui_->listWidget->setIconSize(QSize(16,16));
    ui_->listWidget->setGridSize(QSize(-1,-1));

    const_cast<FileExplorerView*>(this)->ui_->listWidget->fillFileList(fileExplorerTarget_->getFiles(),
                                                                      fileExplorerTarget_->getSortedFileIndices(),
                                                                      viewMode_);
    const_cast<FileExplorerView*>(this)->viewMode_=LIST;
    ui_->listWidget->show();
    // Update the view actions
    emitViewState();
}

void FileExplorerView::setIconMode(bool bBigIcons) const
{
    ui_->tableWidget->hide();
    ui_->listWidget->setViewMode(QListView::IconMode);
    if(bBigIcons)
    {
        const_cast<FileExplorerView*>(this)->viewMode_=BIGICONS;
        ui_->listWidget->setIconSize(QSize(48,48));
        ui_->listWidget->setGridSize(QSize(64,64));
        ui_->listWidget->setResizeMode(QListView::Adjust);
    }
    else
    {
        const_cast<FileExplorerView*>(this)->viewMode_=SMALLICONS;
        ui_->listWidget->setIconSize(QSize(24,24));
        ui_->listWidget->setGridSize(QSize(48,48));
        ui_->listWidget->setResizeMode(QListView::Adjust);
    }

    const_cast<FileExplorerView*>(this)->ui_->listWidget->fillFileList(fileExplorerTarget_->getFiles(),
                                                                      fileExplorerTarget_->getSortedFileIndices(),
                                                                      viewMode_);
    ui_->listWidget->show();
    // Update the view actions
    emitViewState();
}

ListViewMode FileExplorerView::viewMode() const
{
    return viewMode_;
}

void FileExplorerView::setViewMode(ListViewMode mode) const
{
    switch(mode)
    {
        case LIST:
            setListMode();
            break;
        case BIGICONS:
            setIconMode(true);
            break;
        case SMALLICONS:
            setIconMode(false);
            break;
        case DETAILS:
        default:
            setDetailsMode();
    }
}

WinFileInfoList FileExplorerView::getSelectedFiles() const
{
    return ui_->tableWidget->getSelectedFiles();
}

QString FileExplorerView::getPathLine() const
{
    return ui_->lineEditPath->text();
}

void FileExplorerView::refresh() const
{
    const_cast<FileExplorerView*>(this)->changeDirNoHistory(ui_->lineEditPath->text());
}

void FileExplorerView::historyBackward()
{
    changeDirNoHistory(fileExplorerHistory_->back());
}

void FileExplorerView::historyForward()
{
    changeDirNoHistory(fileExplorerHistory_->forward());
}

void FileExplorerView::setFileExplorer(FileExplorer* fileExplorer)
{
    fileExplorer_=fileExplorer;
    if(fileExplorer_->isActive())
    {
        ui_->lineEditPath->setStyleSheet("QLineEdit {background: rgb(139,161,105);}");
    }
    else
    {
        ui_->lineEditPath->setStyleSheet("QLineEdit {background: white;}");
    }
}
