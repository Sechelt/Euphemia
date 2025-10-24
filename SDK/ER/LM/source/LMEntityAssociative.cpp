#include "LibInfo.h"
#include "LMEntityAssociative.h"

#include <DGraphicsProxyItem.h>

#include "LMModel.h" 
#include "LMAttribute.h"
#include "LMEntityNormal.h"
#include "LMAssociativeLink.h"

LMEntityAssociative::LMEntityAssociative( LMModel *pmodelParent )
    : LMEntityBase( pmodelParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    pRectangle = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMEntityAssociative::~LMEntityAssociative()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMEntityAssociative::setSelectType()
{
    switch ( getSelectType() )
    {
        case SelectResize:
            DRectangleBase::setSelectType( SelectMenu );
            break;
        case SelectTransformRotate:
            break;
        case SelectTransformShear:
            break;
        case SelectMenu:
            DRectangleBase::setSelectType( SelectResize );
            break;
        case SelectOnly:
            break;
    }
}

QPixmap LMEntityAssociative::getIcon()
{
    return QPixmap( ":LM/Association16x16" );
}

AWDataWidget *LMEntityAssociative::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMEntityBase::getObjectWidget( pWidgetParent );
    
    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase LMEntityAssociative::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Information );
        mapValidations.insert( tr( "Link missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Warning );
        mapValidations.insert( tr( "Link missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Code missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Name duplicate" ), ADValidation::Error );
        mapValidations.insert( tr( "Code duplicate" ), ADValidation::Error );
        mapValidations.insert( tr( "Link missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "LMEntityAssociative", mapStrictness );

    return mapClasses;
}

void LMEntityAssociative::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Code...                                                                                                                        
    stringValidation = tr("Code missing");
    nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getCode().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
                                                                                                                      
    // Duplicate...
    ADObject *pObjectParent = (ADObject*)getParent( "ADObject" );
    if ( pObjectParent )
    { 
        stringValidation = tr("Name duplicate");
        nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getName().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldName ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

        stringValidation = tr("Code duplicate");                                                                         
        nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                                                          
        if ( nSeverity != ADValidation::Ignore && !getCode().isEmpty() && pObjectParent->getDuplicate( this, ADObject::FieldCode ) )                                                                                                
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                
    }
                 
    // need at least two links connected...
/*
    stringValidation = tr("Link missing");                                                                          
    nSeverity = getValidationSeverity( "LMEntityAssociative", nStrictness, stringValidation );                      
    if ( nSeverity != ADValidation::Ignore )                                                                        
    {                                                                                                               
        int nLinks = 0;                                                                                             
        DGSink *pSink;                                                                                              
                                                                                                                    
        foreach( pSink, vectorSinks )                                                                               
        {                                                                                                           
            if ( pSink->isGlued() ) nLinks++;                                                                       
            if ( nLinks >= 2 ) break;                                                                               
        }                                                                                                           
        if ( nLinks <= 1 )                                                                                          
            eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );
    }                                                                                                               
*/
                                                                                                                                      
    // check our children                                                                                                              
    QList<ADObject*>    listChildren = getObjects( QString(), QString(), QString() );                                            
    ADObject *          pObject;
    int                 nIndex = -1;                                                                                           

    foreach( pObject, listChildren )
    {                                                                                                                                  
        nIndex++;                                                                                                                      

        // append child to path                                                                                                        
        listObjectPathItems += ADObjectPathItem( nValueType, pObject, pObject->metaObject()->className(), pObject->getName(), nIndex );
        // validate child                                                                                                              
        pObject->doValidate( nStrictness, nValueType, listObjectPathItems );                                                                        
        // remove child from path                                                                                                      
        listObjectPathItems.pop_back();                                                                                                
    }                                                                                                                                  
}

void LMEntityAssociative::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    LMRectangle::mouseReleaseHandle( pHandle, pointScene );
    if ( !pRectangle ) return;
    doSyncMenu();
    pRectangle = nullptr;
}

void LMEntityAssociative::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    int nLineHeight     = pPainter->fontMetrics().height() + 2;
    // QFont font          = pPainter->font();
    
    //
    // ENTITY
    // Draw the rectangle of the entity.
    //
    QRectF rectBounding  = boundingRect();
#if QT_VERSION < 0x060000
    pPainter->drawRoundRect( rectBounding );
#else
    pPainter->drawRoundedRect( rectBounding, 20.0, 15.0 );
#endif
    pPainter->drawLine( rectBounding.left(), rectBounding.top() + nLineHeight, rectBounding.right(), rectBounding.top() + nLineHeight );

    //
    // ENTITY NAME
    // Draw the entity name.
    //
    QRectF rectName;
    font.font.setBold( true );
    pPainter->setFont( font.font );
    pPainter->setPen( font.color );
    rectName.setRect( rectBounding.left() + 2, rectBounding.top(), rectBounding.width() - 2, nLineHeight );
    pPainter->drawText( rectName, getTextFlags(), getName() );
    font.font.setBold( false );
    pPainter->setFont( font.font );
    rectName.setY( rectName.y() + 3 );

    //
    // DESIRED WIDTHS
    // Init the field rectangles to height and max needed widths.
    //

    // get max needed widths...
    int nNameW;
    int nDataTypeW;
    int nKeyW;
    getAttributeWidths( pPainter, &nNameW, &nDataTypeW, &nKeyW );
 
    rectName.setWidth( nNameW );
    QRectF rectDataType( rectName.x() + nNameW, rectName.y(), nDataTypeW, rectName.height() );
    QRectF rectKey( rectDataType.x() + nDataTypeW, rectName.y(), nKeyW, rectName.height() );

    //
    // SHRINK WIDTHS
    // Work right to left.
    // Shrink widths when not enough space.
    // Widths are set to 0 when the field has to be 'sacraficed' due to lack of horizontal space. 
    //

    // key
    if ( rectKey.left() > rectBounding.right() ) rectKey.setWidth( 0 );
    else if ( rectKey.right() > rectBounding.right() ) rectKey.setWidth( rectKey.width() - (rectKey.right() - rectBounding.right()) );

    // data type
    if ( rectDataType.left() > rectBounding.right() ) rectDataType.setWidth( 0 );
    else if ( rectDataType.right() > rectBounding.right() ) rectDataType.setWidth( rectDataType.width() - (rectDataType.right() - rectBounding.right()) );

    // name
    if ( rectName.left() > rectBounding.right() ) rectName.setWidth( 0 );
    else if ( rectName.right() > rectBounding.right() ) rectName.setWidth( rectName.width() - (rectName.right() - rectBounding.right()) );

    //
    // ATTRIBUTES
    // Draw the attributes. Each contains up to 3 fields/rectangles; name, type and key.
    // Move the boxs down as we go.
    // Stop early if we run out space. In such a case - draw a '+' to give indication.
    //
    {
        QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
        ADObject *          pObject;
        foreach( pObject, listAttributes )
        {
            LMAttribute *pattribute = (LMAttribute*)pObject;

            // identifiers are underlined
            if ( pattribute->getIdentifier() )
                font.font.setUnderline( true );
            else
                font.font.setUnderline( false );
            pPainter->setFont( font.font );

            // move down 1 row
            rectName.moveTo( rectName.x(), rectName.y() + nLineHeight );
            rectDataType.moveTo( rectDataType.x(), rectDataType.y() + nLineHeight );
            rectKey.moveTo( rectKey.x(), rectKey.y() + nLineHeight );

            // are we completely past the bottom of the table object?
            if ( rectName.bottom() >= rectBounding.bottom() )
            {
                pPainter->drawText( QRectF( rectBounding.left(), rectBounding.bottom() - nLineHeight, rectBounding.width(), nLineHeight ), Qt::AlignCenter, "+" );
                return;
            }

            // are we partially past the bottom of the table object?
            if ( rectName.bottom() > rectBounding.bottom() )
            {
                rectName.setBottom( rectBounding.bottom() );
                rectDataType.setBottom( rectBounding.bottom() );
                rectKey.setBottom( rectBounding.bottom() );
            }

            // form key
            QString stringKey( "" );
            if ( pattribute->getIdentifier() && pattribute->getMandatory() )
                stringKey = "pi,M";
            else if ( pattribute->getIdentifier() )
                stringKey = "pi";
            else if ( pattribute->getMandatory() )
                stringKey = "M";

            // draw text
            pPainter->drawText( rectName, Qt::AlignLeft, pattribute->getName() );
            pPainter->drawText( rectDataType, Qt::AlignLeft, pattribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) );
            pPainter->drawText( rectKey, Qt::AlignRight, stringKey );

// this is great for debugging
/*
            pPainter->setPen( Qt::red );       
            pPainter->drawRect( rectName );    
            pPainter->setPen( Qt::green );     
            pPainter->drawRect( rectDataType );
            pPainter->setPen( Qt::blue );      
            pPainter->drawRect( rectKey );     
*/

        } // while
    }
}

void LMEntityAssociative::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    if ( pHandle == vectorHandles.at( 0 ) )
    {
        if ( !pRectangle )
        {
            LMModel *pModel = (LMModel*)getParent( "LMModel" );
            pRectangle = (LMEntityNormal*)pModel->getObject( "LMEntityNormal" );
            pRectangle->setPos( pointScenePos );
            LMAssociativeLink *pAssociativeLink = (LMAssociativeLink*)pModel->getObject( "LMAssociativeLink" );  
            pAssociativeLink->setPos( pointScenePos );
            pAssociativeLink->setBegin( getPos() );
            pAssociativeLink->setEnd( pointScenePos );
            pAssociativeLink->doConnect( this, "Begin" );
            pAssociativeLink->doConnect( pRectangle, "End" );
        }
        else pRectangle->setPos( pointScenePos );
        pHandle->setPos( pointScenePos );
    }
}

void LMEntityAssociative::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 1 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QPixmap( ":LM/Entity16x16" ).scaled( 16, 16 ) );
    vectorHandles[0] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}

void LMEntityAssociative::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void LMEntityAssociative::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[0];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );
}


