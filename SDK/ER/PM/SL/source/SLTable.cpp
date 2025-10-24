#include "LibInfo.h"
#include "SLTable.h"

#include "SLModel.h"
#include "SLKey.h"

//
// SLTable
//
SLTable::SLTable( SLModel *pParent )
    : PMTableNormal( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "SLColumn", ADMeta( QIcon(), "SLColumn", "Column" ) );
    mapMeta.insert( "SLIndex", ADMeta( QIcon(), "SLIndex", "Index" ) );
    mapMeta.insert( "SLKey", ADMeta( QIcon(), "SLKey", "Key" ) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

SLTable::~SLTable()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void SLTable::setStrict( bool b )
{
    bStrict = b;
}

void SLTable::setRowID( bool b )
{
    bRowID = b;
}

ADObject *SLTable::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "SLColumn" )
        p = new SLColumn( this );
    else if ( stringClass == "SLIndex" )
    {
        SLIndex *pIndex = new SLIndex( this );
        p = pIndex;
        pIndex->setName( pIndex->getDefaultName() );
    }
    else if ( stringClass == "SLKey" )
        p = new SLKey( this );

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

AWDataWidget *SLTable::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = PMTableNormal::getObjectWidget( pWidgetParent );
    p->insertTab( 1, new SLTableWidget( this, p ), tr("Table") );
    p->setCurrentIndex( 0 );

    return p;
}

QDomElement SLTable::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = PMTableNormal::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "Strict", getStrict() );
    domElemThis.setAttribute( "RowID", getRowID() );

    return domElemThis;
}

bool SLTable::doLoad( QDomElement *pdomElemThis )
{
    PMTableNormal::doLoad( pdomElemThis );

    bStrict = pdomElemThis->attribute( "Strict", "0" ).toInt();
    bRowID  = pdomElemThis->attribute( "RowID", "1" ).toInt();

    // load child nodes                                                                
    QDomElement         domElem;                                                       
    QDomNode            domNode;                                                       
    domNode = pdomElemThis->firstChild();                                              
    while( !domNode.isNull() )                                                         
    {                                                                                  
        domElem = domNode.toElement();                                                 
        if( !domElem.isNull() )                                                        
        {                                                                              
            if ( domElem.tagName() == "SLKey" )                                 
            {                                                                          
                SLKey *pKey = (SLKey*)getObject( "SLKey" );
                pKey->doLoad( &domElem );                                       
            }                                                                          
        }                                                                              
        domNode = domNode.nextSibling();                                               
    } // while more children                                                           

    return true;
}

//
// SLTableWidget
//
SLTableWidget::SLTableWidget( SLTable *p, QWidget *pParent )
    : QWidget( pParent )
{
    pTable = p;

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->insertRow( 0, new QLabel( tr("Strict") ), pStrict = new QCheckBox( this ) );
    pLayout->insertRow( 0, new QLabel( tr("RowID") ), pRowID = new QCheckBox( this ) );

    pStrict->setChecked( pTable->getStrict() );
    pRowID->setChecked( pTable->getRowID() );
}

