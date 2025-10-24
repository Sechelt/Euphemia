TARGET      = route
DESTDIR     = ../../bin
TEMPLATE    = app

include(../../Euphemia.pri)

INCLUDEPATH += ../../SDK/R/include
INCLUDEPATH += ../../SDK/CBD/include
INCLUDEPATH += ../../SDK/include
INCLUDEPATH += ../../SDK

LIBS        += -L../../SDK/lib 
LIBS        += -lR

HEADERS += \
        MainWindow.h \
        RouteControlWidget.h \
        RouteScene.h \
        RouteSource.h \
        RouteWidget.h

SOURCES += \
        main.cpp \
        MainWindow.cpp \
        RouteControlWidget.cpp \
        RouteScene.cpp \
        RouteSource.cpp \
        RouteWidget.cpp


