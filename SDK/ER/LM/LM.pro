# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = LM
DESTDIR     = ../../lib

include(../../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH += ../../include
INCLUDEPATH += ../../../include
INCLUDEPATH += ../../CBD/include
INCLUDEPATH += ../../W/include
INCLUDEPATH += ../../DATA/include
INCLUDEPATH += ../../AD/include
INCLUDEPATH += ../../AW/include
INCLUDEPATH += ../../D/include

RESOURCES   = LM.qrc

# #########################################################
# FILES - MODEL
# #########################################################
HEADERS += \
    include/LMAdditionalChecksWidget.h \
    include/LMAssociativeLink.h \
    include/LMAssociativeLinkWidget.h \
    include/LMAttribute.h \
    include/LMAttributeChecksWidget.h \
    include/LMAttributesUsedWidget.h \
    include/LMAttributesWidget.h \
    include/LMDataItem.h \
    include/LMDataItemChecksWidget.h \
    include/LMDataItemsWidget.h \
    include/LMDataTypeArgsWidget.h \
    include/LMDataTypeWidget.h \
    include/LMDiagramDisplayWidget.h \
    include/LMDomain.h \
    include/LMDomainChecksWidget.h \
    include/LMDomainsWidget.h \
    include/LMEntityAssociative.h \
    include/LMEntityBase.h \
    include/LMEntityNormal.h \
    include/LMEntityOptionsWidget.h \
    include/LMIdentifier.h \
    include/LMIdentifierOptionsWidget.h \
    include/LMIdentifiersWidget.h \
    include/LMInheritance.h \
    include/LMInheritanceLink.h \
    include/LMInheritanceWidget.h \
    include/LMLine.h \
    include/LMLineWidget.h \
    include/LMModel.h \
    include/LMModelEditorWidget.h \
    include/LMModelPalWidget.h \
    include/LMNotesWidget.h \
    include/LMObject.h \
    include/LMRectangle.h \
    include/LMRelationship.h \
    include/LMRelationshipWidget.h \
    include/LMRule.h \
    include/LMRulesUsedWidget.h \
    include/LMRulesWidget.h \
    include/LMRuleWidget.h \
    include/LMValidationWidget.h

SOURCES += \
    source/LMAdditionalChecksWidget.cpp \
    source/LMAssociativeLink.cpp \
    source/LMAssociativeLinkWidget.cpp \
    source/LMAttribute.cpp \
    source/LMAttributeChecksWidget.cpp \
    source/LMAttributesUsedWidget.cpp \
    source/LMAttributesWidget.cpp \
    source/LMDataItem.cpp \
    source/LMDataItemChecksWidget.cpp \
    source/LMDataItemsWidget.cpp \
    source/LMDataTypeArgsWidget.cpp \
    source/LMDataTypeWidget.cpp \
    source/LMDiagramDisplayWidget.cpp \
    source/LMDomain.cpp \
    source/LMDomainChecksWidget.cpp \
    source/LMDomainsWidget.cpp \
    source/LMEntityAssociative.cpp \
    source/LMEntityBase.cpp \
    source/LMEntityNormal.cpp \
    source/LMEntityOptionsWidget.cpp \
    source/LMIdentifier.cpp \
    source/LMIdentifierOptionsWidget.cpp \
    source/LMIdentifiersWidget.cpp \
    source/LMInheritance.cpp \
    source/LMInheritanceLink.cpp \
    source/LMInheritanceWidget.cpp \
    source/LMLine.cpp \
    source/LMLineWidget.cpp \
    source/LMModel.cpp \
    source/LMModelEditorWidget.cpp \
    source/LMModelPalWidget.cpp \
    source/LMNotesWidget.cpp \
    source/LMObject.cpp \
    source/LMRectangle.cpp \
    source/LMRelationship.cpp \
    source/LMRelationshipWidget.cpp \
    source/LMRule.cpp \
    source/LMRulesUsedWidget.cpp \
    source/LMRulesWidget.cpp \
    source/LMRuleWidget.cpp \
    source/LMValidationWidget.cpp \
    source/LibInfo.h



