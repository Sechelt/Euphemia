#include "LibInfo.h"
#include "ODBCDrvModel.h"

#include <AWClassNameWidget.h>

#include <DATAProfiles.h>
#include <DATAProfile.h>

ODBCDrvModel::ODBCDrvModel( ADObject *pParent )
    : ODBCModel( pParent )
{
    // we start off knowing nothing - then we are either load from XML or we connect and load from the connection
    pProfile = nullptr;
    mapMeta.clear();
}

ODBCDrvModel::~ODBCDrvModel()
{
}

AWClassWidget *ODBCDrvModel::getClassWidget( QWidget *pWidgetParent )
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
            stringText += "<TR><TD>" + tr("Conformance Mechanism:") + "</TD><TD>" + tr("ODBC Driver/Data Source Defined") + "</TD></TR>";
            if ( pProfile )
                stringText += "<TR><TD>" + tr("Conformance: ") + "</TD><TD>" + (pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->getSupported()->vValue.isNull() ? tr("UNKNOWN") : pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->getSupported()->stringInfo) + tr(" May exhibit more; see Info and Data Types.") + "</TD></TR>";
            else
                stringText += "<TR><TD>" + tr("Conformance: ") + "</TD><TD></TD></TR>";
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

QMenu *ODBCDrvModel::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = ODBCModel::getContextMenu( pParent );

    if ( isEditor() )
    { 
        pMenu->addAction( QPixmap( ":W/Connected48x48" ), tr( "Query For Features" ), this, SLOT( slotQueryForFeatures() ) );
    }

    return pMenu; // caller invokes and then deletes
}

QDomElement ODBCDrvModel::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = ODBCModel::doSave( pdomDoc, pdomElemParent );

    // same profile in xml 
    if ( pProfile )
    {
        domElemThis.setAttribute( "ProfileKey", pProfile->stringKey );
        pProfile->doSave( pdomDoc, &domElemThis );
    }

    // classes
    {
        QDomElement domElem = pdomDoc->createElement( "ClassNames" );

        for ( auto i = mapMeta.cbegin(), end = mapMeta.cend(); i != end; ++i )
        {
            domElem.setAttribute( i.key(), i.value().stringFriendlyName );
        }
        domElemThis.appendChild( domElem );
    }

    return domElemThis;
}

bool ODBCDrvModel::doLoad( QDomElement *pdomElemThis )
{
    // create a profile
    pProfile = nullptr;
    mapMeta.clear();
    QString stringProfileKey = pdomElemThis->attribute( "ProfileKey" );
    if ( !stringProfileKey.isEmpty() ) pProfile = g_Profiles->getProfile( stringProfileKey, true );

    //
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Profile" && pProfile ) pProfile->doLoad( &domElem );
        else if ( domElem.tagName() == "ClassNames" ) doLoadClasses( &domElem );

        domNode = domNode.nextSibling();
    }

    // unlike most cases - we call for default after...
    // this sets things up for the object factory
    ODBCModel::doLoad( pdomElemThis );

    return true;
}

bool ODBCDrvModel::doLoadClasses( QDomElement *pdomElem )
{
    QDomNamedNodeMap NodeMap = pdomElem->attributes();
    for ( int n = 0; n < NodeMap.count(); n++ )
    {
        QDomNode Node = NodeMap.item( n );
        QDomAttr Attr = Node.toAttr();
        if ( Attr.isNull() ) continue;
        if ( Attr.name().isEmpty() ) continue;

        mapMeta.insert( Attr.name(), ADMeta( QIcon(), Attr.name(), Attr.value() ) );
    }

    return true;
}

bool ODBCDrvModel::slotQueryForFeatures()
{
    pProfile = nullptr;
    mapMeta.clear();

    if ( !isDataConnected() )
    {
        doDataConnect();
        if ( !isDataConnected() )
        {
            // we are useless without a connection
            doMessageBox( "WARNING", tr( "ODBC Driver Model" ) , tr( "Failed to connect. This model has no useful features without a connection." ) );
            return false; 
        }
    }

    // the connection set its profile (creating/loading) - unlike other models (which create/use a standard profile) we adopt the one in the connection
    pProfile = pConnection->getProfile();
    Q_ASSERT( pProfile );

    // the profile sticks around in a cache so we can use it even after connection disconnected
    doDataDisconnect();

    // Do we know the SQL92 conformance level?
    if ( pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->mapValues.count() )
    {
        // Ok. We may be able to specify additional features. Some from ODBC and some from SQL92 specification.
        // Use SQL92 prefix for stuff added from specification. ie mapInfo["SQL92_NEWDATATYPE"]  
        if ( pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->isSupported( "SQL_SC_SQL92_ENTRY" ) )
        {
        }
        else if ( pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->isSupported( "SQL_SC_FIPS127_2_TRANSITIONAL" ) )
        {
        }
        else if ( pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->isSupported( "SQL_SC_SQL92_FULL" ) )
        {
        }
        else if ( pProfile->mapInfo["SQL_SQL_CONFORMANCE"]->isSupported( "SQL_SC_SQL92_INTERMEDIATE" ) )
        {
        }
    }

    // configure object factory
    // - only support the classes that are needed for this compliance level
    // - ODBCReference is added to make the diagram useful. The DDL may not include anything for it.
    // - no way to determine, from ODBC, if tables can inherit so not supported here
    if ( hasFeature( PMModel::FeatureTable, PMModel::FeatureSupportUse ) )
    {
        mapMeta.insert( "ODBCTable", ADMeta( QIcon(), "ODBCTable", "Table" ) );
    }

    if ( hasFeature( PMModel::FeatureReference, PMModel::FeatureSupportUse ) )
    {
        mapMeta.insert( "ODBCReference", ADMeta( QIcon(), "ODBCReference", "Reference" ) );
    }

    // views?
    if ( hasFeature( PMModel::FeatureView, PMModel::FeatureSupportUse ) )
    { 
        mapMeta.insert( "ODBCView", ADMeta( QIcon(), "ODBCView", "View" ) );
    }

    // domains?
    if ( hasFeature( PMModel::FeatureDomain, PMModel::FeatureSupportUse ) )
    { 
        mapMeta.insert( "ODBCDomain", ADMeta( QIcon(), "ODBCDomain", "Domain" ) );
    }

    // procedures?
    if ( hasFeature( PMModel::FeatureProcedure, PMModel::FeatureSupportUse ) )
    { 
        mapMeta.insert( "ODBCProcedure", ADMeta( QIcon(), "ODBCProcedure", "Procedure" ) );
    }

    return true;
}


