# ################################################################ 
# CONFIG
# =======
#
# These are config options which are common among all sources.
# 
# ################################################################

# ################################################################ 
#
# Development or release?
#
# ################################################################ 

CONFIG	+= thread
CONFIG	+= warn_on
CONFIG	+= debug
# CONFIG	+= release
# CONFIG 	+= c++11
CONFIG 	+= sqlite

unix {
	macx {
		#
		# Enable qDebug to console on OSX
		#
		contains( CONFIG, "debug" ) {
			CONFIG -= app_bundle
		}
	}
}

win32 {
	#
	# Enable qDebug to console on MS Windows
	# - we do not want this in a release as it forces use of a console
	#
	contains( CONFIG, "debug" ) {
		CONFIG += console
	}
}

# ################################################################ 
#
# Qt modules we want to use.
#
# ################################################################ 
QT += widgets
QT += printsupport
QT += xml
QT += svg
QT += openglwidgets
QT += concurrent

#
# SQL
# - tried using Qt SQL but decided to use ODBC directly (via DATA & DATAW)
# - we use our own model (interfacing Qt & ODBC) to allow same use of Qt views
#
QT += sql

# ################################################################ 
#
# Scintilla
#
# UNIX - assumes prebuilt package installed
# WIN - assumes qscintilla source was built in SDK\qscintilla 
# ################################################################ 
# unix {
# 	CONFIG += qscintilla2
# 	LIBS += -lqscintilla2_qt6
# }
# else {
# 	CONFIG += qscintilla2
# 	INCLUDEPATH += c:\SandBox\svn.codebydesign.com\Lab6\trunk\SDK\qscintilla\src
# 	LIBS += -Lc:\SandBox\svn.codebydesign.com\Lab66\trunk\SDK\qscintilla\src\release
# 	LIBS += -lqscintilla2_qt6
# }



