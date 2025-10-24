#include "LibInfo.h"
#include "DATAWQueryEILimits.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEILimits::DATAWQueryEILimits( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "SQL Limits" ) )
{
    this->pConnection        = pConnection;
    stringComment = tr( "Information about the limits applied to identifiers and clauses in SQL statements, such as the maximum lengths of identifiers and the maximum number of columns in a select list. Limitations can be imposed by either the driver or the data source." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEILimits::~DATAWQueryEILimits()
{
}

ADObject *DATAWQueryEILimits::getObject( const QString &s, ADObject *p )
{
    ADObject *pParent   = ( p ? p : this ); 
    AWObject *pChild    = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryExtendedInfoItem" )        
        pChild = new DATAWQueryExtendedInfoItem( pParent );

    if ( !pChild )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit pParent->signalCreated( pChild );    
    emit pParent->signalCreated( pParent, pChild );

    return pChild;
}

void DATAWQueryEILimits::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    doQueryDetails( "SQL_MAX_BINARY_LITERAL_LEN" );    
    doQueryDetails( "SQL_MAX_IDENTIFIER_LEN" );        
    doQueryDetails( "SQL_MAX_CATALOG_NAME_LEN" );      
    doQueryDetails( "SQL_MAX_INDEX_SIZE" );            
    doQueryDetails( "SQL_MAX_CHAR_LITERAL_LEN" );      
    doQueryDetails( "SQL_MAX_PROCEDURE_NAME_LEN" );    
    doQueryDetails( "SQL_MAX_COLUMN_NAME_LEN" );       
    doQueryDetails( "SQL_MAX_ROW_SIZE" );              
    doQueryDetails( "SQL_MAX_COLUMNS_IN_GROUP_BY" );   
    doQueryDetails( "SQL_MAX_ROW_SIZE_INCLUDES_LONG" );
    doQueryDetails( "SQL_MAX_COLUMNS_IN_INDEX" );      
    doQueryDetails( "SQL_MAX_SCHEMA_NAME_LEN" );       
    doQueryDetails( "SQL_MAX_COLUMNS_IN_ORDER_BY" );   
    doQueryDetails( "SQL_MAX_STATEMENT_LEN" );         
    doQueryDetails( "SQL_MAX_COLUMNS_IN_SELECT" );     
    doQueryDetails( "SQL_MAX_TABLE_NAME_LEN" );        
    doQueryDetails( "SQL_MAX_COLUMNS_IN_TABLE" );      
    doQueryDetails( "SQL_MAX_TABLES_IN_SELECT" );      
    doQueryDetails( "SQL_MAX_CURSOR_NAME_LEN" );       
    doQueryDetails( "SQL_MAX_USER_NAME_LEN" );         

    setModified( false );  // self and all of our descendents
    bQueried = true;
    bLoadingChildren = false;
}


void DATAWQueryEILimits::doQueryDetails( const QString &s )
{
    ADObject *pParent;                  
    ADObject *pChild;                 
    DATAInfo *pInfoParent;                                         
    DATAInfo *pInfoChild;                                        

    pInfoParent = pConnection->getProfile()->mapInfo.value( s );
    pParent = getObject( "DATAWQueryExtendedInfoItem" );      
    pParent->setName( pInfoParent->stringInfo );                  
    pParent->setCode( pInfoParent->vValue.toString() );           
    pParent->setComment( pInfoParent->stringDescription );           

    foreach( pInfoChild, pInfoParent->mapValues )                          
    {                                                   
        pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );      
        pChild->setName( pInfoChild->stringInfo );                  
        pChild->setCode( pInfoChild->vValue.toString() );           
        pChild->setComment( pInfoChild->stringDescription );           
    }                                                   
}

