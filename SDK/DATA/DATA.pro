# #########################################################
# DATA - database access via ODBC
#
# We create/use this because we want 100% of what ODBC has to offer - not just
# what someone elses API offers us. 
#
# Benefits;
#
#   - simplified calls
#   - convenience methods
#   - use of Qt types (QVariant and QString for example)
#   - use of Qt signal/slot mechanism to emit messages/diagnostics
#   - complete control over interface with ODBC
# 
# #########################################################
TEMPLATE    = lib
TARGET      = DATA
DESTDIR     = ../lib
RESOURCES   = DATA.qrc

# only needed if we are shared lib?
# VERSION = %V_MAJOUR%.%V_MINOR%.%V_RELEASE%

include(../../Euphemia.pri)

CONFIG += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH += ../W/include

# #########################################################
# FILES
# #########################################################
HEADERS =   \ 
            include/DATAAttrWidget.h \
            include/DATAConnectDialog.h \
            include/DATAConnection.h \
            include/DATAConstants.h \
            include/DATADataType.h \
            include/DATADataTypesWidget.h \
		    include/DATADiagnostic.h \
		    include/DATAEnvironment.h \
            include/DATAFunctionsWidget.h \
		    include/DATAHandle.h \
            include/DATAInfoWidget.h \
		    include/DATAMessage.h \
            include/DATAOutputDiagnosticsWidget.h \
            include/DATAOutputMessagesWidget.h \
            include/DATAProfile.h \
            include/DATAProfiles.h \
            include/DATAProfileWidget.h \
            include/DATAResultSetWidget.h \
            include/DATASqlWidget.h \
		    include/DATAStatement.h \
		    include/DATASystem.h

SOURCES =   \ 
            source/DATAAttrWidget.cpp \
            source/DATAConnectDialog.cpp \
            source/DATAConnection.cpp \
            source/DATAConstants.cpp \
            source/DATADataType.cpp \
            source/DATADataTypesWidget.cpp \
            source/DATADiagnostic.cpp \
		    source/DATAEnvironment.cpp \
            source/DATAFunctionsWidget.cpp \
		    source/DATAHandle.cpp \
            source/DATAInfoWidget.cpp \
		    source/DATAMessage.cpp \
            source/DATAOutputDiagnosticsWidget.cpp \
            source/DATAOutputMessagesWidget.cpp \
            source/DATAProfile.cpp \
            source/DATAProfiles.cpp \
            source/DATAProfileWidget.cpp \
            source/DATAResultSetWidget.cpp \
            source/DATASqlWidget.cpp \
		    source/DATAStatement.cpp \
		    source/DATASystem.cpp

