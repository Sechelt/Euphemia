TARGET      = euphemia
DESTDIR     = ./bin
TEMPLATE    = app

include(Euphemia.pri)

INCLUDEPATH += ../Classic-SDK/W/include
INCLUDEPATH += ../Euphemia-SDK/P/include
INCLUDEPATH += ./include

LIBS += -L../Euphemia-SDK/lib -lP
LIBS += -L../Classic-SDK/lib -lW

LIBS += -lz

RESOURCES   = Euphemia.qrc

HEADERS += \
        include/EMainWindow.h \
        include/EPreferencesDialog.h

SOURCES += \
        Euphemia.h \
        source/AppInfo.h \
        source/main.cpp \
        source/EMainWindow.cpp \
        source/EPreferencesDialog.cpp


