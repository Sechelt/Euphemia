#
# \brief	These are misc options which are common among all 
#		sources. 
#
OBJECTS_DIR			= .tmp
UI_DIR          		= .tmp
MOC_DIR		        	= .tmp
LANGUAGE			= C++
CPP_ALWAYS_CREATE_SOURCE	= TRUE

# QMAKE_CXXFLAGS -= -Wint-to-pointer-cast
# QMAKE_CXXFLAGS -= -Wall
# QMAKE_CXXFLAGS += -fms-extensions

#
# zlib
#
# Linux - install it with package manager and it will go to standard locations
#	- COMPILE: include <zlib.h>
#	- LINK: -lz
#
# Win mingw - mingw comes with a zlib dev and Qt will use it 
#
# Win VS    - No system zlib but Qt uses one internally.
#           - add Qt's QtZlib to INCLUDEPATH
#           - COMPILE: include <QtZlib/zlib.h>
#           - LINK:
#
# NOTE: No reason to use mingw as VS can be had for free.
# NOTE: Get rid of this problem and use qCompress/qUncompress.

!win32 {
# On Linux, perhaps others (not Windows), we need to link to this compression library.
# Having this here works on openSuSE but not on kubuntu. On kubuntu this needs to at the end of the link info so added in the app pro file.
#    LIBS += -lz
}

win32 {
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtZlib

#   This to resolve a legacy variation of printf used by ODBC. 
#   This is to resolve legacy MessageBoxW used by ODBC.	
    LIBS += -llegacy_stdio_definitions -luser32
}



