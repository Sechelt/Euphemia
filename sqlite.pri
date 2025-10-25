# ################################################################ 
#
# sqlite
#
# UNIX - assumes prebuilt package installed
# WIN - assumes sqlite3 source was built in SDK\sqlite and no 'nmake install' to move/copy the files
# ################################################################ 
unix {
	LIBS += -lsqlite3
}
else {
	INCLUDEPATH += c:\SandBox\svn.codebydesign.com\Euphemia\trunk\SDK\sqlite
	LIBS += -Lc:\SandBox\svn.codebydesign.com\Euphemia\trunk\SDK\sqlite
	LIBS += -lsqlite3
}



