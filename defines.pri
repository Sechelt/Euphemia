# ################################################################ 
# DEFINES
# =======
#
# Code
# ----
#
# Anything set with "DEFINES" will be added to the compile line with a -D. For example;
#
# DEFINES += CBD_LM
#
# Will result in a "-DCBD_LM" on the compile line. This can then be used in the source as follows;
#
# #ifdef CBD_LM
# 	#include "LMModel.h"
# #endif
#
# ################################################################ #

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#
# Version. This is used by the build system (dynamic library version for example) and should be consistent
# with the version information in the source include files.
#
VERSION=0.0.1



