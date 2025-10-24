# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = FL
DESTDIR     = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH += ../W/include
INCLUDEPATH += ../AD/include
INCLUDEPATH += ../AW/include
INCLUDEPATH += ../R/include
INCLUDEPATH += ../D/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = FL.qrc

HEADERS += \
        include/FLAsync.h \
        include/FLBegin.h \
        include/FLConditionClose.h \
        include/FLConditionOpen.h \
        include/FLDiagram.h \
        include/FLDiagramEditorWidget.h \
        include/FLDiagramPalWidget.h \
        include/FLEnd.h \
        include/FLFlow.h \
        include/FLLayout.h \
        include/FLLoopClose.h \
        include/FLLoopOpen.h \
        include/FLNameWidget.h \
        include/FLRectangle.h \
        include/FLStep.h \
        include/FLSync.h

SOURCES += \
        source/FLAsync.cpp \
        source/FLBegin.cpp \
        source/FLConditionClose.cpp \
        source/FLConditionOpen.cpp \
        source/FLDiagram.cpp \
        source/FLDiagramEditorWidget.cpp \
        source/FLDiagramPalWidget.cpp \
        source/FLEnd.cpp \
        source/FLFlow.cpp \
        source/FLLayout.cpp \
        source/FLLoopClose.cpp \
        source/FLLoopOpen.cpp \
        source/FLNameWidget.cpp \
        source/FLRectangle.cpp \
        source/FLStep.cpp \
        source/FLSync.cpp \
        source/LibInfo.h


