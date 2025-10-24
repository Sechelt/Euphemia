# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = AW
DESTDIR		= ../lib

include(../../Euphemia.pri)

CONFIG		+= staticlib

INCLUDEPATH	+= include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH += ../W/include
INCLUDEPATH += ../AD/include

RESOURCES   = AW.qrc

# #########################################################
# FILES
# #########################################################
HEADERS	+= \
    source/LibInfo.h \
    include/AWClassDialog.h \
    include/AWClassNameWidget.h \
    include/AWClassWidget.h \
    include/AWDocumentNameWidget.h \
    include/AWDataDialog.h \
    include/AWDataTable.h \
    include/AWDataWidget.h \
    include/AWDevScriptWidget.h \
    include/AWDevWidget.h \
    include/AWDynamicDataWidget.h \
    include/AWEditorWidget.h \
    include/AWFactoryWidget.h \
    include/AWFile.h \
    include/AWFolder.h \
    include/AWObject.h \
    include/AWObjectChildrenWidget.h \
    include/AWObjectNameWidget.h \
    include/AWObjectSelectWidget.h \
    include/AWObjectsWidget.h \
    include/AWObjectUsedByWidget.h \
    include/AWObjectUsesWidget.h \
    include/AWOutputMessagesWidget.h \
    include/AWOutputSearchWidget.h \
    include/AWOutputValidationWidget.h \
    include/AWOutputWidget.h \
    include/AWPalWidget.h \
    include/AWPanWidget.h \
    include/AWPropObjectWidget.h \
    include/AWPropWidget.h \
    include/AWQuery.h \
    include/AWScratchWidget.h \
    include/AWTreeWidget.h \
    include/AWValidationWidget.h

SOURCES	+= \
    source/AWClassDialog.cpp \
    source/AWClassNameWidget.cpp \
    source/AWClassWidget.cpp \
    source/AWDocumentNameWidget.cpp \
    source/AWDataDialog.cpp \
    source/AWDataTable.cpp \
    source/AWDataWidget.cpp \
    source/AWDevScriptWidget.cpp \
    source/AWDevWidget.cpp \
    source/AWDynamicDataWidget.cpp \
    source/AWEditorWidget.cpp \
    source/AWFactoryWidget.cpp \
    source/AWFile.cpp \
    source/AWFolder.cpp \
    source/AWObject.cpp \
    source/AWObjectChildrenWidget.cpp \
    source/AWObjectNameWidget.cpp \
    source/AWObjectSelectWidget.cpp \
    source/AWObjectsWidget.cpp \
    source/AWObjectUsedByWidget.cpp \
    source/AWObjectUsesWidget.cpp \
    source/AWOutputMessagesWidget.cpp \
    source/AWOutputSearchWidget.cpp \
    source/AWOutputValidationWidget.cpp \
    source/AWOutputWidget.cpp \
    source/AWPalWidget.cpp \
    source/AWPanWidget.cpp \
    source/AWPropObjectWidget.cpp \
    source/AWPropWidget.cpp \
    source/AWQuery.cpp \
    source/AWScratchWidget.cpp \
    source/AWTreeWidget.cpp \
    source/AWValidationWidget.cpp


