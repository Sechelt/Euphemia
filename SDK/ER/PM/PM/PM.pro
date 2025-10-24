# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = PM
DESTDIR     = ../../../lib

include(../../../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH += ../../../include
INCLUDEPATH += ../../../CBD/include
INCLUDEPATH += ../../../W/include
INCLUDEPATH += ../../../DATA/include
INCLUDEPATH += ../../../DATAW/include
INCLUDEPATH += ../../../AD/include
INCLUDEPATH += ../../../AW/include
INCLUDEPATH += ../../../DT/include
INCLUDEPATH += ../../../D/include
INCLUDEPATH += ../../LM/include

# #########################################################
# FILES - MODEL
# #########################################################
RESOURCES   = PM.qrc

HEADERS		+= \
        include/PMAdditionalChecksWidget.h \
        include/PMColumn.h \
        include/PMColumnChecksWidget.h \
        include/PMColumnsUsedWidget.h \
        include/PMColumnsWidget.h \
        include/PMDataTypeArgsWidget.h \
        include/PMDataTypeWidget.h \
        include/PMDDLManager.h \
        include/PMDDLWidget.h \
        include/PMDomain.h \
        include/PMDomainchecksWidget.h \
        include/PMIndex.h \
        include/PMIndexColumn.h \
        include/PMIndexColumnsWidget.h \
        include/PMIndexesWidget.h \
        include/PMInherit.h \
        include/PMKey.h \
        include/PMKeyOptionsWidget.h \
        include/PMKeysWidget.h \
        include/PMLine.h \
        include/PMModel.h \
        include/PMModelDisplayWidget.h \
        include/PMNotesWidget.h \
        include/PMObject.h \
        include/PMProcedure.h \
        include/PMProcedureWidget.h \
        include/PMRectangle.h \
        include/PMReference.h \
        include/PMReferenceColumn.h \
        include/PMReferenceColumnsWidget.h \
        include/PMReferenceWidget.h \
        include/PMRule.h \
        include/PMRulesUsedWidget.h \
        include/PMRuleWidget.h \
        include/PMSequence.h \
        include/PMSequenceWidget.h \
        include/PMTableBase.h \
        include/PMTableDerived.h \
        include/PMTableNormal.h \
        include/PMViewColumn.h \
        include/PMView.h \
        include/PMViewWidget.h

SOURCES		+= \
        source/LibInfo.h \
        source/PMAdditionalChecksWidget.cpp \
        source/PMColumn.cpp \
        source/PMColumnChecksWidget.cpp \
        source/PMColumnsUsedWidget.cpp \
        source/PMColumnsWidget.cpp \
        source/PMDataTypeArgsWidget.cpp \
        source/PMDataTypeWidget.cpp \
        source/PMDDLManager.cpp \
        source/PMDDLWidget.cpp \
        source/PMDomain.cpp \
        source/PMDomainChecksWidget.cpp \
        source/PMIndex.cpp \
        source/PMIndexColumn.cpp \
        source/PMIndexColumnsWidget.cpp \
        source/PMIndexesWidget.cpp \
        source/PMInherit.cpp \
        source/PMKey.cpp \
        source/PMKeyOptionsWidget.cpp \
        source/PMKeysWidget.cpp \
        source/PMLine.cpp \
        source/PMModel.cpp \
        source/PMModelDisplayWidget.cpp \
        source/PMNotesWidget.cpp \
        source/PMObject.cpp \
        source/PMProcedure.cpp \
        source/PMProcedureWidget.cpp \
        source/PMRectangle.cpp \
        source/PMReference.cpp \
        source/PMReferenceColumn.cpp \
        source/PMReferenceColumnsWidget.cpp \
        source/PMReferenceWidget.cpp \
        source/PMRule.cpp \
        source/PMRulesUsedWidget.cpp \
        source/PMRuleWidget.cpp \
        source/PMSequence.cpp \
        source/PMSequenceWidget.cpp \
        source/PMTableBase.cpp \
        source/PMTableDerived.cpp \
        source/PMTableNormal.cpp \
        source/PMView.cpp \
        source/PMViewColumn.cpp \
        source/PMViewWidget.cpp



