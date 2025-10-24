#include "LibInfo.h"
#include "MSAReference.h"

#include "MSAModel.h"
#include "MSAReferenceColumn.h"

MSAReference::MSAReference( MSAModel *pParent )
    : PMReference( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "MSAReferenceColumn", ADMeta( QIcon(), "MSAReferenceColumn", "Column" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

MSAReference::~MSAReference()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ADObject *MSAReference::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "MSAReferenceColumn" )
        p = new MSAReferenceColumn( this );
    else
    {
        return PMReference::getObject( stringClass );
    }

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


