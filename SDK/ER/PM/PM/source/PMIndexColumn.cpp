#include "LibInfo.h"
#include "PMIndexColumn.h"

#include "PMModel.h"
#include "PMIndex.h"

PMIndexColumn::PMIndexColumn( PMIndex *pParent )
    : AWObject( pParent ), PMObject( this )
{ 
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

PMIndexColumn::~PMIndexColumn()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, className(), this );
#endif
}

void PMIndexColumn::setOrder( const QString &s )
{
    if ( stringOrder == s ) return;
    stringOrder = s;
    setModified();
}

QPixmap PMIndexColumn::getIcon()
{
    return QPixmap( ":PM/Column" );
}

ADValidationDatabase PMIndexColumn::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    return mapClasses;
}

QString PMIndexColumn::getCreateAuto()
{
    return getName();
}

QString PMIndexColumn::getDropAuto()
{
    return QString();
}

/*!
    doSave
    
    Save ourself to the XML.
*/
QDomElement PMIndexColumn::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    doSavePMObject( pdomDoc, &domElemThis );

    if ( !getOrder().isEmpty() ) domElemThis.setAttribute( "Order", getOrder() );

    return domElemThis;
}

/*!
    doLoad
    
    Load ourself from the given XML.
*/
bool PMIndexColumn::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );
    doLoadPMObject( pdomElemThis );

    stringOrder = pdomElemThis->attribute( "Order" );

    return true;
}

/*!
    doValidate
    
    Validate ourself.
*/
void PMIndexColumn::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

void PMIndexColumn::slotName( const QString &s )
{
    setName( s );
}

void PMIndexColumn::slotOrder( const QString &s )
{
    setOrder( s );
}


