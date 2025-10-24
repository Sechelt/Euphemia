# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = MSA
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
        include/MSAColumn.h \
        include/MSADomain.h \
        include/MSAIndex.h \
        include/MSAIndexColumn.h \
        include/MSAKey.h \
        include/MSAModel.h \
        include/MSAModelEditorWidget.h \
        include/MSAModelPalWidget.h \
        include/MSAProcedure.h \
        include/MSAReference.h \
        include/MSAReferenceColumn.h \
        include/MSARule.h \
        include/MSASequence.h \
        include/MSATable.h \
        include/MSAView.h \
        include/MSAViewColumn.h

SOURCES		+= \
        source/LibInfo.h \
        source/MSAColumn.cpp \
        source/MSADomain.cpp \
        source/MSAIndex.cpp \
        source/MSAIndexColumn.cpp \
        source/MSAKey.cpp \
        source/MSAModel.cpp \
        source/MSAModelEditorWidget.cpp \
        source/MSAModelPalWidget.cpp \
        source/MSAProcedure.cpp \
        source/MSAReference.cpp \
        source/MSAReferenceColumn.cpp \
        source/MSARule.cpp \
        source/MSASequence.cpp \
        source/MSATable.cpp \
        source/MSAView.cpp \
        source/MSAViewColumn.cpp

