# #########################################################
# DRAW
# ===============
#
# This provides a LLayer which supports 'drawing' aka 2D/vector editing.
#
# Strength: - scales very well
#           - objects drawn on the canvas continue to exist as an object
#               - can continue to be manipulated after 'commit' to canvas
#               - can be grouped/ungrouped
#               - can be inserted/removed from a layout
#               - its geometry can be relative to another object
# Weakness: - blending and many other pixel manipulation options are not easily supported if supported at all 
# Uses: - original art where limited pixel manipulation is ok
#       - original art which needs to be good at scaling
#       - diagrams
#
# #########################################################
TEMPLATE      = lib
TARGET        = DR
DESTDIR       = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH	+= ../AD/include
INCLUDEPATH	+= ../AW/include
INCLUDEPATH	+= ../S/include
INCLUDEPATH	+= ../W/include
INCLUDEPATH	+= ../L/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = DR.qrc

HEADERS = \
        include/DR.h \
        include/DRArc.h \
        include/DRChord.h \
        include/DRImage.h \
        include/DRDraw.h \
        include/DREllipse.h \
#        include/DRGroup.h \
        include/DRLine.h \
        include/DRPie.h \
        include/DRPolygon.h \
        include/DRPolyline.h \
        include/DRRectangle.h \
        include/DRRectangleBase.h \
        include/DRSelectPolygon.h \
        include/DRSelectRectangle.h \
        include/DRSImage.h \
        include/DRText.h \
        include/DRObject.h

SOURCES = \
        source/LibInfo.h \
        source/DRArc.cpp \
        source/DRChord.cpp \
        source/DRImage.cpp \
        source/DRDraw.cpp \
        source/DREllipse.cpp \
#        source/DRGroup.cpp \
        source/DRLine.cpp \
        source/DRPie.cpp \
        source/DRPolygon.cpp \
        source/DRPolyline.cpp \
        source/DRRectangle.cpp \
        source/DRRectangleBase.cpp \
        source/DRSelectPolygon.cpp \
        source/DRSelectRectangle.cpp \
        source/DRSImage.cpp \
        source/DRText.cpp \
        source/DRObject.cpp


