#include "LibInfo.h"
#include "ODBCCorModel.h"

#include <AWClassNameWidget.h>

#include <DATAProfiles.h>
#include <DATAProfile.h>

ODBCCorModel::ODBCCorModel( ADObject *pParent )
    : ODBCMinModel( pParent )
{
    // configure object factory
    // - add more classes
    // - index comes in here but it gets picked up by ODBCTable 
    mapMeta.insert( "ODBCView", ADMeta( QIcon(), "ODBCView", "View" ) );

    stringDatabaseType = "ODBCCor";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

ODBCCorModel::~ODBCCorModel()
{
}

AWClassWidget *ODBCCorModel::getClassWidget( QWidget *pWidgetParent )
{
    AWClassWidget *p = ODBCModel::getClassWidget( pWidgetParent );

    for ( int n = 0; n < p->count(); n++ )
    {
        QWidget *pWidget = p->widget( n );
        if ( pWidget->inherits( "AWClassNameWidget" ) )
        {
            AWClassNameWidget *pClassNameWidget = (AWClassNameWidget*)pWidget;
            QString stringText;
            stringText += "<TABLE>";
            stringText += "<TR><TD>" + QStringLiteral("<img src=':PhysicalModel48x48'>") + "</TD><TD>" + tr("Entity Relationships - Physical Model") + "</TD></TR>";
            stringText += "</TABLE border=1>";
            stringText += "<TABLE>";
            stringText += "<TR><TD>" + tr("Conformance Mechanism:") + "</TD><TD>" + tr("Open Database Connectivty Specification") + "</TD></TR>";
            stringText += "<TR><TD>" + tr("Conformance: ") + "</TD><TD>" + tr("v3.8 - Core Grammar") + "</TD></TR>";
            stringText += "<TR><TD>" + tr("Call Level Interface: ") + "</TD><TD>" + tr("ODBC v3.8") + "</TD></TR>";
            stringText += "</TABLE>";
            stringText += "<HR>";
            stringText += "<BR><BR>" + AWClassNameWidget::getProductInfo();
            stringText += "<BR><BR>" + AWClassNameWidget::getCompanyInfo();
            pClassNameWidget->setText( stringText );
            break;
        }
    }

    return p;
}

