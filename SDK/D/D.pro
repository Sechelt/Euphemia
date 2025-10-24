# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = D
DESTDIR     = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH += ../W/include
INCLUDEPATH += ../S/include
INCLUDEPATH += ../AD/include
INCLUDEPATH += ../AW/include
INCLUDEPATH += ../R/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = D.qrc

HEADERS += \
        include/DArc.h \
        include/DBackgroundWidget.h \
        include/DBitmap.h \
        include/DBitmapWidget.h \
        include/DCallOut.h \
        include/DCallOutWidget.h \
        include/DDiagram.h \
        include/DDiagramEditorWidget.h \
        include/DDiagramPalWidget.h \
        include/DDiagramPanWidget.h \
        include/DEllipse.h \
        include/DFile.h \
        include/DGraphicsItemLine.h \
        include/DGraphicsProxyItem.h \
        include/DGraphicsSceneProxy.h \
        include/DGraphicsView.h \
        include/DGroup.h \
        include/DHandle.h \
        include/DLayout.h \
        include/DLayoutGrid.h \
        include/DLayoutHorizontal.h \
        include/DLayoutLinear.h \
        include/DLayoutVertical.h \
        include/DLine.h \
        include/DLine90.h \
        include/DLine90Auto.h \
        include/DLine90H.h \
        include/DLine90V.h \
        include/DLineDynamic.h \
        include/DLinePoly.h \
        include/DLinePolyAuto.h \
        include/DLineWidget.h \
        include/DObject.h \
        include/DPanner.h \
        include/DPointsWidget.h \
#        include/DPrefDiagram.h \
        include/DRectangle.h \
        include/DRectangleBase.h \
        include/DRectangleWidget.h \
        include/DRouteSource.h \
        include/DRubberBand.h \
        include/DSink.h \
        include/DSinksWidget.h \
        include/DSource.h \
        include/DSourcesWidget.h \
        include/DSpacer.h \
        include/DSpacerHorizontal.h \
        include/DSpacerVertical.h \
        include/DSpacerWidget.h \
        include/DStencil.h \
        include/DStencilItem.h \
        include/DStencils.h \
        include/DStencilsWidget.h \
        include/DText.h \
        include/DTitle.h \ 
        include/DTitleDoc.h \ 
        include/DVector.h \
        include/DVectorWidget.h

SOURCES += \
        source/DArc.cpp \
        source/DBackgroundWidget.cpp \
        source/DBitmap.cpp \
        source/DBitmapWidget.cpp \
        source/DCallOut.cpp \
        source/DCallOutWidget.cpp \
        source/DDiagram.cpp \
        source/DDiagramEditorWidget.cpp \
        source/DDiagramPalWidget.cpp \
        source/DDiagramPanWidget.cpp \
        source/DEllipse.cpp \
        source/DFile.cpp \
        source/DGraphicsItemLine.cpp \
        source/DGraphicsProxyItem.cpp \
        source/DGraphicsSceneProxy.cpp \
        source/DGraphicsView.cpp \
        source/DGroup.cpp \
        source/DHandle.cpp \
        source/DLayout.cpp \
        source/DLayoutGrid.cpp \
        source/DLayoutHorizontal.cpp \
        source/DLayoutLinear.cpp \
        source/DLayoutVertical.cpp \
        source/DLine.cpp \
        source/DLine90.cpp \
        source/DLine90Auto.cpp \
        source/DLine90H.cpp \
        source/DLine90V.cpp \
        source/DLineDynamic.cpp \
        source/DLinePoly.cpp \
        source/DLinePolyAuto.cpp \
        source/DLineWidget.cpp \
        source/DObject.cpp \
        source/DPanner.cpp \
        source/DPointsWidget.cpp \
#        source/DPrefDiagram.cpp \
        source/DRectangle.cpp \
        source/DRectangleBase.cpp \
        source/DRectangleWidget.cpp \
        source/DRouteSource.cpp \
        source/DRubberBand.cpp \
        source/DSink.cpp \
        source/DSinksWidget.cpp \
        source/DSource.cpp \
        source/DSourcesWidget.cpp \
        source/DSpacer.cpp \
        source/DSpacerHorizontal.cpp \
        source/DSpacerVertical.cpp \
        source/DSpacerWidget.cpp \
        source/DStencil.cpp \
        source/DStencilItem.cpp \
        source/DStencils.cpp \
        source/DStencilsWidget.cpp \
        source/DText.cpp \
        source/DTitle.cpp \
        source/DTitleDoc.cpp \
        source/DVector.cpp \
        source/DVectorWidget.cpp \
        source/LibInfo.h


