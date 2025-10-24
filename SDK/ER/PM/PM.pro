include(../../../Euphemia.pri)

TEMPLATE    = subdirs

# ############################################################
# Physical Models
#
# Base classes...
#
#   PM	        - base
#
# Standards we like...
#
#   SQL92       - SQL92  
#   ODBC 	    - ODBC
#
# Vendor specific... 
#
#   MSA         - Microsoft Access
#   SL          - SQLite
#   MY 	        - MySQL
#   MS          - MS SQL
#   OR          - Oracle
#   DB2         - DB2
#   PS          - Postgres
#
# ############################################################
SUBDIRS +=  PM \
            SQL92 \
            ODBC \
            MSA \
            SL


