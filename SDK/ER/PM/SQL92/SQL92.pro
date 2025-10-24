# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = SQL92
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
        include/SQL92Column.h \
        include/SQL92Domain.h \
        include/SQL92Index.h \
        include/SQL92IndexColumn.h \
        include/SQL92Key.h \
        include/SQL92Model.h \
        include/SQL92ModelEditorWidget.h \
        include/SQL92ModelPalWidget.h \
        include/SQL92Procedure.h \
        include/SQL92Reference.h \
        include/SQL92ReferenceColumn.h \
        include/SQL92Rule.h \
        include/SQL92Sequence.h \
        include/SQL92Table.h \
        include/SQL92View.h \
        include/SQL92ViewColumn.h

SOURCES		+= \
        source/LibInfo.h \
        source/SQL92Column.cpp \
        source/SQL92Domain.cpp \
        source/SQL92Index.cpp \
        source/SQL92IndexColumn.cpp \
        source/SQL92Key.cpp \
        source/SQL92Model.cpp \
        source/SQL92ModelEditorWidget.cpp \
        source/SQL92ModelPalWidget.cpp \
        source/SQL92Procedure.cpp \
        source/SQL92Reference.cpp \
        source/SQL92ReferenceColumn.cpp \
        source/SQL92Rule.cpp \
        source/SQL92Sequence.cpp \
        source/SQL92Table.cpp \
        source/SQL92View.cpp \
        source/SQL92ViewColumn.cpp

