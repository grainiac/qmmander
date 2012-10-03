@echo off
set QTPROJECTFILE=Qmmander.pro
set PATH2QTBIN=C:\Qt\2009.04\qt\bin\

%PATH2QTBIN%lrelease.exe %QTPROJECTFILE%

move .\language_ts\*.qm .\language_qm\
xcopy .\language_qm\*.qm .\Release\
xcopy .\language_qm\*.qm .\Debug\
