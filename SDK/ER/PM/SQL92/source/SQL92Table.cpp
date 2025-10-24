#include "LibInfo.h"
#include "SQL92Table.h"

#include "SQL92Model.h"
#include "SQL92Key.h"

SQL92Table::SQL92Table( SQL92Model *pParent )
    : PMTableNormal( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "SQL92Column", ADMeta( QIcon(), "SQL92Column", "Column" ) );
    mapMeta.insert( "SQL92Index", ADMeta( QIcon(), "SQL92Index", "Index" ) );
    mapMeta.insert( "SQL92Key", ADMeta( QIcon(), "SQL92Key", "Key" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

SQL92Table::~SQL92Table()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

ADObject *SQL92Table::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "SQL92Column" )
        p = new SQL92Column( this );
    else if ( stringClass == "SQL92Index" )
    {
        SQL92Index *pIndex = new SQL92Index( this );
        p = pIndex;
        pIndex->setName( pIndex->getDefaultName() );
    }
    else if ( stringClass == "SQL92Key" )
        p = new SQL92Key( this );

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

bool SQL92Table::doLoad( QDomElement *pdomElemThis )
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
            if ( domElem.tagName() == "SQL92Key" )                                 
            {                                                                          
                SQL92Key *pKey = (SQL92Key*)getObject( "SQL92Key" );
                pKey->doLoad( &domElem );                                       
            }                                                                          
        }                                                                              
        domNode = domNode.nextSibling();                                               
    } // while more children                                                           

    return true;
}


