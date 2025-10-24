include(../Euphemia.pri)

TEMPLATE = subdirs

SUBDIRS += Widgets

#
# Route - test line routing
#
SUBDIRS += Route

#
# Blue - basic painting and drawing
# - support for layers where each layer is either 'paint' or 'draw'
# - support for paint only as common bit map file formats
# - support for draw only as XML (no svg parse at the moment)
# - support for layers as XML
# - layers can be merged to a single, paint, layer for save as bit map instead of XML
#
SUBDIRS += Blue

#
# DataArchitect - advanced data management
# - reverse and forward engineer using LM (hub) and PM (spoke)
# - validate
# - SQL editor
# - table browse
#
SUBDIRS += DataArchitect
