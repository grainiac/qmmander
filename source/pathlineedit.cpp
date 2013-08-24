/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2013
** All rights reserved.
**
** A specialized version of QLineEdit.
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
#include "pathlineedit.h"
#include "FileExplorerView.h"
#include "FileExplorer.h"

#include <QMoveEvent>

PathLineEdit::PathLineEdit( QWidget * parent )
:   QLineEdit(parent)
{
}

void PathLineEdit::mousePressEvent(QMouseEvent* e)
{
    QWidget::mousePressEvent(e);
    if(e->button()==Qt::LeftButton)
    {
        FileExplorerView* view=dynamic_cast<FileExplorerView*>(QWidget::parentWidget());
        if(view)
        {
            // Click navigation in PathLineEdit-field
            int pos=cursorPositionAt(e->pos());
            if(pos<2)
                view->changeDirNoHistory(text().left(2));
            else if(pos<text().length())
            {
                QString newPath;
                if(text().at(pos)=='\\')
                    newPath=text().left(pos);
                else
                {
                    int iIndex=text().indexOf('\\', pos);
                    if(iIndex!=-1)
                        newPath=text().left(iIndex);
                }
                if(newPath.toUpper().contains(".ZIP")) // if the path contains an archive mark it...
                {
                    pos=newPath.toUpper().indexOf(".ZIP");
                    pos=newPath.lastIndexOf("\\", pos);
                    newPath.insert(pos+1, "<Archive>");
                }
                else if(newPath.toUpper().contains(".7Z"))
                {
                    pos=newPath.toUpper().indexOf(".7Z");
                    pos=newPath.lastIndexOf("\\", pos);
                    newPath.insert(pos+1, "<Archive>");
                }
                if(newPath.length()>0)
                    view->changeDirNoHistory(newPath);
            }
        }
    }
}
