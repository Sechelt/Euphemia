#include "LibInfo.h"
#include "DATAWQueryEIConversion.h"

#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIConversion::DATAWQueryEIConversion( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr( "Conversion" ) )
{
    this->pConnection   = pConnection;
    stringComment       = tr( "The SQL data types this can be converted to (ie with the CONVERT scalar function)." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIConversion::~DATAWQueryEIConversion()
{
}

ADObject *DATAWQueryEIConversion::getObject( const QString &s, ADObject *p )
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

void DATAWQueryEIConversion::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    // LOAD CHILDREN                                                                                               
    ADObject *p;                                                                             
    QVariant  v;

    // conversion                                                                                                  
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_BIGINT" );                                                                     
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_BIGINT );                                                                
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                           
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_BINARY" );                                                                     
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_BINARY );                                                                
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_BIT" );                                                                        
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_BIT );                                                                   
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_CHAR" );                                                                       
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_CHAR );                                                                  
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_DATE" );                                                                       
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_DATE );                                                                  
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_DECIMAL" );                                                                    
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_DECIMAL );                                                               
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_DOUBLE" );                                                                     
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_DOUBLE );                                                                
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_FLOAT" );                                                                      
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_FLOAT );                                                                 
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_INTEGER" );                                                                    
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_INTEGER );                                                               
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_INTERVAL_YEAR_MONTH" );                                                        
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_INTERVAL_YEAR_MONTH );                                                   
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_INTERVAL_DAY_TIME" );                                                          
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_INTERVAL_DAY_TIME );                                                     
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_LONGVARBINARY" );                                                              
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_LONGVARBINARY );                                                         
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_LONGVARCHAR" );                                                                
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_LONGVARCHAR );                                                           
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_NUMERIC" );                                                                    
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_NUMERIC );                                                               
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_REAL" );                                                                       
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_REAL );                                                                  
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_SMALLINT" );                                                                   
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_SMALLINT );                                                              
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_TIME" );                                                                       
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_TIME );                                                                  
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_TIMESTAMP" );                                                                  
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_TIMESTAMP );                                                             
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_TINYINT" );                                                                    
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_TINYINT );                                                               
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_VARBINARY" );                                                                  
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_VARBINARY );                                                             
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );
                                                                                                                   
    p = getObject( "DATAWQueryExtendedInfoItem" );                                                                 
    p->setName( "SQL_CONVERT_VARCHAR" );                                                                    
    p->setComment( "indicates the conversions supported by the data source with the CONVERT scalar function" );
    v = pConnection->getInfo( SQL_CONVERT_VARCHAR );                                                               
    if ( !v.isNull() ) doLoadConversionDetails( v.toUInt(), p );                                               
    p->setModified( false );

    bQueried = true;
    bLoadingChildren = false;
}

void DATAWQueryEIConversion::doLoadConversionDetails( SQLUINTEGER nBitMask, ADObject *pParent )
{
    ADObject *pChild;               
                                                     
    pParent->setCode( QString::number( nBitMask ) );
                                                     
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent ); 
    pChild->setName( "SQL_CVT_BIGINT" );          
    pChild->setCode( "N" );                       
    if ( nBitMask & SQL_CVT_BIGINT )                 
        pChild->setCode( "Y" );                   
                                                         
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent ); 
    pChild->setName( "SQL_CVT_BINARY" );          
    pChild->setCode( "N" );                       
    if ( nBitMask & SQL_CVT_BINARY )                 
        pChild->setCode( "Y" );                   
                                                     
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent ); 
    pChild->setName( "SQL_CVT_BIT" );             
    pChild->setCode( "N" );                       
    if ( nBitMask & SQL_CVT_BIT )                    
        pChild->setCode( "Y" );                   
                                                     
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );
    pChild->setName( "SQL_CVT_GUID" );
    pChild->setCode( "N" );
    if ( nBitMask & SQL_CVT_GUID )
        pChild->setCode( "Y" );

    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_CHAR" );               
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_CHAR )                      
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_DATE" );               
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_DATE )                      
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_DECIMAL" );            
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_DECIMAL )                   
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_DOUBLE" );             
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_DOUBLE )                    
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_FLOAT" );              
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_FLOAT )                     
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_INTEGER" );            
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_INTEGER )                   
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_INTERVAL_YEAR_MONTH" );
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_INTERVAL_YEAR_MONTH )       
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_INTERVAL_DAY_TIME" );  
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_INTERVAL_DAY_TIME )         
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_LONGVARBINARY" );      
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_LONGVARBINARY )             
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_LONGVARCHAR" );        
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_LONGVARCHAR )               
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_NUMERIC" );            
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_NUMERIC )                   
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_REAL" );               
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_REAL )                      
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_SMALLINT" );           
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_SMALLINT )                  
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_TIME" );               
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_TIME )                      
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_TIMESTAMP" );          
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_TIMESTAMP )                 
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_TINYINT" );            
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_TINYINT )                   
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_VARBINARY" );          
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_VARBINARY )                 
        pChild->setCode( "Y" );                      
                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem", pParent );    
    pChild->setName( "SQL_CVT_VARCHAR" );            
    pChild->setCode( "N" );                          
    if ( nBitMask & SQL_CVT_VARCHAR )                   
        pChild->setCode( "Y" );                      
}





