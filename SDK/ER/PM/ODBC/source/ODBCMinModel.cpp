#include "LibInfo.h"
#include "ODBCMinModel.h"

#include <AWClassNameWidget.h>

#include <DATAProfiles.h>
#include <DATAProfile.h>

ODBCMinModel::ODBCMinModel( ADObject *pParent )
    : ODBCModel( pParent )
{
    stringDatabaseType = "ODBCMin";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

ODBCMinModel::~ODBCMinModel()
{
}

AWClassWidget *ODBCMinModel::getClassWidget( QWidget *pWidgetParent )
{
    AWClassWidget *p = ODBCModel::getClassWidget( pWidgetParent );

    // ODBC Grammar: Minimum/Core/Extended
    // CLI: Open Group/ISO92/ODBC Built with: ODBC v3.8 Core/Level1/Level2

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
            stringText += "<TR><TD>" + tr("Conformance: ") + "</TD><TD>" + tr("v3.8 - Minimum Grammar") + "</TD></TR>";
            stringText += "<TR><TD>" + tr("Call Level Interface: ") + "</TD><TD>" + tr("Open Group/ISO92/ODBC v3.8 Core/Level1/Level2") + "</TD></TR>";
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

