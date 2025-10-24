TARGET      = da
DESTDIR     = ../../bin
TEMPLATE    = app

include(../../Euphemia.pri)

INCLUDEPATH += ../../SDK/ER/PM/SL/include
INCLUDEPATH += ../../SDK/ER/PM/MSA/include
INCLUDEPATH += ../../SDK/ER/PM/ODBC/include
INCLUDEPATH += ../../SDK/ER/PM/SQL92/include
INCLUDEPATH += ../../SDK/ER/PM/PM/include
INCLUDEPATH += ../../SDK/ER/LM/include
INCLUDEPATH += ../../SDK/DATAW/include
INCLUDEPATH += ../../SDK/DATA/include
INCLUDEPATH += ../../SDK/FL/include
INCLUDEPATH += ../../SDK/ORG/include
INCLUDEPATH += ../../SDK/FR/include
INCLUDEPATH += ../../SDK/D/include
INCLUDEPATH += ../../SDK/R/include
INCLUDEPATH += ../../SDK/AW/include
INCLUDEPATH += ../../SDK/AD/include
INCLUDEPATH += ../../SDK/S/include
INCLUDEPATH += ../../SDK/W/include
# INCLUDEPATH += ../../SDK/QtProperties/include
INCLUDEPATH += ../../SDK/CBD/include
INCLUDEPATH += ../../SDK/include
INCLUDEPATH += ../../SDK
INCLUDEPATH += ./include

LIBS += -L../../SDK/lib 

LIBS += -lSL
LIBS += -lMSA
LIBS += -lODBC
LIBS += -lSQL92
LIBS += -lPM
LIBS += -lLM
LIBS += -lDATAW
LIBS += -lDATA
LIBS += -lFL
LIBS += -lORG
LIBS += -lFR
LIBS += -lD
LIBS += -lR
LIBS += -lAW -lAD -lS -lW -lCBD
# LIBS += -lQtProperties

#
# These are in Euphemia.pri but we add these again so we have the libs at the end of the list of libs.
#
include(../../sqlite.pri)
include(../../odbc.pri)

!win32 {
        LIBS += -lz
}

RESOURCES   = DataArchitect.qrc

HEADERS += \
        source/AppInfo.h \
#        include/SAAccountConnectSplash.h \
        include/SAConnection.h \
        include/SAMainWindow.h \
        include/SANewDialog.h \
        include/SAOutputWidget.h \
        include/SAPreferencesDialog.h \
        include/SAFile.h 

SOURCES += \
        source/main.cpp \
#        source/SAAccountConnectSplash.cpp \
        source/SAConnection.cpp \
        source/SAMainWindow.cpp \
        source/SANewDialog.cpp \
        source/SAOutputWidget.cpp \
        source/SAPreferencesDialog.cpp \
        source/SAFile.cpp


