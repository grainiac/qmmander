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
#include "filetablewidget.h"
#include "FileExplorerView.h"

#include <QKeyEvent>
#include <QFileIconProvider>
#include <QScrollBar>
#include <QHeaderView>
#include <QApplication>

#include "mainwindow.h"

FileTableWidget::FileTableWidget(QWidget* parent)
:   QTableWidget(parent),
    m_alreadyShown(false)
{     
    connect(horizontalHeader(), SIGNAL(sectionResized(int,int,int)), this, SLOT(headerSectionResized(int,int,int)));
}

void FileTableWidget::headerSectionResized(int logicalIndex, int oldSize, int newSize)
{
    saveCollumnWidth(logicalIndex, newSize);
    horizontalHeader()->resizeSection(logicalIndex, newSize);
}

void FileTableWidget::setParentFileExplorerView(const FileExplorerView* view)
{
    m_parentView=view;
}

void FileTableWidget::keyPressEvent(QKeyEvent* e)
{
    QTableWidget::keyPressEvent(e);
    if(e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return)
        emit cellDoubleClicked(currentRow(),0);
}

void FileTableWidget::paintEvent (QPaintEvent* e)
{
    QTableWidget::paintEvent(e);
    if(!m_alreadyShown)
    {
        m_alreadyShown=true;
        for(int i=0; i<columnCount(); i++)
            horizontalHeader()->resizeSection(i, getCollumnWidth(i));
    }
}

void FileTableWidget::saveCollumnWidth(int collumn, int width)
{    
    QString registryKey;
    MainWindow* main=getMainWindow();
    if(main && m_parentView)
    {
        if( main->isLeftExplorer(m_parentView->getFileExplorer()) )
            registryKey="left_explorer/collumn_width_";
        else
            registryKey="right_explorer/collumn_width_";

        QSettings settings("ASKSoft", "Qmmander");
        int oldWidth=settings.value(QString("%1%2").arg(registryKey).arg(collumn), 75).toInt();

        if(width!=oldWidth)
            settings.setValue(QString("%1%2").arg(registryKey).arg(collumn), width);
    }    
}

int FileTableWidget::getCollumnWidth(int collumn)
{
    QString registryKey;
    MainWindow* main=getMainWindow();
    if(main && m_parentView)
    {
        if( main->isLeftExplorer(m_parentView->getFileExplorer()) )
            registryKey="left_explorer/collumn_width_";
        else
            registryKey="right_explorer/collumn_width_";

        QSettings settings("ASKSoft", "Qmmander");
        return settings.value(QString("%1%2").arg(registryKey).arg(collumn), 75).toInt();
    }
    return 75;
}

MainWindow* FileTableWidget::getMainWindow()
{
    MainWindow* main=0;
    foreach(QWidget *widget, QApplication::allWidgets())
    {
        main=dynamic_cast<MainWindow*>(widget);
        if(main)
            return main;
    }
    return main;
}

void FileTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        setStyleSheet("QTableView {background-color: rgb(224,255,224);}");
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void FileTableWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    setStyleSheet("QTableView {background-color: rgb(255,255,255);}");    
    event->accept();
}

void FileTableWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void FileTableWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        QTableWidgetItem* pItem=itemAt(event->pos());
        if(pItem)
        {
            if(pItem!=m_itemFrom)
            {
                QList<QUrl> listURLs = event->mimeData()->urls();
                emit(filesDropped(pItem, listURLs, event->dropAction()));
                event->acceptProposedAction();
            }
            else
                event->ignore();
        }
        else
        {
            QList<QUrl> listURLs = event->mimeData()->urls();
            emit(filesDropped(NULL, listURLs, event->dropAction()));
            event->acceptProposedAction();
        }
    }
    else
    {
        event->ignore();
    }
    setStyleSheet("QTableView {background-color: rgb(255,255,255);}");
}

void FileTableWidget::mousePressEvent(QMouseEvent *event)
{    
    QTableWidget::mousePressEvent(event);

    m_itemFrom=itemAt(event->pos());

    WinFileInfoList listSelected=getSelectedFiles();
    if(listSelected.count()>0)
    {
        QMimeData *mimeData = new QMimeData;        
        mimeData->setUrls(getURLList(listSelected));

        QSharedPointer<QFileIconProvider> iconProvider(new QFileIconProvider);
        QIcon icon;
        if(listSelected.count()==1)
            if(listSelected.at(0).isDir())
                icon=iconProvider->icon(QFileIconProvider::Folder);
        else
            icon=listSelected.at(0).getWinIcon();
        else
            icon=iconProvider->icon(QFileIconProvider::File);

        QPixmap pixmap(icon.pixmap(32,32));
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixmap);

        drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
    }
}

WinFileInfoList FileTableWidget::getSelectedFiles()
{
    QList<QTableWidgetItem *> itemList=selectedItems();
    WinFileInfoList listFileInfos;
    for(int i=0; i<itemList.count(); i++)
    {
        QTableWidgetItem* item=itemList.at(i);
        if(item->column()==0 && item->text()!="..")
        {
            // Get the QFileInfo-Object out of the QVariant...
            QVariant variant=item->data(Qt::UserRole);

            if(variant.type()==QVariant::UserType)
            {
                WinFileInfo fileInfo=variant.value<WinFileInfo>();
                //if(fileInfo.fileName()!="." && fileInfo.fileName()!="..")
                    listFileInfos.push_back(fileInfo);
            }
        }
    }
    return listFileInfos;
}

QList<QUrl> FileTableWidget::getURLList(WinFileInfoList list)
{
    QList<QUrl> listURls;

    for(int i=0; i<list.count(); i++)
        listURls.push_back(QUrl("/"+list.at(i).filePath()));

    return listURls;
}

QString FileTableWidget::selectionInfosFromTableWidget()
{
    quint64 itemcount=0;       // item count in dir
    quint64 itemsSizeInByte=0; // size of all items

    QString freeSpaceOnDrive="";
    for(int i=0; i<rowCount(); i++)
    {
        QTableWidgetItem* tableItem=item(i,0);
        if(tableItem && tableItem->text()!="..") // skip ".." entry
        {
            QVariant variant=tableItem->data(Qt::UserRole);
            if(variant.type()==QVariant::UserType)
            {
                WinFileInfo fi=variant.value<WinFileInfo>();
                if(!fi.isDir())
                    itemsSizeInByte+=fi.size();
                if(freeSpaceOnDrive=="")
                    freeSpaceOnDrive=WinFileInfo::getFreeSpaceOnDrive(fi.path());
                itemcount++;
            }
        }
    }

    QString combinedStatusText;
    if( selectedItems().count()==0 ||
       (selectedItems().count()==1 && selectedItems().at(0)->text()=="..") )
    {
        QString fileSize=QString("%1 KB").arg(qRound(static_cast<qreal>(itemsSizeInByte)/1024+.5));
        combinedStatusText=QString("%1 %2 %3 Byte(s) (%4 %5)").arg( itemcount )\
                                                              .arg(tr("Object(s)"))\
                                                              .arg( fileSize)\
                                                              .arg(tr("Free"))\
                                                              .arg( freeSpaceOnDrive );
    }
    else
    {
        QList<QTableWidgetItem*> listSelected=selectedItems();
        QList<QTableWidgetItem*> listCol0Items;
        for(int i=0; i<listSelected.count(); i++) // first collumn elements which contains the FileInfo-Objekt
            if(listSelected.at(i)->column()==0 && listSelected.at(i)->text()!="..")
                listCol0Items.push_back(listSelected.at(i));

        if(listCol0Items.count()==1)
        {
            QVariant variant=listCol0Items.at(0)->data(Qt::UserRole);
            WinFileInfo fi=variant.value<WinFileInfo>();
            QDateTime d;
            if(fi.isDir())
                d.setTime_t(fi.creationTime());
            else
                d.setTime_t(fi.lastModificationTime());
            QString date=d.toString("dd.MM.yyyy hh:mm:ss");
            QString fileSize=QString("%1 KB").arg(qRound64(fi.size()/1024+.5));
            QString attributes=WinFileInfo::getAttributeString(fi);

            QString detailsOnFile;
            if(fi.isDir())
            {
                detailsOnFile=date+" "+     // Change date
                              attributes;   // Attributes
            }
            else
            {
                detailsOnFile=fileSize+" "+  // Size
                              date+" "+      // Change date
                              attributes;    // Attributes
            }
            combinedStatusText=tr("1/%1 %2 %3 (%4 %5)").arg( itemcount )\
                                                       .arg( tr("Object(s) selected") )\
                                                       .arg( detailsOnFile )\
                                                       .arg( tr("Free"))\
                                                       .arg( freeSpaceOnDrive );

        }
        else
        {
            quint64 fileLength=0;
            for(int j=0; j<listCol0Items.count(); j++)
            {
                // get WinFileInfo-Object from the QVariant stored in table-element...
                QVariant variant=listCol0Items.at(j)->data(Qt::UserRole);
                WinFileInfo FileInfo=variant.value<WinFileInfo>();
                if(FileInfo.isFile()==true)
                    fileLength+=FileInfo.size();
            }

            QString fileSize;
            if(fileLength>0)
            {
                fileSize=QString("%1 KB").arg(fileLength/1024+.5);
                combinedStatusText=QString("%1/%2 %3 %4 (%5 %6)").arg( listCol0Items.count() )\
                                                                 .arg( itemcount )\
                                                                 .arg( tr("Object(s) selected") )\
                                                                 .arg( fileSize )\
                                                                 .arg( tr("Free"))\
                                                                 .arg( freeSpaceOnDrive );
            }
            else
            {
                combinedStatusText=QString("%1/%2 %3 (%4 %5)").arg( listCol0Items.count() )\
                                                              .arg( itemcount )\
                                                              .arg( tr("Object(s) selected") )\
                                                              .arg( tr("Free"))\
                                                              .arg( freeSpaceOnDrive );
            }
        }
    }
    return combinedStatusText;
}

void FileTableWidget::fillFileTable(const WinFileInfoPtrList& files, const WinFileInfoIndices& visibleIndices)
{
    clearContents();

    // Icons über QFileIconProvider besorgen....
    QSharedPointer<QFileIconProvider> iconProvider(new QFileIconProvider);

    TableItemPrototypes* tableItemPrototypes=MainWindow::getTableItemPrototypes();
    FileTypeIcons*       fileTypeIcons=MainWindow::getFileTypeIcons();

    quint64 insertCount=0;

    setRowCount(visibleIndices.size());

    for(WinFileInfoIndices::const_iterator it=visibleIndices.begin(); it!=visibleIndices.end(); it++)
    {
        WinFileInfo* fi=files.at(*it);
        if(fi->fileName()==QString("."))
            continue;

        QDateTime d;
        if(fi->isDir())
            d.setTime_t(fi->creationTime());
        else
            d.setTime_t(fi->lastModificationTime());
        QString date=d.toString("dd.MM.yyyy hh:mm:ss");

        QString fileSize=QString("%1 KB").arg(qRound64(fi->size()/1024+.5));

        // Every FileType-Description will be cached in a static map of MainWindow, so
        // all Explorers/Views can share these informations and don't need to fetch
        // them again. The same is done for the icons of a filetype. Speedup with caching
        // was up to factor 7!!! C:\Windows\SysWOW64 takes now approx. 1,2sec to display
        // (before up to 8,5sec on an C2Q 6600 system).
        //
        QString fileTypeDescription=WinFileInfo::getFileTypeDescripton(*fi);
        if(!tableItemPrototypes->contains(fileTypeDescription))
            (*tableItemPrototypes)[fileTypeDescription]=QTableWidgetItem(fileTypeDescription);

        if(!fileTypeIcons->contains(fileTypeDescription) && fi->isDir() && !fi->isSymLink())
            (*fileTypeIcons)[fileTypeDescription]=iconProvider->icon(QFileIconProvider::Folder);

        if(!fileTypeIcons->contains(fileTypeDescription) && !fi->isDir())
            (*fileTypeIcons)[fileTypeDescription]=fi->getWinIcon();

        QString attributes=WinFileInfo::getAttributeString(*fi);
        if(!tableItemPrototypes->contains(attributes))
        {
            (*tableItemPrototypes)[attributes]=QTableWidgetItem(attributes);
            (*tableItemPrototypes)[attributes].setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        }

        // Under the first collumn of a row goes a user-defined QVariant which holds the
        // QFileInfo-Object for the entry created from QDir.
        QTableWidgetItem* item=0;
        if(fi->isDir())
        {
            if(fi->isSymLink())
            {
                item=new QTableWidgetItem((*fileTypeIcons)["ICON_FOLDER_WITH_LINK16"], fi->fileName());
            }
            else
            {
                if(fi->isArchiveFile())
                    item=new QTableWidgetItem((*fileTypeIcons)[fileTypeDescription], fi->archiveFileName());
                else
                    item=new QTableWidgetItem((*fileTypeIcons)[fileTypeDescription], fi->fileName());
            }
            item->setToolTip(tr("Creation date: ")+date);
        }
        else
        {
            if(fi->isArchiveFile())
                item=new QTableWidgetItem((*fileTypeIcons)[fileTypeDescription], fi->archiveFileName());
            else
                item=new QTableWidgetItem((*fileTypeIcons)[fileTypeDescription], fi->fileName());

            fileTypeDescription=WinFileInfo::getFileTypeDescripton(*fi);
            item->setToolTip(tr("Type")+": "+fileTypeDescription+"\n"+
                              tr("Size")+": "+fileSize+"\n"+
                              tr("Last modified")+": "+date);
        }
        item->setData(Qt::UserRole, qVariantFromValue(*fi) );
        setItem(insertCount, 0, item);                                               // Col 1

        if(!fi->isDir())
        {
            item=(*tableItemPrototypes)["__EMPTY_ITEM_RA__"].clone();
            item->setText(fileSize);
        }
        else
            item=(*tableItemPrototypes)["__EMPTY_ITEM_RA__"].clone();

        setItem(insertCount, 1, item);                                               // Col 2

        if(!fi->isArchiveFile() || (fi->isArchiveFile() && !fi->isDir()))                               // Col 3
        {
            setItem(insertCount, 2, new QTableWidgetItem(date));
        }
        else
        {
            setItem(insertCount, 2, new QTableWidgetItem(""));
        }

        setItem(insertCount, 3, (*tableItemPrototypes)[fileTypeDescription].clone());    // Col 4

        if(attributes=="")
            item=(*tableItemPrototypes)["__EMPTY_ITEM_LA__"].clone();
        else
            item=(*tableItemPrototypes)[attributes].clone();
        setItem(insertCount, 4, item);               // Col 5

        insertCount++;
    }

    setRowCount(insertCount);
    verticalScrollBar()->setSliderPosition(0);
}
