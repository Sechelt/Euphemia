TARGET      = widgets
DESTDIR     = ../../bin
TEMPLATE    = app

include(../../Euphemia.pri)

INCLUDEPATH += ../../SDK/CBD/include
INCLUDEPATH += ../../SDK/include
INCLUDEPATH += ../../SDK

HEADERS += \
        MainWindow.h

SOURCES += \
        main.cpp \
        MainWindow.cpp


