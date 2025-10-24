# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = ODBC
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
        include/ODBCColumn.h \
        include/ODBCDomain.h \
        include/ODBCIndex.h \
        include/ODBCIndexColumn.h \
        include/ODBCKey.h \
        include/ODBCModel.h \
        include/ODBCModelEditorWidget.h \
        include/ODBCModelPalWidget.h \
        include/ODBCProcedure.h \
        include/ODBCReference.h \
        include/ODBCTable.h \
        include/ODBCView.h \
        include/ODBCCorModel.h \
        include/ODBCDrvModel.h \
        include/ODBCExtModel.h \
        include/ODBCMinModel.h

SOURCES		+= \
        source/LibInfo.h \
        source/ODBCColumn.cpp \
        source/ODBCDomain.cpp \
        source/ODBCIndex.cpp \
        source/ODBCIndexColumn.cpp \
        source/ODBCKey.cpp \
        source/ODBCModel.cpp \
        source/ODBCModelEditorWidget.cpp \
        source/ODBCModelPalWidget.cpp \
        source/ODBCProcedure.cpp \
        source/ODBCReference.cpp \
        source/ODBCTable.cpp \
        source/ODBCView.cpp \
        source/ODBCCorModel.cpp \
        source/ODBCDrvModel.cpp \
        source/ODBCExtModel.cpp \
        source/ODBCMinModel.cpp


