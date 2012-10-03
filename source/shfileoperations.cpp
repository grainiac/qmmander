/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** Helper functions for Win32 SHFileOperation.
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
#include "shfileoperations.h"
#include <QList>
#include <QStringList>
#include <shellapi.h>

TCHAR* getSHFileOperationFromString(WinFileInfoList list)
{
    // Strings for shellapi function must be double zero terminated
    int iBuffLength=0;
    for(int i=0; i<list.count(); i++)
    {
        iBuffLength+=list.at(i).filePath().length();   // regular characters
    }
    iBuffLength+=list.count()+1; // string length + ammount of null characters
    TCHAR* pszzSHFOL=new TCHAR[iBuffLength];
    memset(pszzSHFOL, 0, sizeof(TCHAR)*iBuffLength);

    int iPos=0;
    for(int i=0; i<list.count(); i++)
    {
        QString str=list.at(i).filePath();
        wcscpy(&pszzSHFOL[iPos], str.toStdWString().c_str());
        pszzSHFOL[iPos+str.length()]='\0';
        iPos+=str.length()+1;
    }
    pszzSHFOL[iPos]='\0';
    return pszzSHFOL;
}

TCHAR* getSHFileOperationFromString(QStringList files)
{
    // Strings for shellapi function must be double zero terminated
    int iBuffLength=0;
    for(int i=0; i<files.count(); i++)
    {
        iBuffLength+=files.at(i).length();   // regular characters
    }
    iBuffLength+=files.count()+1; // string length + ammount of null characters
    TCHAR* pszzSHFOL=new TCHAR[iBuffLength];
    memset(pszzSHFOL, 0, sizeof(TCHAR)*iBuffLength);

    int iPos=0;
    for(int i=0; i<files.count(); i++)
    {
        QString str=files.at(i);
        wcscpy(&pszzSHFOL[iPos], str.toStdWString().c_str());
        pszzSHFOL[iPos+str.length()]='\0';
        iPos+=str.length()+1;
    }
    pszzSHFOL[iPos]='\0';
    return pszzSHFOL;
}

TCHAR* getSHFileOperationMoveToString(WinFileInfoList listOfFiles2Move, QString strDestPath)
{
    // Strings for shellapi function must be double zero terminated
    int iBuffLength=0;
    for(int i=0; i<listOfFiles2Move.count(); i++)
    {
        iBuffLength+=strDestPath.length()+1;                       // +1 for "\\" at the end
        iBuffLength+=listOfFiles2Move.at(i).fileName().length();   // regular characters
    }
    iBuffLength+=listOfFiles2Move.count()+1; // string length + ammount of null characters
    TCHAR* pszzSHFOL=new TCHAR[iBuffLength];
    memset(pszzSHFOL, 0, sizeof(TCHAR)*iBuffLength);

    int iPos=0;
    for(int i=0; i<listOfFiles2Move.count(); i++)
    {
        QString str=strDestPath+"\\"+listOfFiles2Move.at(i).fileName();
        wcscpy(&pszzSHFOL[iPos], str.toStdWString().c_str());
        pszzSHFOL[iPos+str.length()]='\0';
        iPos+=str.length()+1;
    }
    pszzSHFOL[iPos]='\0';
    return pszzSHFOL;
}

TCHAR* getSHFileOperationMoveToString(QStringList listOfFiles2Move, QString strDestPath)
{
    // Strings for shellapi function must be double zero terminated
    int iBuffLength=0;
    for(int i=0; i<listOfFiles2Move.count(); i++)
    {
        QFileInfo file(listOfFiles2Move.at(i));
        iBuffLength+=strDestPath.length()+1;         // +1 for "\\" at the end
        iBuffLength+=file.fileName().length() + 1;   // regular characters
    }
    iBuffLength+=listOfFiles2Move.count()+1; // string length + ammount of null characters
    TCHAR* pszzSHFOL=new TCHAR[iBuffLength];
    memset(pszzSHFOL, 0, sizeof(TCHAR)*iBuffLength);

    int iPos=0;
    for(int i=0; i<listOfFiles2Move.count(); i++)
    {
        QFileInfo file(listOfFiles2Move.at(i));
        QString str=strDestPath+"\\"+file.fileName();
        wcscpy(&pszzSHFOL[iPos], str.toStdWString().c_str());
        pszzSHFOL[iPos+str.length()]='\0';
        iPos+=str.length()+1;
    }
    pszzSHFOL[iPos]='\0';
    return pszzSHFOL;
}

TCHAR* getSHFileOperationCopyToString(QString strFilePath)
{
    int iBuffLength=strFilePath.length()+2; // string length + ammount of null characters
    TCHAR* pszz=new TCHAR[iBuffLength];
    memset(pszz, 0, sizeof(TCHAR)*iBuffLength);
    wcscpy(pszz, strFilePath.toStdWString().c_str());
    pszz[strFilePath.length()]='\0';
    pszz[strFilePath.length()+1]='\0';
    return pszz;
}
