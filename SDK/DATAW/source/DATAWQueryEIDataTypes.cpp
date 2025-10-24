#include "LibInfo.h"
#include "DATAWQueryEIDataTypes.h"

#include <DATAProfile.h>
#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIDataTypes::DATAWQueryEIDataTypes( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "DataTypes" ) )
{
    this->pConnection = pConnection;
    stringComment = tr( "List of data types supported by the driver. These can be used in DDL." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIDataTypes::~DATAWQueryEIDataTypes()
{
}

ADObject *DATAWQueryEIDataTypes::getObject( const QString &s, ADObject *p )
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

void DATAWQueryEIDataTypes::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    DATADataTypeSpec *pDataTypeSpec;                                                           
    foreach( pDataTypeSpec, pConnection->getProfile()->mapDataTypes )                                                               
    {                                                                             
        ADObject * pChild = getObject( "DATAWQueryExtendedInfoItem" );
        pChild->setName( pDataTypeSpec->TYPE_NAME );
        // CREATE_PARAMS relied upon instead of getSyntaxString (put it on the driver)
        // pChild->setComment( DATADataTypeSpec::getSyntaxString( d.TYPE_NAME ) );                                         
        doQueryDetails( pChild, pDataTypeSpec );
        pChild->setModified( false ); // self and all of our descendents
    }

    bQueried = true;
    bLoadingChildren = false;
}

void DATAWQueryEIDataTypes::doQueryDetails( ADObject *pParent, DATADataTypeSpec *pDataTypeSpec )
{
    ADObject * pChild;

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "DATA_TYPE" );         
    pChild->setCode( pDataTypeSpec->DATA_TYPE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "COLUMN_SIZE" );       
    pChild->setCode( pDataTypeSpec->COLUMN_SIZE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "LITERAL_PREFIX" );    
    pChild->setCode( pDataTypeSpec->LITERAL_PREFIX );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "LITERAL_SUFFIX" );    
    pChild->setCode( pDataTypeSpec->LITERAL_SUFFIX );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "CREATE_PARAMS" );     
    pChild->setCode( pDataTypeSpec->CREATE_PARAMS );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "NULLABLE" );          
    pChild->setCode( pDataTypeSpec->NULLABLE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "CASE_SENSITIVE" );    
    pChild->setCode( pDataTypeSpec->CASE_SENSITIVE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "SEARCHABLE" );        
    pChild->setCode( pDataTypeSpec->SEARCHABLE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "UNSIGNED_ATTRIBUTE" );
    pChild->setCode( pDataTypeSpec->UNSIGNED_ATTRIBUTE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "FIXED_PREC_SCALE" );  
    pChild->setCode( pDataTypeSpec->FIXED_PREC_SCALE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "AUTO_UNIQUE_VALUE" ); 
    pChild->setCode( pDataTypeSpec->AUTO_UNIQUE_VALUE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "LOCAL_TYPE_NAME" );   
    pChild->setCode( pDataTypeSpec->LOCAL_TYPE_NAME );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "MINIMUM_SCALE" );     
    pChild->setCode( pDataTypeSpec->MINIMUM_SCALE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "MAXIMUM_SCALE" );     
    pChild->setCode( pDataTypeSpec->MAXIMUM_SCALE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "SQL_DATA_TYPE" );     
    pChild->setCode( pDataTypeSpec->SQL_DATA_TYPE );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "SQL_DATETIME_SUB" );  
    pChild->setCode( pDataTypeSpec->SQL_DATETIME_SUB );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "NUM_PREC_RADIX" );    
    pChild->setCode( pDataTypeSpec->NUM_PREC_RADIX );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "INTERVAL_PRECISION" );
    pChild->setCode( pDataTypeSpec->INTERVAL_PRECISION );
}
