#include "LibInfo.h"
#include "DATAWQueryEIDataSource.h"

#include <DATAConnection.h>

#include "DATAWQueryExtendedInfoItem.h"

DATAWQueryEIDataSource::DATAWQueryEIDataSource( ADObject *pParent, DATAConnection *pConnection )
    : DATAWQueryFolder( pParent, tr("Data Source") )
{
    this->pConnection        = pConnection;
    stringComment  = tr( "Information about the data source, such as cursor characteristics and transaction capabilities." );

    mapMeta.insert( "DATAWQueryExtendedInfoItem", ADMeta( QIcon(), "DATAWQueryExtendedInfoItem", "Info" ) );
}

DATAWQueryEIDataSource::~DATAWQueryEIDataSource()
{
}

ADObject *DATAWQueryEIDataSource::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryExtendedInfoItem" )        
        p = new DATAWQueryExtendedInfoItem( this );

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

void DATAWQueryEIDataSource::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    // LOAD CHILDREN                                                                                    
    ADObject *pChild;                                                                  
    QVariant  v;                                                                           
    QString   stringComment;

    // data source                                                                                      
    pChild = getObject( "DATAWQueryExtendedInfoItem" );                                                      
    pChild->setName( "SQL_ACCESSIBLE_PROCEDURES" );                                                   
    pChild->setComment( tr("the user can execute all procedures returned by SQLProcedures") );
    v = pConnection->getInfo( SQL_ACCESSIBLE_PROCEDURES );                                              
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               
                                                                                                        
    pChild = getObject( "DATAWQueryExtendedInfoItem" );                                                      
    pChild->setName( "SQL_ACCESSIBLE_TABLES" );                                                       
    pChild->setComment( tr("the user is guaranteed SELECT privileges to all tables returned by SQLTables") );
    v = pConnection->getInfo( SQL_ACCESSIBLE_TABLES );                                                  
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               

/* PAH TODO:

SQL_BOOKMARK_PERSISTENCE  
SQL_CATALOG_TERM  
SQL_COLLATION_SEQ  
SQL_CONCAT_NULL_BEHAVIOR  
SQL_CURSOR_COMMIT_BEHAVIOR  
SQL_CURSOR_ROLLBACK_BEHAVIOR  
SQL_CURSOR_SENSITIVITY  
SQL_DATA_SOURCE_READ_ONLY  
SQL_DEFAULT_TXN_ISOLATION  
SQL_DESCRIBE_PARAMETER 
SQL_MULT_RESULT_SETS
SQL_MULTIPLE_ACTIVE_TXN
SQL_NEED_LONG_DATA_LEN
SQL_NULL_COLLATION
SQL_PROCEDURE_TERM
SQL_SCHEMA_TERM
SQL_SCROLL_OPTIONS
SQL_TABLE_TERM
SQL_TXN_CAPABLE
SQL_TXN_ISOLATION_OPTION

*/

    pChild = getObject( "DATAWQueryExtendedInfoItem" );                                                                                     
    pChild->setName( "SQL_USER_NAME" );                                                                                              
    pChild->setComment( tr("A character string with the name used in a particular database, which can be different from the login name.") );
    v = pConnection->getInfo( SQL_USER_NAME );                                                                                         
    if ( v.isNull() ) pChild->setCode( "U" );                                               
    else pChild->setCode( v.toString() );                                               

    setModified( false );
    bQueried = true;
    bLoadingChildren = false;
}

