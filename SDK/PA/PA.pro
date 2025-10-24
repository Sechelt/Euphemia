# #########################################################
# PAINT
# ===============
#
# This provides a LLayer which supports 'painting' aka bitmap/raster/pixel editing.
#
# Strength: - can blend a paint action with existing canvas pixels
#           - supports pixel manipulation filters
# Weakness: does not scale well
# Uses: - working with photos which are not likley to be scaled much
#       - working with art which is not likley to get scaled much
#       - working with deliberately pixelated art 
#
# #########################################################
TEMPLATE      = lib
TARGET        = PA
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
RESOURCES   = PA.qrc

HEADERS = \
        include/PA.h \
        include/PAArc.h \
        include/PAChord.h \
        include/PAEllipse.h \
        include/PAEllipseFilled.h \
        include/PAErase.h \
        include/PAFloodFill.h \
        include/PAFreeHand.h \
        include/PAGradientFill.h \
        include/PALine.h \
        include/PAPaint.h \
        include/PAPasteRectangle.h \
        include/PAPie.h \
        include/PAPixelSelection.h \
        include/PAPolygon.h \
        include/PAPolygonFilled.h \
        include/PAPolyline.h \
        include/PARectangle.h \
        include/PARectangleFilled.h \
        include/PASelectEllipse.h \
        include/PASelectPolygon.h \
        include/PASelectRectangle.h \
        include/PASErase.h \
        include/PASFreeHand.h \
        include/PASGradient.h \
        include/PASPaste.h \
        include/PASPolygonFilled.h \
        include/PASpray.h \
        include/PASSpray.h \
        include/PAText.h \
        include/PATool.h \
        include/PAToolFree.h

SOURCES = \
        source/LibInfo.h \
        source/PAArc.cpp \
        source/PAChord.cpp \
        source/PAEllipse.cpp \
        source/PAEllipseFilled.cpp \
        source/PAErase.cpp \
        source/PAFloodFill.cpp \
        source/PAFreeHand.cpp \
        source/PAGradientFill.cpp \
        source/PALine.cpp \
        source/PAPaint.cpp \
        source/PAPasteRectangle.cpp \
        source/PAPie.cpp \
        source/PAPixelSelection.cpp \
        source/PAPolygon.cpp \
        source/PAPolygonFilled.cpp \
        source/PAPolyline.cpp \
        source/PARectangle.cpp \
        source/PARectangleFilled.cpp \
        source/PASelectEllipse.cpp \
        source/PASelectPolygon.cpp \
        source/PASelectRectangle.cpp \
        source/PASErase.cpp \
        source/PASFreeHand.cpp \
        source/PASGradient.cpp \
        source/PASPaste.cpp \
        source/PASPolygonFilled.cpp \
        source/PASpray.cpp \
        source/PASSpray.cpp \
        source/PAText.cpp \
        source/PATool.cpp \
        source/PAToolFree.cpp


