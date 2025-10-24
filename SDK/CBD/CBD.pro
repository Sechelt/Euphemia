# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = CBD
DESTDIR		= ../lib

include(../../Euphemia.pri)

CONFIG		+= staticlib

INCLUDEPATH	+= ./include
INCLUDEPATH	+= ../include

# #########################################################
# FILES
# #########################################################
# RESOURCES   = CBD.qrc

HEADERS	+= \
        include/CBD.h \
        include/CBDMath.h \
        include/CBDPersist.h

SOURCES	+= \
        source/LibInfo.h \
        source/CBD.cpp \
        source/CBDMath.cpp \
        source/CBDPersist.cpp

