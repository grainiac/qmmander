/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
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
#include "filelistwidget.h"
#include "internals.h"
#include <QKeyEvent>
#include <QFileIconProvider>
#include <QtGui>

FileListWidget::FileListWidget(QWidget* pParent)
:   QListWidget(pParent)
{
}

void FileListWidget::keyPressEvent(QKeyEvent* e)
{
    QListWidget::keyPressEvent(e);
    if(e->key()==Qt::Key_Enter || e->key()==Qt::Key_Return)
        emit itemDoubleClicked(currentItem());
}

void FileListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {
        setStyleSheet("QListView {background-color: rgb(224,255,224);}");
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void FileListWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    setStyleSheet("QListView {background-color: rgb(255,255,255);}");
    event->accept();
}

void FileListWidget::dragMoveEvent(QDragMoveEvent *event)
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

void FileListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
    {        
        QListWidgetItem* item=itemAt(event->pos());
        if(item && item!=itemFrom_)
        {
            if(item!=itemFrom_)
            {
                QList<QUrl> listURLs = event->mimeData()->urls();
                emit(filesDropped(item, listURLs, event->dropAction()));
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
    setStyleSheet("QListView {background-color: rgb(255,255,255);}");
}

void FileListWidget::mousePressEvent(QMouseEvent *event)
{
    QListWidget::mousePressEvent(event);

    itemFrom_=itemAt(event->pos());
    WinFileInfoList listSelected=getSelectedFiles();
    if(listSelected.count()==0)
        return;

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

WinFileInfoList FileListWidget::getSelectedFiles()
{
    QList<QListWidgetItem *> itemList=selectedItems();
    WinFileInfoList listFileInfos;
    for(int i=0; i<itemList.count(); i++)
    {
        QListWidgetItem* pItem=itemList.at(i);
        if(pItem)
        {
            // Get the QFileInfo-Object out of the QVariant...
            QVariant variant=pItem->data(Qt::UserRole);

            if(variant.type()==QVariant::UserType)
            {
                WinFileInfo fileInfo=variant.value<WinFileInfo>();
                if(fileInfo.fileName()!="." && fileInfo.fileName()!="..")
                    listFileInfos.push_back(fileInfo);
            }
        }
    }
    return listFileInfos;
}

QList<QUrl> FileListWidget::getURLList(WinFileInfoList list)
{
    QList<QUrl> listURls;

    for(int i=0; i<list.count(); i++)
        listURls.push_back(QUrl("/"+list.at(i).filePath()));

    return listURls;
}

QString FileListWidget::selectionInfosFromListWidget()
{
    quint64 itemcount=0;       // item count in dir
    quint64 itemsSizeInByte=0; // size of all items

    QString freeSpaceOnDrive="";
    for(int i=0; i<count(); i++)
    {
        QListWidgetItem* nextItem=item(i);
        if(nextItem->text()!="..") // skip ".." entry
        {
            QVariant variant=nextItem->data(Qt::UserRole);
            if(variant.type()==QVariant::UserType)
            {
                WinFileInfo FileInfo=variant.value<WinFileInfo>();
                if(!FileInfo.isDir())
                    itemsSizeInByte+=FileInfo.size();
                if(freeSpaceOnDrive=="")
                    freeSpaceOnDrive=WinFileInfo::getFreeSpaceOnDrive(FileInfo.path());
                itemcount++;
            }
        }
    }

    QList<QListWidgetItem*> listSelected=selectedItems();

    // remove".."-entry
    QList<QListWidgetItem *> listFound=findItems("..", Qt::MatchExactly);
    if(listFound.count()>0)
        listSelected.removeOne(listFound.at(0));

    QString combinedStatusText;
    if( selectedItems().count()==0 ||
       (selectedItems().count()==1 && selectedItems().at(0)->text()=="..") )
    {
        QString fileSize=QString("%1 KB").arg(qRound(static_cast<qreal>(itemsSizeInByte)/1024+.5));
        combinedStatusText=QString("%1 Objekt(e) %2 Byte(s) (Frei %3)").arg( itemcount )\
                                                                       .arg( fileSize)\
                                                                       .arg( freeSpaceOnDrive );
    }
    else
    {
        if(listSelected.count()==1)
        {
            QVariant variant=listSelected.at(0)->data(Qt::UserRole);
            WinFileInfo FileInfo=variant.value<WinFileInfo>();
            QDateTime d;
            if(FileInfo.isDir())
                d.setTime_t(FileInfo.creationTime());
            else
                d.setTime_t(FileInfo.lastModificationTime());
            QString date=d.toString("dd.MM.yyyy hh:mm:ss");
            QString fileSize=QString("%1 KB").arg(qRound64(FileInfo.size()/1024+.5));
            QString attributes=WinFileInfo::getAttributeString(FileInfo);

            QString detailsOnFile;
            if(FileInfo.isDir())
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
            combinedStatusText=QString("1/%1 Objekt(e) markiert %2 (Frei %3)").arg( itemcount )\
                                                                              .arg( detailsOnFile )\
                                                                              .arg( freeSpaceOnDrive );
        }
        else
        {
            quint64 length=0;
            for(int i=0; i<listSelected.count(); i++)
            {
                // get WinFileInfo-Object from the QVariant stored in list-element...
                QVariant variant=listSelected.at(i)->data(Qt::UserRole);
                WinFileInfo FileInfo=variant.value<WinFileInfo>();
                if(FileInfo.isFile()==true)
                    length+=FileInfo.size();
            }

            QString fileSize;
            if(length>0)
            {
                fileSize=QString("%1 KB").arg(length/1024+.5);
                combinedStatusText=QString("%1/%2 Objekt(e) markiert %3 (Frei %4)").arg( listSelected.count() )\
                                                                                   .arg( itemcount )\
                                                                                   .arg( fileSize )\
                                                                                   .arg( freeSpaceOnDrive );
            }
            else
            {
                combinedStatusText=QString("%1/%2 Objekt(e) markiert (Frei %3)").arg( listSelected.count() )\
                                                                                .arg( itemcount )\
                                                                                .arg( freeSpaceOnDrive );
            }
        }
    }
    return combinedStatusText;
}

void FileListWidget::fillFileList(const WinFileInfoPtrList& files, const WinFileInfoIndices& visibleIndices, ListViewMode mode)
{
    clear();

    // Icons über QFileIconProvider besorgen....
    QSharedPointer<QFileIconProvider> iconProvider(new QFileIconProvider);

    FileTypeIcons& fileTypeIcons = qmndr::Internals::instance().fileTypeIcons();

    quint64 insertCount=0;
    for(WinFileInfoIndices::const_iterator it=visibleIndices.begin(); it!=visibleIndices.end(); it++)
    {
        WinFileInfo* fileInfo=files.at(*it);
        if(fileInfo->fileName()==QString("."))
            continue;

        QDateTime d;
        if(fileInfo->isDir())
            d.setTime_t(fileInfo->creationTime());
        else
            d.setTime_t(fileInfo->lastModificationTime());
        QString date=d.toString("dd.MM.yyyy hh:mm:ss");
        QString fileSize=QString("%1 KB").arg(qRound64(fileInfo->size()/1024+.5));

        // Every FileType-Description will be cached in a static map of MainWindow, so
        // all Explorers/Views can share these informations and don't need to fetch
        // them again. Speedup with caching was up to factor 7!!!
        // C:\Windows\SysWOW64 takes now approx. 1,2sec to display  (before up to 8,5sec
        // on an C2Q 6600 system).
        //
        QString fileTypeDescription=WinFileInfo::getFileTypeDescripton(*fileInfo);
        if(!fileTypeIcons.contains(fileTypeDescription) && fileInfo->isDir() && !fileInfo->isSymLink())
                    fileTypeIcons[fileTypeDescription]=iconProvider->icon(QFileIconProvider::Folder);

        if(!fileTypeIcons.contains(fileTypeDescription) && !fileInfo->isDir())
            fileTypeIcons[fileTypeDescription]=fileInfo->getWinIcon();

        // Under the first collumn of a row goes a user-defined QVariant which holds the
        // QFileInfo-Object for the entry created from QDir.
        QListWidgetItem* item=0;
        if(fileInfo->isDir())
        {
            if(fileInfo->isSymLink())
            {
                if(mode==LIST)
                    item=new QListWidgetItem(fileTypeIcons["ICON_FOLDER_WITH_LINK16"], fileInfo->fileName());
                else
                    item=new QListWidgetItem(fileTypeIcons["ICON_FOLDER_WITH_LINK32"], fileInfo->fileName());
            }
            else
                item=new QListWidgetItem(fileTypeIcons[fileTypeDescription], fileInfo->fileName());
            item->setToolTip(tr("Creation date: ")+date);
        }
        else
        {
            item=new QListWidgetItem(fileTypeIcons[fileTypeDescription], fileInfo->fileName());
            item->setToolTip(tr("Type")+": "+fileTypeDescription+"\n"+
                             tr("Size")+": "+fileSize+"\n"+
                             tr("Last modified")+": "+date);
        }
        item->setData(Qt::UserRole, qVariantFromValue(*fileInfo) );
        addItem(item);

        insertCount++;
    }
}
