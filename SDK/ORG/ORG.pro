# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = ORG
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
RESOURCES   = ORG.qrc

HEADERS += \
        include/ORGDiagram.h \
        include/ORGDiagramEditorWidget.h \
        include/ORGDiagramPalWidget.h \
        include/ORGLayout.h \
        include/ORGObjectEmploymentWidget.h \
        include/ORGObjectNameWidget.h \
        include/ORGObjectProjectWidget.h \
        include/ORGPerson.h \
        include/ORGReportsTo.h

SOURCES += \
        source/ORGDiagram.cpp \
        source/ORGDiagramEditorWidget.cpp \
        source/ORGDiagramPalWidget.cpp \
        source/ORGLayout.cpp \
        source/ORGObjectEmploymentWidget.cpp \
        source/ORGObjectNameWidget.cpp \
        source/ORGObjectProjectWidget.cpp \
        source/ORGPerson.cpp \
        source/ORGReportsTo.cpp
        source/LibInfo.h


