TARGET      = blue
DESTDIR     = ../../bin
TEMPLATE    = app

include(../../Euphemia.pri)

# SDK HEADERS
INCLUDEPATH += ../../SDK/DR/include
INCLUDEPATH += ../../SDK/PA/include
INCLUDEPATH += ../../SDK/L/include
INCLUDEPATH += ../../SDK/W/include
INCLUDEPATH += ../../SDK/S/include
INCLUDEPATH += ../../SDK/AW/include
INCLUDEPATH += ../../SDK/AD/include
INCLUDEPATH += ../../SDK/CBD/include
INCLUDEPATH += ../include
INCLUDEPATH += ./include

# SDK LIBS - order matters!
LIBS += -L../../SDK/lib
LIBS += -lDR -lPA
LIBS += -lL -lAD -lAW -lW -lS -lCBD

!win32 {
        LIBS += -lz
}

RESOURCES   = Blue.qrc

HEADERS += \
        source/AppInfo.h \
        include/EExportDialog.h \
        include/EFile.h \
        include/EMainWindow.h \
        include/EPreferencesDialog.h \
        include/EWorkspace.h

SOURCES += \
        source/EExportDialog.cpp \
        source/EFile.cpp \
        source/EMainWindow.cpp \
        source/EPreferencesDialog.cpp \
        source/EWorkspace.cpp \
        source/main.cpp


