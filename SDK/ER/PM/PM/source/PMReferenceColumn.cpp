#include "LibInfo.h"
#include "PMReferenceColumn.h"

// #include "PMModel.h"
#include "PMReference.h"
#include "PMColumn.h"

//
// PMReferenceColumn
//
PMReferenceColumn::PMReferenceColumn( PMReference *pParent )
    : AWObject( pParent )
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMReferenceColumn::~PMReferenceColumn()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void PMReferenceColumn::setParentColumn( PMColumn *pColumn )
{
    // \note we "Use" ParentColumn
    
    // remove any existing reference
    PMColumn *p = getParentColumn();
    if ( p )
    {
        if ( p == pColumn ) return;
        // Remove ref...
        doDisconnect( p );  
        setModified();
    }

    // Add ref...
    if ( pColumn )
    {
        doConnect( pColumn );
        setModified();
    }
}

void PMReferenceColumn::setChildColumn( PMColumn *pColumn )
{
    // \note we are "UsedBy" ChildColumn
   
    // remove any existing ref
    PMColumn *p = getChildColumn();
    if ( p )
    {
        if ( p == pColumn ) return;
        // Remove ref...
        p->doDisconnect( this );  
        setModified();
    }

    // Add ref...
    if ( pColumn )
    {
        pColumn->doConnect( this );
        setModified();
    }
}

QPixmap PMReferenceColumn::getIcon()
{
    return QPixmap( ":PM/Column" );
}

QString PMReferenceColumn::getName()
{
    return tr("Condition");
}

QString PMReferenceColumn::getComment()
{
    // todo show complete condition ie table.column = table.column  and...
#ifdef CBD_TODO
    printf( "[PAH][%s][%s][%d] ToDo\n", __FILE__, __FUNCTION__, __LINE__ );
#endif
    return AWObject::getComment();
}

PMColumn *PMReferenceColumn::getParentColumn()
{
    // \note we "Use" ParentColumn

    QList<ADObjectReferenceKey> l = getUsesKeys( nullptr, QString(), QString(), "PMColumn" );
    if ( !l.count() ) return nullptr;

    return (PMColumn*)(l.first().pObject);
}

PMColumn *PMReferenceColumn::getChildColumn()
{
    // \note we are "UsedBy" ChildColumn

    QList<ADObjectReferenceKey> l = getUsedByKeys( nullptr, QString(), QString(), "PMColumn" );
    if ( !l.count() ) return nullptr;

    return (PMColumn *)(l.first().pObject);
}

//
// PMReferenceColumnDisplay
//
PMReferenceColumnDisplay::PMReferenceColumnDisplay( PMReference *pParent )
    : AWObject( pParent )
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMReferenceColumnDisplay::~PMReferenceColumnDisplay()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

QPixmap PMReferenceColumnDisplay::getIcon()
{
    return QPixmap( ":PM/Column" );
}

QVector<PMColumn *>PMReferenceColumnDisplay::getColumns()
{
    QVector<PMColumn *> vectorReturn;
    QList<ADObjectReferenceKey> l = getUsesKeys( nullptr, QString(), QString(), "PMColumn" );
    for ( auto i = l.begin(), end = l.end(); i != end; ++i )
    {
        PMColumn *p = (PMColumn*)(i->pObject);
        vectorReturn << p;
    }
    return vectorReturn;
}

void PMReferenceColumnDisplay::doConnectColumn( PMColumn *pColumn )
{
    // \note we "Use" ParentColumn
    
    // Add ref...
    doConnect( pColumn );
    setModified();
}

void PMReferenceColumnDisplay::doDisconnectColumn( PMColumn *pColumn )
{
    // \note we "Use" ParentColumn
    
    // Remove ref...
    doDisconnect( pColumn );  
    setModified();
}

