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
#include "updateqmmandercommand.h"
#include "mainwindow.h"
#include "FileExplorer.h"
#include "updatedialog.h"
#include "version.h"
#include "internals.h"
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QIODevice>

UpdateQmmanderCommand::~UpdateQmmanderCommand()
{
}

void UpdateQmmanderCommand::execute()
{
    QNetworkAccessManager& networkAccessManager = qmndr::Internals::instance().networkAccessManager();
    connect(&networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(check4Updates(QNetworkReply*)));
    networkAccessManager.get(QNetworkRequest(getUpdateUrl()));
}

QUrl UpdateQmmanderCommand::getUpdateUrl()
{
    QSettings& settings = qmndr::Internals::instance().settings();
    return settings.value("update_server", QUrl("http://qmmander.googlecode.com/files/QmmanderUpdate.xml")).toUrl();
}

void UpdateQmmanderCommand::setSettingsCheck4Update(bool value)
{
    QSettings& settings = qmndr::Internals::instance().settings();
    settings.setValue("check_4_updates", value);
}

bool UpdateQmmanderCommand::getSettingsCheck4Update()
{    
    QSettings& settings = qmndr::Internals::instance().settings();
    return settings.value("check_4_updates", true).toBool();
}

void UpdateQmmanderCommand::check4Updates(QNetworkReply* reply)
{
    reply->deleteLater();
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Request failed, " << reply->errorString();        
        emit(updateQmmanderCommandFinished(this));
        return;
    }
    qDebug() << "Request succeeded";
    processResult( reply );
    emit(updateQmmanderCommandFinished(this));
}

void UpdateQmmanderCommand::processResult(QIODevice *source)
{
    QString version;
    QString downloadUrl;
    QXmlStreamReader reader( source );
    while(!reader.atEnd())
    {
        QXmlStreamReader::TokenType tokenType = reader.readNext();
        if ( tokenType == QXmlStreamReader::StartElement )
        {
            if ( reader.name() == QString("Version") )
            {
                tokenType=reader.readNext();
                if(tokenType == QXmlStreamReader::Characters)
                {
                    version=reader.text().toString();
                    qDebug() << version;
                }
            }
            if ( reader.name() == QString("Download") )
            {
                tokenType=reader.readNext();
                if(tokenType == QXmlStreamReader::Characters)
                {
                    downloadUrl=reader.text().toString();
                    qDebug() << downloadUrl;
                }
            }
        }
    }

    UpdateDialog dlg(getSettingsCheck4Update(), &qmndr::Internals::instance().mainWindow());
    if(version==QString(VERSION_STRING))
    {
        QString strText =QString("<html><body><table style=""text-align: left; width: 100%;"" border=""0"" cellspacing=""10""><tbody>")+
                         QString("<tr><td colspan=""2""><b>You have the newest version of Qmmander!</b><(td></tr>")+
                         QString("<tr><td colspan=""2"">Version ")+QString(VERSION_STRING)+QString("</td></tr>")+
                         QString("</tbody></table></body></html>");

        dlg.setLabelText(strText);
    }
    else
    {
        QString strText =QString("<html><body><table style=""text-align: left; width: 100%;"" border=""0"" cellspacing=""10""><tbody>")+
                         QString("<tr><td colspan=""2""><b>There's a new version of Qmmander available!</b><(td></tr>")+
                         QString("<tr><td colspan=""2"">Your version: ")+QString(VERSION_STRING)+QString("</td></tr>")+
                         QString("<tr><td colspan=""2""><b>New version: ")+version+QString("<b></td></tr>")+
                         QString("<tr><td colspan=""2"">You can download the new version by clicking on the link below:</td></tr>")+
                         QString("<tr><td colspan=""2""><a href=""")+downloadUrl+QString(""">")+downloadUrl+QString("</a></td></tr>")+
                         QString("</tbody></table></body></html>");

        dlg.setLabelText(strText);
    }
    dlg.exec();
    setSettingsCheck4Update(dlg.getUpdateBoxState());
}

UpdateQmmanderCommand * UpdateQmmanderCommand::getUpdateCommand()
{
    return new UpdateQmmanderCommand();
}
