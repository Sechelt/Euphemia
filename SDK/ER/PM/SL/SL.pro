# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = SL
DESTDIR     = ../../../lib

include(../../../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH += ../../../include
INCLUDEPATH += ../../../CBD/include
INCLUDEPATH += ../../../W/include
INCLUDEPATH += ../../../DATA/include
INCLUDEPATH += ../../../AD/include
INCLUDEPATH += ../../../AW/include
INCLUDEPATH += ../../../DT/include
INCLUDEPATH += ../../../D/include
# INCLUDEPATH += ../../LM/include
INCLUDEPATH += ../PM/include

# #########################################################
# FILES - MODEL
# #########################################################
HEADERS		+= \
        include/SLColumn.h \
        include/SLDomain.h \
        include/SLIndex.h \
        include/SLIndexColumn.h \
        include/SLKey.h \
        include/SLModel.h \
        include/SLModelEditorWidget.h \
        include/SLModelPalWidget.h \
        include/SLProcedure.h \
        include/SLReference.h \
        include/SLReferenceColumn.h \
        include/SLRule.h \
        include/SLSequence.h \
        include/SLTable.h \
        include/SLView.h \
        include/SLViewColumn.h

SOURCES		+= \
        source/LibInfo.h \
        source/SLColumn.cpp \
        source/SLDomain.cpp \
        source/SLIndex.cpp \
        source/SLIndexColumn.cpp \
        source/SLKey.cpp \
        source/SLModel.cpp \
        source/SLModelEditorWidget.cpp \
        source/SLModelPalWidget.cpp \
        source/SLProcedure.cpp \
        source/SLReference.cpp \
        source/SLReferenceColumn.cpp \
        source/SLRule.cpp \
        source/SLSequence.cpp \
        source/SLTable.cpp \
        source/SLView.cpp \
        source/SLViewColumn.cpp

