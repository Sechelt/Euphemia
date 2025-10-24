# #########################################################
# DATAW - Provides widgets which build on DATA.
#
# #########################################################
TEMPLATE    = lib
TARGET      = DATAW
DESTDIR     = ../lib

# only needed if we are shared lib?
# VERSION 	= %V_MAJOUR%.%V_MINOR%.%V_RELEASE%

include(../../Euphemia.pri)

CONFIG          += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH += ../W/include
INCLUDEPATH += ../DATA/include
INCLUDEPATH += ../AD/include
INCLUDEPATH += ../AW/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = DATAW.qrc

HEADERS =   \
    include/DATAWConnectDialog.h \
    include/DATAWConnection.h \
    include/DATAWConnectionToolBar.h \
    include/DATAWEditorToolBar.h \
    include/DATAWConnectWizard.h \
    include/DATAWDataWidget.h \
    include/DATAWFindDialog.h \
    include/DATAWMeta.h \
    include/DATAWOutputDiagnosticsWidget.h \
    include/DATAWOutputMessagesWidget.h \
    include/DATAWPropDialog.h \
    include/DATAWQueryCatalog.h \
    include/DATAWQueryCatalogs.h \
    include/DATAWQueryColumn.h \
    include/DATAWQueryConnection.h \
    include/DATAWQueryDataSource.h \
    include/DATAWQueryDataSourceConnection.h \
    include/DATAWQueryDataSources.h \
    include/DATAWQueryDriver.h \
    include/DATAWQueryDriverConnection.h \
    include/DATAWQueryDrivers.h \
    include/DATAWQueryEICallLevelInterface.h \
    include/DATAWQueryEIConversion.h \
    include/DATAWQueryEIDataSource.h \
    include/DATAWQueryEIDataTypes.h \
    include/DATAWQueryEIDBMS.h \
    include/DATAWQueryEIDriver.h \
    include/DATAWQueryEIFunctions.h \
    include/DATAWQueryEILimits.h \
    include/DATAWQueryEISupported.h \
    include/DATAWQueryExtendedInfo.h \
    include/DATAWQueryExtendedInfoItem.h \
    include/DATAWQueryFolder.h \
    include/DATAWQueryForeignKeys.h \
    include/DATAWQueryForeignKey.h \
    include/DATAWQuery.h \
    include/DATAWQueryIndex.h \
    include/DATAWQueryIndexs.h \
    include/DATAWQueryPrimaryKeys.h \
    include/DATAWQuerySchema.h \
    include/DATAWQuerySpecialColumns.h \
    include/DATAWQuerySystem.h \
    include/DATAWQueryTable.h \
    include/DATAWQueryTableType.h \
    include/DATAWSql.h \
    include/DATAWSqlEditorWidget.h \
    include/DATAWTableModel.h \
    include/DATAWTableModelApp.h \
    include/DATAWTableModelODBC.h \
    include/DATAWTableView.h \
    include/DATAWTableWidget.h \
    source/LibInfo.h

SOURCES =   \
    source/DATAWConnectDialog.cpp \
    source/DATAWConnection.cpp \
    source/DATAWConnectionToolBar.cpp \
    source/DATAWEditorToolBar.cpp \
    source/DATAWConnectWizard.cpp \
    source/DATAWDataWidget.cpp \
    source/DATAWFindDialog.cpp \
    source/DATAWMeta.cpp \
    source/DATAWOutputDiagnosticsWidget.cpp \
    source/DATAWOutputMessagesWidget.cpp \
    source/DATAWPropDialog.cpp \
    source/DATAWQueryCatalog.cpp \
    source/DATAWQueryCatalogs.cpp \
    source/DATAWQueryColumn.cpp \
    source/DATAWQueryConnection.cpp \
    source/DATAWQuery.cpp \
    source/DATAWQueryDataSource.cpp \
    source/DATAWQueryDataSourceConnection.cpp \
    source/DATAWQueryDataSources.cpp \
    source/DATAWQueryDriver.cpp \
    source/DATAWQueryDriverConnection.cpp \
    source/DATAWQueryDrivers.cpp \
    source/DATAWQueryEICallLevelInterface.cpp \
    source/DATAWQueryEIConversion.cpp \
    source/DATAWQueryEIDataSource.cpp \
    source/DATAWQueryEIDataTypes.cpp \
    source/DATAWQueryEIDBMS.cpp \
    source/DATAWQueryEIDriver.cpp \
    source/DATAWQueryEIFunctions.cpp \
    source/DATAWQueryEILimits.cpp \
    source/DATAWQueryEISupported.cpp \
    source/DATAWQueryExtendedInfo.cpp \
    source/DATAWQueryExtendedInfoItem.cpp \
    source/DATAWQueryFolder.cpp \
    source/DATAWQueryForeignKeys.cpp \
    source/DATAWQueryForeignKey.cpp \
    source/DATAWQueryIndex.cpp \
    source/DATAWQueryIndexs.cpp \
    source/DATAWQueryPrimaryKeys.cpp \
    source/DATAWQuerySchema.cpp \
    source/DATAWQuerySpecialColumns.cpp \
    source/DATAWQuerySystem.cpp \
    source/DATAWQueryTable.cpp \
    source/DATAWQueryTableType.cpp \
    source/DATAWSql.cpp \
    source/DATAWSqlEditorWidget.cpp \
    source/DATAWTableModel.cpp \
    source/DATAWTableModelApp.cpp \
    source/DATAWTableModelODBC.cpp \
    source/DATAWTableView.cpp \
    source/DATAWTableWidget.cpp


