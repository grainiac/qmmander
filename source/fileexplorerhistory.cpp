/****************************************************************************
**
** This file is part of the Qmmander Filemanager.
**
** Copyright (C) Alex Skoruppa 2009-2011
** All rights reserved.
**
** This class provides a browsing history for the FileExplorerViews.
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
#include "fileexplorerhistory.h"

FileExplorerHistory::FileExplorerHistory()
: historyIndex_(0)
{
}

void FileExplorerHistory::updateHistory(QString url)
{
    history_.push_back(url);
    historyIndex_=history_.count()-1;
}

QString FileExplorerHistory::actual()
{
    if(history_.count()==0)
        return "";
    else
        return history_[historyIndex_];
}

QString FileExplorerHistory::back()
{
    if(history_.count()==0)
        return "";
    if(historyIndex_>0)
        historyIndex_--;
    return history_[historyIndex_];
}

QString FileExplorerHistory::forward()
{
    if(history_.count()==0)
        return "";
    if(historyIndex_<history_.count()-1)
        historyIndex_++;
    return history_[historyIndex_];
}

int FileExplorerHistory::getActualIndex()
{
    return historyIndex_;
}

int FileExplorerHistory::getHistoryCount()
{
    return history_.count();
}
