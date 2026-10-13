# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = S
DESTDIR		= ../lib

include(../../Euphemia.pri)

CONFIG		+= staticlib

INCLUDEPATH	+= ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH	+= ../W/include

# #########################################################
# FILES
# #########################################################
HEADERS	+= \
        source/LibInfo.h \
        include/S.h \
        include/SBrush.h \
        include/SColors.h \
        include/SColorsPickerDialog.h \
        include/SColorsPickerWidget.h \
        include/SColorsWidget.h \
        include/SFont.h \
        include/SGrid.h \
        include/SPen.h \
        include/SRuler.h \
        include/SSetting.h \
        include/SSettings.h \
        include/SText.h

SOURCES	+= \
        source/SBrush.cpp \
        source/SColors.cpp \
        source/SColorsPickerDialog.cpp \
        source/SColorsPickerWidget.cpp \
        source/SColorsWidget.cpp \
        source/SFont.cpp \
        source/SGrid.cpp \
        source/SPen.cpp \
        source/SRuler.cpp \
        source/SSetting.cpp \
        source/SSettings.cpp \
        source/SText.cpp

