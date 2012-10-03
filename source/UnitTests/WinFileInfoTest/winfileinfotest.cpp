/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** UnitTests for class WinFileInfo
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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../winfileinfo.h"

class WinFileInfoTest : public QObject
{
    Q_OBJECT

public:
    WinFileInfoTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    //void testGetDriveList();
    //void testIsSmbRootPath();
    void testIsSmbShareRootPath();
    //void testListFiles();

private:

};

WinFileInfoTest::WinFileInfoTest()
{
}

void WinFileInfoTest::initTestCase()
{
}

void WinFileInfoTest::cleanupTestCase()
{
}

/*void WinFileInfoTest::testGetDriveList()
{
    QStringList drives=WinFileInfo::getDriveList();
    QString allDrives;
    foreach (QString drive, drives)
    {
        allDrives+=drive;
    }
    QVERIFY(allDrives == "C:D:E:");
}

void WinFileInfoTest::testIsSmbRootPath()
{
    QVERIFY2(WinFileInfo::isSmbRoot("\\\\MIBBY"), "Error in isSmbRoot()");
}
*/
void WinFileInfoTest::testIsSmbShareRootPath()
{
    //QVERIFY2(WinFileInfo::isSmbShareRoot("\\\\MIBBY\\Users"), "Error in isSmbShareRoot()");
    QVERIFY2(WinFileInfo::isSmbShareRoot("\\\\devimage01\\images"), "Error in isSmbShareRoot()");
}

/*void WinFileInfoTest::testListFiles()
{
    WinFileInfoPtrList* fileList=WinFileInfo::listFiles("\\\\MIBBY");
    QVERIFY2(fileList->count()==1, "Error in testListFiles(QString path)");
    delete fileList;
}*/

QTEST_APPLESS_MAIN(WinFileInfoTest);

#include "winfileinfotest.moc"
