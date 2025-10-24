# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = R
DESTDIR     = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include

# #########################################################
# FILES
# #########################################################
HEADERS += \
        include/RRoute.h \
        include/RRouteAStar.h \
        include/RRouteAStarControl.h \
        include/RRouteDynamic.h \
        include/RRouteDynamicControl.h \
        include/RSource.h

SOURCES += \
        source/RRoute.cpp \
        source/RRouteAStar.cpp \
        source/RRouteAStarControl.cpp \
        source/RRouteDynamic.cpp \
        source/RRouteDynamicControl.cpp \
        source/RSource.cpp

