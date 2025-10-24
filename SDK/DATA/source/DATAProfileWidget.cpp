#include "LibInfo.h"
#include "DATAProfileWidget.h"

#include "DATAProfile.h"
#include "DATADataTypesWidget.h"
#include "DATAInfoWidget.h"
#include "DATAFunctionsWidget.h"
#include "DATAAttrWidget.h"

DATAProfileWidget::DATAProfileWidget( DATAProfile *pProfile, QWidget *pParent )
    : QTabWidget( pParent )
{
    addTab( new DATADataTypesWidget( pProfile, this ), tr( "Data Types" ) );
    addTab( new DATAInfoWidget( pProfile, this ), tr( "Info" ) );
    addTab( new DATAFunctionsWidget( pProfile, this ), tr( "Functions" ) );
    addTab( new DATAAttrWidget( pProfile, this ), tr( "Attributes" ) );
}


