# #########################################################
# DESCRIPTION
#
# The primary purpose of this library is to provide working storage for an application.
# The storage is an object hierarchy with, as needed, object references. 
# 
# This library also provides the basis for;
#
# - object validation... doValidate()
# - object selection manager... getSelectionManager()
# - object persistence... doLoad(), doSave()
# - object factory... getObject()
# - object messages... doMessage(), doMessageBox()
# - object data... getName(), getCode(), getDescription()
# - navigation... getObject(), doFind()
#
# #########################################################

# #########################################################
# COMMON
# #########################################################
TARGET      = AD
DESTDIR		= ../lib
TEMPLATE    = lib

include(../../Euphemia.pri)

CONFIG		+= staticlib

INCLUDEPATH	+= include
INCLUDEPATH	+= ../include
INCLUDEPATH	+= ../CBD/include

# #########################################################
# FILES
# #########################################################
HEADERS	+= \
        source/LibInfo.h \
        include/AD.h \
        include/ADDocument.h \
        include/ADExport.h \
	    include/ADObject.h \
        include/ADObjectPath.h \
        include/ADObjectReference.h \
        include/ADObjectSelectionManager.h \
        include/ADTransaction.h \
        include/ADValidation.h

SOURCES	+= \
        source/ADDocument.cpp \
	    source/ADObject.cpp \
        source/ADObjectPath.cpp \
        source/ADObjectReference.cpp \
        source/ADObjectSelectionManager.cpp \
        source/ADTransaction.cpp \
        source/ADValidation.cpp

