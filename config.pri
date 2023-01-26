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

# CONFIG	+= thread warn_on release
CONFIG	+= thread warn_on debug
CONFIG 	+= c++11

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
QT += qml
QT += svg
lessThan(QT_MAJOR_VERSION, 6) {
	QT += multimedia
}
greaterThan(QT_MAJOR_VERSION, 5) {
	QT += openglwidgets
}


