Globals - some are 'singletons' 
===============================

SSettings 	AS	g_SSettings	: Provides context for such things as; pen, brush, and font.
SColors 	AS	g_SColors	: Provides access to a configurable color palette.

Some globals that show up elsewhere...

ADObject 	AS	g_Root		: Root ADObject in an AD based model.
ADTransaction	AS 	g_Transaction 	: Transaction manager for an ADObject (usually a AWFile).
AWPreferences 	AS	gPreferences	: Preferences based upon a QSettings file. Alt. to SSettings. This lacks signals.

