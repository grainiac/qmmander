/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Internals implementation.
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
#include "internals.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtWidgets/QFileSystemModel>
#include <QNetworkAccessManager>

qmndr::Internals* qmndr::Internals::internals = nullptr;

qmndr::Internals::Internals()
:   pMainWindow_(nullptr)
{
    pTableItemPrototypes_ = std::unique_ptr<TableItemPrototypes>( new TableItemPrototypes() );
    pFileTypeIcons_ = std::unique_ptr<FileTypeIcons>( new FileTypeIcons() );
    addEmptyItems2TableWidgetPrototypes();
    initializeIconCacheMap();
}

qmndr::Internals& qmndr::Internals::instance()
{
    if(!internals)
        internals = new Internals();

    return *internals;
}

QSettings& qmndr::Internals::settings()
{
    if(pSettings_.isNull())
        pSettings_ = new QSettings(QApplication::applicationDirPath()+"qmmander_settings.ini", QSettings::IniFormat);

    return *pSettings_.data();
}

QFileSystemModel& qmndr::Internals::fileSystemModel()
{
    if(pFileSystemModel_.isNull())
    {
        pFileSystemModel_= new QFileSystemModel();
        pFileSystemModel_->setResolveSymlinks(true);
        QDir::Filters filters=QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System;
        pFileSystemModel_->setFilter(filters);
    }
    return *pFileSystemModel_.data();
}

QNetworkAccessManager& qmndr::Internals::networkAccessManager()
{
    if(pNetworkAccessManager_.isNull())
        pNetworkAccessManager_ = new QNetworkAccessManager();

    return *pNetworkAccessManager_;
}

FileTypeIcons &qmndr::Internals::fileTypeIcons()
{
    return *pFileTypeIcons_.get();
}

TableItemPrototypes &qmndr::Internals::tableItemPrototypes()
{
    return *pTableItemPrototypes_.get();
}

MainWindow& qmndr::Internals::mainWindow()
{
    return *pMainWindow_;
}

void qmndr::Internals::setMainWindow(MainWindow *pMainWindow)
{
    pMainWindow_ = pMainWindow;
}

void qmndr::Internals::addEmptyItems2TableWidgetPrototypes()
{
    pTableItemPrototypes_->insert("__EMPTY_ITEM_LA__", QTableWidgetItem(""));
    (*pTableItemPrototypes_)["__EMPTY_ITEM_LA__"].setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    pTableItemPrototypes_->insert("__EMPTY_ITEM_RA__", QTableWidgetItem(""));
    (*pTableItemPrototypes_)["__EMPTY_ITEM_RA__"].setTextAlignment(Qt::AlignVCenter | Qt::AlignRight);
}

void qmndr::Internals::initializeIconCacheMap()
{
    // Load folder icons
    (*pFileTypeIcons_)["ICON_FOLDER_WITH_LINK16"] = QIcon(QPixmap(":/images/images/folder_link16.png"));
    (*pFileTypeIcons_)["ICON_FOLDER_WITH_LINK16"] = QIcon(QPixmap(":/images/images/folder_link16.png"));
    (*pFileTypeIcons_)["ICON_FOLDER_WITH_LINK32"] = QIcon(QPixmap(":/images/images/folder_link32.png"));

    // Load drive icons
    (*pFileTypeIcons_)["DRIVE_REMOVABLE_FDD"] = QIcon(QPixmap(":/images/images/drivefdd16.png"));
    (*pFileTypeIcons_)["DRIVE_REMOVABLE_STICK"] = QIcon(QPixmap(":/images/images/drivestick16.png"));
    (*pFileTypeIcons_)["DRIVE_FIXED"] = QIcon(QPixmap(":/images/images/drivehdd16.png"));
    (*pFileTypeIcons_)["DRIVE_FIXED_SYS"] = QIcon(QPixmap(":/images/images/drivehddsys16.png"));
    (*pFileTypeIcons_)["DRIVE_REMOTE"] = QIcon(QPixmap(":/images/images/drivenet16.png"));
    (*pFileTypeIcons_)["DRIVE_CDROM"] = QIcon(QPixmap(":/images/images/driveoptical16.png"));

    (*pFileTypeIcons_)["DRIVE_REMOVABLE_FDD48"] = QIcon(QPixmap(":/images/images/drivefdd48.png"));
    (*pFileTypeIcons_)["DRIVE_REMOVABLE_STICK48"] = QIcon(QPixmap(":/images/images/drivestick48.png"));
    (*pFileTypeIcons_)["DRIVE_FIXED48"] = QIcon(QPixmap(":/images/images/drivehdd48.png"));
    (*pFileTypeIcons_)["DRIVE_FIXED_SYS48"] = QIcon(QPixmap(":/images/images/drivehddsys48.png"));
    (*pFileTypeIcons_)["DRIVE_REMOTE48"] = QIcon(QPixmap(":/images/images/drivenet48.png"));
    (*pFileTypeIcons_)["DRIVE_CDROM48"] = QIcon(QPixmap(":/images/images/drivedvd48.png"));
}
