#
# ODBC version we want.
# Lets build for this version of the ODBC specification.
#
# DEFINES += ODBCVER=0x0300
# DEFINES += ODBCVER=0x0350
DEFINES += ODBCVER=0x0380

#
# Turn UNICODE on. 
# Calls will be silently mapped to W (wide char) versions where applicable.
#
DEFINES += UNICODE

#
# Turn UNICODE auto mapping off.
# In this case you have to call the version you want explicitly.
# \note Possibly unixODBC specific
#	   
# DEFINES += SQL_NOUNICODEMAP

#
# Development files.
#
unix {

	macx {
		LIBS		+= -liodbc
	}
	else {
		#INCLUDEPATH	+= $(UNIXODBC_DIR)/include 
		#LIBS		+= -L$(UNIXODBC_LIBDIR)
		LIBS		+= -L/usr/lib/x86_64-linux-gnu
		LIBS		+= -lodbc -lodbcinst
	}
}


#
# mingw
# - ODBC
#	The odbc32.dll/odbccp32.dll does not come in mingw libs so we copy the dll from System32 into the mingw lib dir.
#	We DO NOT want to add System32 to the link path because the linker will go after other stuff in there and confuse the toolchain.
#
win32 {
	# 32-bit
	# LIBS		+= -LC:\Windows\SysWOW64

	# 64-bit
	# LIBS		+= -LC:\Windows\System32

	# Default path will be the mingw lib dir - not the Windows\Sys* dirs.
	# But we have copied these in there so it should be sufficient.
	LIBS		+= -lodbc32  -lodbccp32
}




