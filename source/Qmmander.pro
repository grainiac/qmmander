# -------------------------------------------------
# Project created by QtCreator 2009-09-19T21:15:02
# -------------------------------------------------
TARGET = Qmmander
TEMPLATE = app
QT = core \
    gui \
    widgets \
    network \
    xml

DEFINES += NOMINMAX

SOURCES += main.cpp \
    mainwindow.cpp \
    FileExplorer.cpp \
    FileExplorerView.cpp \
    pathlineedit.cpp \
    filetablewidget.cpp \
    winfileinfo.cpp \
    filelistwidget.cpp \
    renamedialog.cpp \
    networktreewidget.cpp \
    shfileoperations.cpp \
    networkresource.cpp \
    updatedialog.cpp \
    drivecheckerthread.cpp \
    unzipdialog.cpp \
    unzipthread.cpp \
    zipdialog.cpp \
    zipunzipprogressdialog.cpp \
    fileactiondialog.cpp \
    zipunzipwatcherthread.cpp \
    archive.cpp \
    zipthread.cpp \
    ZipUtils/zip.cpp \
    ZipUtils/unzip.cpp \
    ZipUtils/modify.cpp \
    ziputils.cpp \
    fileactionfactories.cpp \
    fileactionhandlers.cpp \
    archiverthread.cpp \
    archiveinfo.cpp \
    attributesdatedialog.cpp \
    attributesanddatechangerthread.cpp \
    fileexplorertarget.cpp \
    fileexplorerhistory.cpp \
    copycommand.cpp \
    deletecommand.cpp \
    deletetorecyclebincommand.cpp \
    movecommand.cpp \
    renamecommand.cpp \
    dropcommand.cpp \
    changefiledatetimecommand.cpp \
    archivecommand.cpp \
    newfoldercommand.cpp \
    updateqmmandercommand.cpp \
    callhelperappcommand.cpp \
    aboutcommand.cpp \
    internals.cpp
HEADERS += mainwindow.h \
    FileExplorer.h \
    FileExplorerView.h \
    pathlineedit.h \
    filetablewidget.h \
    winfileinfo.h \
    filelistwidget.h \
    renamedialog.h \
    networktreewidget.h \
    shfileoperations.h \
    version.h \
    networkresource.h \
    updatedialog.h \
    drivecheckerthread.h \
    unzipdialog.h \
    unzipthread.h \
    zipdialog.h \
    zipunzipprogressdialog.h \
    archiveinfo.h \
    fileactiondialog.h \
    zipunzipwatcherthread.h \
    archive.h \
    zipthread.h \
    ZipUtils/modify.h \
    ZipUtils/zip.h \
    ZipUtils/unzip.h \
    ziputils.h \
    fileactionfactories.h \
    fileactionhandlers.h \
    archiverthread.h \
    fileaction.h \    
    attributesdatedialog.h \
    attributesanddatechangerthread.h \
    fileexplorertarget.h \
    fileexplorerhistory.h \
    command.h \
    copycommand.h \
    deletecommand.h \
    deletetorecyclebincommand.h \
    movecommand.h \
    renamecommand.h \
    dropcommand.h \
    changefiledatetimecommand.h \
    archivecommand.h \
    newfoldercommand.h \
    updateqmmandercommand.h \
    callhelperappcommand.h \
    aboutcommand.h \
    internals.h
FORMS += mainwindow.ui \
    FileExplorer.ui \
    FileExplorerView.ui \
    renamedialog.ui \
    newfolderdialog.ui \
    updatedialog.ui \
    unzipdialog.ui \
    zipdialog.ui \
    zipunzipprogressdialog.ui \
    fileactiondialog.ui \
    attributesdatedialog.ui
OTHER_FILES += LicenseTemplate.txt
RC_FILE = qmmander.rc
TRANSLATIONS = language_ts/Qmmander_de.ts
RESOURCES += Qmmander.qrc
LIBS += -lgdi32 \
    -lole32 \
    -lmpr \
    -lshlwapi \
# for VS 2010
    -lshell32 \
    -luser32

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rd_party/PhysFS_DLL/2.0.3/VC2012_Release/ -lphysfs
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rd_party/PhysFS_DLL/2.0.3/VC2012_Debug/ -lphysfs

INCLUDEPATH += $$PWD/../3rd_party/PhysFS_DLL/2.0.3/include
DEPENDPATH += $$PWD/../3rd_party/PhysFS_DLL/2.0.3/include
