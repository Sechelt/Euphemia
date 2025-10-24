# #########################################################
# LAYERS
# ===============
#
# This introduces QGraphicsScene (etc) to the SDK by providing support 
# for layers where a layer can be (ie) raster (painting) or vector (drawing) based.
#
# #########################################################
TEMPLATE    = lib
TARGET      = L
DESTDIR     = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH	+= ../AD/include
INCLUDEPATH	+= ../AW/include
INCLUDEPATH	+= ../W/include
INCLUDEPATH	+= ../S/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = L.qrc

HEADERS += \
        source/LibInfo.h \
        include/L.h \
        include/LColorsWidget.h \
        include/LCrossHairLine.h \
        include/LEditorWidget.h \
        include/LFile.h \
        include/LGraphicsLayer.h \
        include/LGraphicsLine.h \
        include/LGraphicsObject.h \
        include/LGraphicsScene.h \
        include/LGraphicsView.h \
        include/LHandle.h \
        include/LLayer.h \
        include/LLayersWidget.h \
        include/LObject.h \
        include/LPanner.h \
        include/LPixelsWidget.h \
        include/LSBackground.h \
        include/LScratchWidget.h \
        include/LSEllipse.h \
        include/LSGeneral.h \
        include/LSGrid.h \
        include/LSImage.h \
        include/LSLine.h \
        include/LSPage.h \
        include/LSPointer.h \
        include/LSRectangle.h \
        include/LSRender.h \
        include/LSText.h \
        include/LToolsWidget.h

SOURCES += \
        source/LColorsWidget.cpp \
        source/LCrossHairLine.cpp \
        source/LEditorWidget.cpp \
        source/LFile.cpp \
        source/LGraphicsLayer.cpp \
        source/LGraphicsLine.cpp \
        source/LGraphicsObject.cpp \
        source/LGraphicsScene.cpp \
        source/LGraphicsView.cpp \
        source/LHandle.cpp \
        source/LLayer.cpp \
        source/LLayersWidget.cpp \
        source/LObject.cpp \
        source/LPanner.cpp \
        source/LPixelsWidget.cpp \
        source/LSBackground.cpp \
        source/LScratchWidget.cpp \
        source/LSEllipse.cpp \
        source/LSGeneral.cpp \
        source/LSGrid.cpp \
        source/LSImage.cpp \
        source/LSLine.cpp \
        source/LSPage.cpp \
        source/LSPointer.cpp \
        source/LSRectangle.cpp \
        source/LSRender.cpp \
        source/LSText.cpp \
        source/LToolsWidget.cpp

