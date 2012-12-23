/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#include "FileExplorer.h"
#include "ui_FileExplorer.h"
#include "FileExplorerView.h"
#include "mainwindow.h"
#include <windows.h>

#include <QtWidgets/QDirModel>
#include <QtWidgets/QMessageBox>

FileExplorer::FileExplorer(QWidget *parent)
:   QWidget(parent),
    ui_(new Ui::FileExplorer),
    isActive_(false)
{
    ui_->setupUi(this);

    FileExplorerView* view=new FileExplorerView(*this);
    view->setAttribute(Qt::WA_DeleteOnClose);
    ui_->tabWidgetFiles->addTab(view, "C:");

    // Notify the MainWindow when the active explorer has changed
    connect(this, SIGNAL(fileExplorerActivated(FileExplorer*)), MainWindow::getMainWindow(), SLOT(activeExplorerChanged(FileExplorer*)));

    // Connect TabWidget with the FileExplorer to get notified when the tab index has changed
    connect(ui_->tabWidgetFiles, SIGNAL(currentChanged(int)), this, SLOT(currentTabIndexChanged(int)));

    // Add connections for the QuickFilter-functions
    connect(ui_->lineEditQuickFilter, SIGNAL(textChanged(QString)), this, SLOT(onQuickFilterTextChanged(QString)));
    connect(ui_->pushButtonQuickFilter, SIGNAL(toggled(bool)), this, SLOT(onButtonQuickFilterToggled(bool)));

    MainWindow::getMainWindow()->setSplashScreenMessage(tr("Updating files in default view..."));
    // The explorer should view "C:" at startup
    view->changeDir("C:");
}

FileExplorer::~FileExplorer()
{
    delete ui_;
}

const FileExplorerView* FileExplorer::getActiveView() const
{
    return dynamic_cast<FileExplorerView*>(ui_->tabWidgetFiles->currentWidget());
}

void FileExplorer::setActive(bool isActive)
{        
    isActive_=isActive;
    getActiveView()->checkActiveState();
    if(isActive)
    {
        emit(fileExplorerActivated(this));        
        getActiveView()->emitDriveState();
        getActiveView()->emitHistoryState();
        getActiveView()->emitViewState();
    }
}

void FileExplorer::emitFileExplorerActivated()
{
    emit(fileExplorerActivated(this));
}

bool FileExplorer::isActive()
{
    return isActive_;
}

void FileExplorer::currentTabIndexChanged(int index)
{
    FileExplorerView* view=dynamic_cast<FileExplorerView*>(ui_->tabWidgetFiles->widget(index));
    if(view)
    {
        view->checkActiveState();
        view->emitDriveState();
        view->emitHistoryState();
        view->emitViewState();
    }
}

void FileExplorer::changeEvent(QEvent *e)
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

void FileExplorer::onQuickFilterTextChanged(QString text)
{
    if(text.length()>0)
    {
        ui_->pushButtonQuickFilter->setChecked(true);
        getActiveView()->updateOnQuickFilterTextChanged();
    }
    else
    {
        ui_->pushButtonQuickFilter->setChecked(false);
        getActiveView()->updateOnQuickFilterTextChanged();
    }
}

void FileExplorer::onButtonQuickFilterToggled(bool checked)
{
    if(ui_->lineEditQuickFilter->text().length()>0)
        getActiveView()->updateOnQuickFilterTextChanged();
}

void FileExplorer::setStatusLine(QString text)
{
    ui_->labelStatus->setText(text);
}

QString FileExplorer::getQuickFilterText() const
{
    return ui_->lineEditQuickFilter->text();
}

bool FileExplorer::getQuickFilterState() const
{
    return ui_->pushButtonQuickFilter->isChecked();
}

void FileExplorer::setTabTitle(QString text)
{
    int index=ui_->tabWidgetFiles->indexOf(ui_->tabWidgetFiles->currentWidget());
    ui_->tabWidgetFiles->setTabText(index, text);
}

void FileExplorer::showDirectoryTreeAction()
{
    const FileExplorerView* view=getActiveView();
    view->collapseDirectoryTree(!view->isDirectoryTreeCollapsed());
}

WinFileInfoList FileExplorer::getSelectedFiles() const
{    
    return getActiveView()->getSelectedFiles();
}

QString FileExplorer::getSelectedPath() const
{
    return getActiveView()->getPathLine();
}

void FileExplorer::refresh() const
{
    getActiveView()->refresh();
}

QString FileExplorer::swapViews(QString pathLineOfViewIn)
{
    QString pathOut=getActiveView()->getPathLine();
    const_cast<FileExplorerView*>(getActiveView())->changeDir(pathLineOfViewIn);
    return pathOut;
}

void FileExplorer::copyTab(FileExplorerView* source)
{
    // get path
    QString newPath=source->getPathLine(); // New tab gets the same directory as the actual one
    FileExplorerView* view=new FileExplorerView(*getActiveView());
    view->setAttribute(Qt::WA_DeleteOnClose);
    ui_->tabWidgetFiles->addTab(view, "");
    ui_->tabWidgetFiles->setCurrentWidget(view);
    ui_->tabWidgetFiles->repaint();
    view->setViewMode(source->viewMode());
    view->changeDir(WinFileInfo::markArchiveNameInPath(newPath));
    view->collapseDirectoryTree();
}

void FileExplorer::closeTab()
{
    if(ui_->tabWidgetFiles->count()>1) // there's always one tab open in an explorer
    {
        FileExplorerView* view=const_cast<FileExplorerView*>(getActiveView()); // delete FileExplorerView from list
        int iTabIndex=ui_->tabWidgetFiles->indexOf(view); // now delete it from the TabWidget
        ui_->tabWidgetFiles->removeTab(iTabIndex);
        int iNewIndex= (iTabIndex-1>0) ? iTabIndex-1 : 0; // Select the new active tab whith previous index
        ui_->tabWidgetFiles->setCurrentIndex(iNewIndex);
    }
}

void FileExplorer::closeTabButtonPressed(int tabIndex2Close)
{
    if(ui_->tabWidgetFiles->count()>1) // there's always one tab open in an explorer
    {        
        ui_->tabWidgetFiles->removeTab(tabIndex2Close);
        int iNewIndex= (tabIndex2Close-1>0) ? tabIndex2Close-1 : 0; // Select the new active tab whith previous index
        ui_->tabWidgetFiles->setCurrentIndex(iNewIndex);
    }
}

void FileExplorer::newTab()
{       
    // Extract tab-title
    QString newPath=getActiveView()->getPathLine(); // New tab gets the same directory as the actual one
    FileExplorerView* view=new FileExplorerView(*getActiveView());
    view->setAttribute(Qt::WA_DeleteOnClose);
    ui_->tabWidgetFiles->addTab(view, "");
    ui_->tabWidgetFiles->setCurrentWidget(view);
    view->changeDir(WinFileInfo::markArchiveNameInPath(newPath));
    view->collapseDirectoryTree();

}

void FileExplorer::setNextTabActive()
{
    int index=ui_->tabWidgetFiles->currentIndex();
    if(index<=ui_->tabWidgetFiles->count()-2)
        ui_->tabWidgetFiles->setCurrentIndex(index+1);
    else
        ui_->tabWidgetFiles->setCurrentIndex(0);
}

void FileExplorer::setPreviousTabActive()
{
    int index=ui_->tabWidgetFiles->currentIndex();
    if(index>0)
        ui_->tabWidgetFiles->setCurrentIndex(index-1);
    else
        ui_->tabWidgetFiles->setCurrentIndex(ui_->tabWidgetFiles->count()-1);
}
