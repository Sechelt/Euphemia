# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = W
DESTDIR		= ../lib

include(../../Euphemia.pri)

CONFIG		+= staticlib

INCLUDEPATH	+= ./include

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
INCLUDEPATH	+= ../S/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = W.qrc

HEADERS	+= \
        source/LibInfo.h \
        include/W.h \
        include/WBitmapButton.h \
        include/WBitmapWidget.h \
        include/WBrush.h \
        include/WBrushStyleComboBox.h \
        include/WCanvasSizeWidget.h \
        include/WColorButton.h \
        include/WColorsPickerDialog.h \
        include/WColorsPickerWidget.h \
        include/WColorsWidget.h \
        include/WCoordWidget.h \
        include/WDataNavWidget.h \
        include/WDirectiveWidget.h \
        include/WDirSelectWidget.h \
        include/WFileSelectWidget.h \
        include/WFindTextDialog.h \
        include/WFindTextWidget.h \
        include/WFontButton.h \
        include/WFontColorButton.h \
        include/WFontColorDialog.h \
        include/WFontColorWidget.h \
        include/WGeometryWidget.h \
        include/WIconLayout.h \
        include/WIconWidget.h \
        include/WImageButton.h \
        include/WLabelButton.h \
        include/WLayoutWidget.h \
        include/WLineEndComboBox.h \
        include/WLineStyleComboBox.h \
        include/WMarginsWidget.h \
        include/WObjectHAlignComboBox.h \
        include/WObjectVAlignComboBox.h \
        include/WPasswordWidget.h \
        include/WPen.h \
        include/WPosWidget.h \
        include/WRulerWidget.h \
        include/WScratchTool.h \
        include/WScriptEditor.h \
        include/WShapeSizeComboBox.h \
        include/WSizeConstraintsWidget.h \
        include/WSizeDialog.h \
        include/WSizeHintWidget.h \
        include/WSizeWidget.h \
        include/WSplash.h \
        include/WStackedWidget.h \
        include/WStretchWidget.h \
        include/WSvgButton.h \
        include/WTabDialog.h \
        include/WText.h \
        include/WTextVAlignComboBox.h \
        include/WTextHAlignComboBox.h \
        include/WValueListWidget.h \
        include/WZoomWidget.h

SOURCES	+= \
        source/WBitmapButton.cpp \
        source/WBitmapWidget.cpp \
        source/WBrush.cpp \
        source/WBrushStyleComboBox.cpp \
        source/WCanvasSizeWidget.cpp \
        source/WColorButton.cpp \
        source/WColorsPickerDialog.cpp \
        source/WColorsPickerWidget.cpp \
        source/WColorsWidget.cpp \
        source/WCoordWidget.cpp \
        source/WDataNavWidget.cpp \
        source/WDirectiveWidget.cpp \
        source/WDirSelectWidget.cpp \
        source/WFileSelectWidget.cpp \
        source/WFindTextDialog.cpp \
        source/WFindTextWidget.cpp \
        source/WFontButton.cpp \
        source/WFontColorButton.cpp \
        source/WFontColorDialog.cpp \
        source/WFontColorWidget.cpp \
        source/WGeometryWidget.cpp \
        source/WIconLayout.cpp \
        source/WIconWidget.cpp \
        source/WImageButton.cpp \
        source/WLabelButton.cpp \
        source/WLayoutWidget.cpp \
        source/WLineEndComboBox.cpp \
        source/WLineStyleComboBox.cpp \
        source/WMarginsWidget.cpp \
        source/WObjectHAlignComboBox.cpp \
        source/WObjectVAlignComboBox.cpp \
        source/WPasswordWidget.cpp \
        source/WPen.cpp \
        source/WPosWidget.cpp \
        source/WRulerWidget.cpp \
        source/WScratchTool.cpp \
        source/WScriptEditor.cpp \
        source/WShapeSizeComboBox.cpp \
        source/WSizeConstraintsWidget.cpp \
        source/WSizeDialog.cpp \
        source/WSizeHintWidget.cpp \
        source/WSizeWidget.cpp \
        source/WSplash.cpp \
        source/WStackedWidget.cpp \
        source/WStretchWidget.cpp \
        source/WSvgButton.cpp \
        source/WTabDialog.cpp \
        source/WText.cpp \
        source/WTextVAlignComboBox.cpp \
        source/WTextHAlignComboBox.cpp \
        source/WValueListWidget.cpp \
        source/WZoomWidget.cpp

