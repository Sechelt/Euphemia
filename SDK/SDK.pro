# ################################################################ 
# SDK
# =======
#
# Components are built based upon the main config.pri.
#
# QtProperties was an experiment to use for the properties widget.
# It was a bit of work so it lingers until 100% not needed.
#
# ################################################################

include(../Euphemia.pri)

TEMPLATE    = subdirs

SUBDIRS +=  CBD \
            W \
            S \
            DATA \
            AD \
            AW \
            R \
            L \
            PA \
            DR \
            D \
            ORG \
            FL \
            DATAW \
            ER \
            FR

