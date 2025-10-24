#include "LibInfo.h"
#include "ODBCTable.h"

#include "ODBCModel.h"
#include "ODBCKey.h"

ODBCTable::ODBCTable( ODBCModel *pParent )
    : PMTableNormal( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // we always support columns
    mapMeta.insert( "ODBCColumn", ADMeta( QIcon(), "ODBCColumn", "Column" ) );

    if ( pParent->hasFeature( PMModel::FeaturePrimaryKey, PMModel::FeatureSupportUse ) )
    {
        mapMeta.insert( "ODBCKey", ADMeta( QIcon(), "ODBCKey", "Key" ) );
    }

    if ( pParent->hasFeature( PMModel::FeatureIndex, PMModel::FeatureSupportUse ) )
    { 
        mapMeta.insert( "ODBCIndex", ADMeta( QIcon(), "ODBCIndex", "Index" ) );
    }

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ODBCTable::~ODBCTable()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ADObject *ODBCTable::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "ODBCColumn" )
        p = new ODBCColumn( this );
    else if ( stringClass == "ODBCKey" )
        p = new ODBCKey( this );
    else if ( stringClass == "ODBCIndex" )
        p = new ODBCIndex( this );

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}


