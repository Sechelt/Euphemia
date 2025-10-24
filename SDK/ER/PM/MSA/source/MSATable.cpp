#include "LibInfo.h"
#include "MSATable.h"

#include "MSAModel.h"
#include "MSAKey.h"

MSATable::MSATable( MSAModel *pParent )
    : PMTableNormal( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "MSAColumn", ADMeta( QIcon(), "MSAColumn", "Column" ) );
    mapMeta.insert( "MSAIndex", ADMeta( QIcon(), "MSAIndex", "Index" ) );
    mapMeta.insert( "MSAKey", ADMeta( QIcon(), "MSAKey", "Key" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

MSATable::~MSATable()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ADObject *MSATable::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "MSAColumn" )
        p = new MSAColumn( this );
    else if ( stringClass == "MSAIndex" )
    {
        MSAIndex *pIndex = new MSAIndex( this );
        p = pIndex;
        pIndex->setName( pIndex->getDefaultName() );
    }
    else if ( stringClass == "MSAKey" )
        p = new MSAKey( this );

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

bool MSATable::doLoad( QDomElement *pdomElemThis )
{
    PMTableNormal::doLoad( pdomElemThis );

    // load child nodes                                                                
    QDomElement         domElem;                                                       
    QDomNode            domNode;                                                       
    domNode = pdomElemThis->firstChild();                                              
    while( !domNode.isNull() )                                                         
    {                                                                                  
        domElem = domNode.toElement();                                                 
        if( !domElem.isNull() )                                                        
        {                                                                              
            if ( domElem.tagName() == "MSAKey" )                                 
            {                                                                          
                MSAKey *pKey = (MSAKey*)getObject( "MSAKey" );
                pKey->doLoad( &domElem );                                       
            }                                                                          
        }                                                                              
        domNode = domNode.nextSibling();                                               
    } // while more children                                                           

    return true;
}


