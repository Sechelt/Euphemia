#include "LibInfo.h"
#include "ODBCExtModel.h"

#include <AWClassNameWidget.h>

#include <DATAProfiles.h>
#include <DATAProfile.h>

//
// \todo Handle nSQLDataType for date/time intervals
//

ODBCExtModel::ODBCExtModel( ADObject *pParent )
    : ODBCCorModel( pParent )
{
    // configure object factory
    // - add more classes

    // NOTHING MORE

    stringDatabaseType = "ODBCExt";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

ODBCExtModel::~ODBCExtModel()
{
}

AWClassWidget *ODBCExtModel::getClassWidget( QWidget *pWidgetParent )
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
            stringText += "</TABLE>";
            stringText += "<TABLE border=1>";
            stringText += "<TR><TD>" + tr("Conformance Mechanism:") + "</TD><TD>" + tr("Open Database Connectivty Specification") + "</TD></TR>";
            stringText += "<TR><TD>" + tr("Conformance: ") + "</TD><TD>" + tr("v3.8 - Extended Grammar") + "</TD></TR>";
            stringText += "<TR><TD>" + tr("Call Level Interface: ") + "</TD><TD>" + tr("ODBC v3.8") + "</TD></TR>";
            stringText += "</TABLE>";
            stringText += "<HR>";
            stringText += "<BR><BR>" + AWClassNameWidget::getProductInfo();
            stringText += "<BR><BR>" + AWClassNameWidget::getCompanyInfo();
            pClassNameWidget->setText( stringText );
        }
    }

    return p;
}

