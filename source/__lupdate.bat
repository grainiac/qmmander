@echo off
set QTPROJECTFILE=Qmmander.pro
set PATH2QTBIN=C:\Qt\2009.04\qt\bin\

%PATH2QTBIN%lupdate.exe -pro "%QTPROJECTFILE%"
