/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** Command for checking for Qmmander updates.
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
#ifndef UPDATEQMMANDERCOMMAND_H
#define UPDATEQMMANDERCOMMAND_H

#include "command.h"
#include <QUrl>
#include <QObject>

class QNetworkReply;
class QIODevice;

class UpdateQmmanderCommand : public QObject,  public Command
{
    Q_OBJECT
public:
    virtual ~UpdateQmmanderCommand();
    virtual void execute();

    static UpdateQmmanderCommand* getUpdateCommand();

signals:
    void updateQmmanderCommandFinished(UpdateQmmanderCommand* deleteMe);

protected:
    UpdateQmmanderCommand(){};

private slots:
    void check4Updates(QNetworkReply* reply);

private:
    QUrl getUpdateUrl();
    void setSettingsCheck4Update(bool value);
    bool getSettingsCheck4Update();
    void processResult(QIODevice *source);        
};

#endif // UPDATEQMMANDERCOMMAND_H
