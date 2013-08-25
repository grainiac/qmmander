/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Internals is a factory that holds all internal and shared parts of Qmmander.
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
#ifndef INTERNALS_H
#define INTERNALS_H

#include <QPointer>
#include <QSettings>
#include <memory>

class QFileSystemModel;
class QNetworkAccessManager;
class QTableWidgetItem;
class MainWindow;

typedef QMap<QString, QTableWidgetItem> TableItemPrototypes;
typedef QMap<QString, QIcon>            FileTypeIcons;

namespace qmndr {

class Internals
{
public:

    static Internals& instance();

    QSettings&              settings();
    QFileSystemModel&       fileSystemModel();
    QNetworkAccessManager&  networkAccessManager();
    FileTypeIcons&          fileTypeIcons();
    TableItemPrototypes&    tableItemPrototypes();
    MainWindow&             mainWindow();

    void setMainWindow(MainWindow* pMainWindow);


private:
    Internals();
    void addEmptyItems2TableWidgetPrototypes();
    void initializeIconCacheMap();

    static Internals* internals;

    QPointer<QSettings>                    pSettings_;
    QPointer<QFileSystemModel>             pFileSystemModel_;
    QPointer<QNetworkAccessManager>        pNetworkAccessManager_;
    std::unique_ptr<TableItemPrototypes>   pTableItemPrototypes_;
    std::unique_ptr<FileTypeIcons>         pFileTypeIcons_;
    MainWindow*                            pMainWindow_;
};

}   // namespace qmndr

#endif // INTERNALS_H
