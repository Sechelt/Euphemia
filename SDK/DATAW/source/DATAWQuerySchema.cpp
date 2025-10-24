#include "LibInfo.h"
#include "DATAWQuerySchema.h"

// #include <stdio.h>

#include <DATAConnection.h>

#include "DATAWQueryTableType.h"

DATAWQuerySchema::DATAWQuerySchema( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog )
    : DATAWQuery( pParent )
{
    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    stringComment = tr( "All schemas (namespaces) within the catalog." );

    mapMeta.insert( "DATAWQueryTableType", ADMeta( QIcon(), "DATAWQueryTableType", "TableType" ) );
}

DATAWQuerySchema::~DATAWQuerySchema()
{
}

QPixmap DATAWQuerySchema::getIcon( AWObject::enumIconRoles nRole )
{
    switch ( nRole )
    {
    case nIconRoleExpanded:
        return QPixmap( ":DATAW/FolderOpened" );
    case nIconRoleCollapsed:
        return QPixmap( ":DATAW/FolderClosed" );
    }
    return getIcon();
}

QPixmap DATAWQuerySchema::getIcon()
{
    return QPixmap( ":DATAW/FolderClosed" );
}

ADObject *DATAWQuerySchema::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryTableType" )        
        p = new DATAWQueryTableType( this, pConnection, stringCatalog, (getName()=="SCHEMA"?"":getName()) );           

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );

    return p;
}

void DATAWQuerySchema::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    // driver provided a list of supported table types?
    QStringList l = pConnection->getTableTypes();
    if ( l.count() )
    {
        // use driver supplied list
        QString stringTableType;
        foreach( stringTableType, l )
        {
            ADObject *p = getObject( "DATAWQueryTableType" );
            p->setName( stringTableType );
            p->setModified( false );
        }
    }
    else
    {
        // use a commonly known list
        ADObject *p;
        p = getObject( "DATAWQueryTableType" );
        p->setName( "TABLE" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "VIEW" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "SYSTEM TABLE" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "GLOBAL TEMPORARY" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "LOCAL TEMPORARY" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "ALIAS" );
        p->setModified( false );
        p = getObject( "DATAWQueryTableType" );
        p->setName( "SYNONYM" );
        p->setModified( false );
    }

    bQueried = true;
    bLoadingChildren = false;
}

