# #########################################################
# COMMON
# #########################################################
TEMPLATE    = lib
TARGET      = FR
DESTDIR     = ../lib

include(../../Euphemia.pri)

CONFIG      += staticlib

INCLUDEPATH += ./include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include
# INCLUDEPATH += ../QtProperties/include
INCLUDEPATH += ../W/include
INCLUDEPATH += ../AD/include
INCLUDEPATH += ../AW/include
INCLUDEPATH += ../R/include
INCLUDEPATH += ../D/include

# #########################################################
# FILES
# #########################################################
RESOURCES   = FR.qrc

HEADERS += \
        source/LibInfo.h \
        include/FRCalendarWidget.h \
        include/FRCheckBox.h \
        include/FRComboBox.h \
        include/FRDateEdit.h \
        include/FRDateTimeEdit.h \
        include/FRDial.h \
        include/FRDialogButtonBox.h \
        include/FRDoubleSpinBox.h \
        include/FRForm.h \
        include/FRFormEditorWidget.h \
        include/FRFormPalWidget.h \
        include/FRFrame.h \
        include/FRGraphicsProxyObject.h \
        include/FRGroupBox.h \
        include/FRHorizontalScrollBar.h \
        include/FRHorizontalSlider.h \
        include/FRLabel.h \
        include/FRLCDNumber.h \
        include/FRLineEdit.h \
        include/FRListWidget.h \
        include/FRObject.h \
        include/FROpenGLWidget.h \
        include/FRPlainTextEdit.h \
        include/FRProgressBar.h \
        include/FRPushButton.h \
        include/FRRadioButton.h \
        include/FRScrollArea.h \
        include/FRSpinBox.h \
        include/FRStackedWidget.h \
        include/FRTableWidget.h \
        include/FRTabWidget.h \
        include/FRTextBrowser.h \
        include/FRTextEdit.h \
        include/FRTimeEdit.h \
        include/FRToolBox.h \
        include/FRToolButton.h \
        include/FRTreeWidget.h \
        include/FRVerticalScrollBar.h \
        include/FRVerticalSlider.h

SOURCES += \
        source/FRCalendarWidget.cpp \
        source/FRCheckBox.cpp \
        source/FRComboBox.cpp \
        source/FRDateEdit.cpp \
        source/FRDateTimeEdit.cpp \
        source/FRDial.cpp \
        source/FRDialogButtonBox.cpp \
        source/FRDoubleSpinBox.cpp \
        source/FRForm.cpp \
        source/FRFormEditorWidget.cpp \
        source/FRFormPalWidget.cpp \
        source/FRFrame.cpp \
        source/FRGraphicsProxyObject.cpp \
        source/FRGroupBox.cpp \
        source/FRHorizontalScrollBar.cpp \
        source/FRHorizontalSlider.cpp \
        source/FRLabel.cpp \
        source/FRLCDNumber.cpp \
        source/FRLineEdit.cpp \
        source/FRListWidget.cpp \
        source/FRObject.cpp \
        source/FROpenGLWidget.cpp \
        source/FRPlainTextEdit.cpp \
        source/FRProgressBar.cpp \
        source/FRPushButton.cpp \
        source/FRRadioButton.cpp \
        source/FRScrollArea.cpp \
        source/FRSpinBox.cpp \
        source/FRStackedWidget.cpp \
        source/FRTableWidget.cpp \
        source/FRTabWidget.cpp \
        source/FRTextBrowser.cpp \
        source/FRTextEdit.cpp \
        source/FRTimeEdit.cpp \
        source/FRToolBox.cpp \
        source/FRToolButton.cpp \
        source/FRTreeWidget.cpp \
        source/FRVerticalScrollBar.cpp \
        source/FRVerticalSlider.cpp
