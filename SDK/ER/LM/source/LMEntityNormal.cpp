#include "LibInfo.h"
#include "LMEntityNormal.h"

#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>
#include <DGraphicsProxyItem.h>

#include "LMModel.h" 
#include "LMIdentifier.h"
#include "LMAttributesWidget.h"
#include "LMIdentifiersWidget.h"
#include "LMEntityOptionsWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"
#include "LMEntityAssociative.h"
#include "LMRelationship.h"
#include "LMAssociativeLink.h"

LMEntityNormal::LMEntityNormal( LMModel *pmodel )
    : LMEntityBase( pmodel )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    mapMeta.insert( "LMIdentifier", ADMeta( QIcon(), "LMIdentifier", "Identifier" ) );

    pRectangle = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMEntityNormal::~LMEntityNormal()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMEntityNormal::setSelectType()
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

/*!
    The icon to be used in an Object Browser (ListView) and elsewhere.
*/
QPixmap LMEntityNormal::getIcon()
{
    return QPixmap( ":LM/Entity16x16" );
}

AWDataWidget *LMEntityNormal::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMEntityBase::getObjectWidget( pWidgetParent );
    
    p->insertTab( 2, new LMIdentifiersWidget( this, p ), tr("Identifiers") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase LMEntityNormal::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
/*
    ADValidation * pproperty;                                                                            
                                                                                                         
    switch ( nValidationLevel )                                                                          
    {                                                                                                    
    case nValidationLevelLow:                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name missing" );              
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code missing" );              
        doInitValidationProperty( pproperty, "" );                                                       
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity comment missing" );           
        doInitValidationProperty( pproperty, "" );                                                       
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity description missing" );       
        doInitValidationProperty( pproperty, "" );                                                       
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name duplicate" );            
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code duplicate" );            
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity attribute missing" );         
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity primary identifier missing" );
        doInitValidationProperty( pproperty, "" );                                                       
        plistValidations->append( pproperty );                                                           
        break;                                                                                           
    case nValidationLevelMedium:                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name missing" );              
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code missing" );              
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity comment missing" );           
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity description missing" );       
        doInitValidationProperty( pproperty, "" );                                                       
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name duplicate" );            
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code duplicate" );            
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity attribute missing" );         
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity primary identifier missing" );
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
        break;                                                                                           
    case nValidationLevelHigh:                                                                           
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name missing" );              
        doInitValidationProperty( pproperty, "Error" );                                                  
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code missing" );              
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity comment missing" );           
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity description missing" );       
        doInitValidationProperty( pproperty, "Information" );                                            
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity name duplicate" );            
        doInitValidationProperty( pproperty, "Error" );                                                  
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity code duplicate" );            
        doInitValidationProperty( pproperty, "Error" );                                                  
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity attribute missing" );         
        doInitValidationProperty( pproperty, "Error" );                                                  
        plistValidations->append( pproperty );                                                           
                                                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Entity primary identifier missing" );
        doInitValidationProperty( pproperty, "Warning" );                                                
        plistValidations->append( pproperty );                                                           
        break;                                                                                           
    }                                                                                                    
*/
    return mapClasses;
}

/*!
    getIdentifier
    
    Return the first identifier matching bPrimary.
*/
LMIdentifier * LMEntityNormal::getIdentifier( bool bPrimary )
{
    QList<ADObject*>          listIdentifiers = getObjects( QString(), QString(), "LMIdentifier" );
    ADObject *pObject;

    foreach( pObject, listIdentifiers )
    {
        LMIdentifier *pidentifier = (LMIdentifier*)pObject;
        if ( pidentifier->isPrimary() == bPrimary )
            return pidentifier;
    }

    return 0;
}

ADObject *LMEntityNormal::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "LMIdentifier" )
        p = new LMIdentifier( this );

    if ( !p ) return LMEntityBase::getObject( s );

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}

bool LMEntityNormal::doLoad( QDomElement *pdomElemThis )
{
    LMEntityBase::doLoad( pdomElemThis );

    // load child nodes
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( !domElem.isNull() ) 
        {
            if ( domElem.tagName() == "LMIdentifier" )
            {
                LMIdentifier *pIdentifier = (LMIdentifier*)getObject( "LMIdentifier" );
                pIdentifier->doLoad( &domElem );
            }
        }
        domNode = domNode.nextSibling();
    } // while more children

    return true;
}

/*!
    doValidate
        
    Validate ourself. 
*/
void LMEntityNormal::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString stringCheck;                                                                                                              
    QString stringCheckLevel;                                                                                                         
    LMModel *pmodel = (LMModel*)getParent( "LMModel" );                                                                         
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Entity name missing";                                                                                      
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Entity code missing";                                                                                      
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Entity comment missing";                                                                                   
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Entity description missing";                                                                               
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getDescription().isEmpty() )                                                                                             
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Duplicate?                                                                                                                     
    {                                                                                                                                 
        bool    bCheckName    = false;                                                                                                
        bool    bCheckCode    = false;                                                                                                
        bool *  pbCheckName   = 0;                                                                                                    
        bool *  pbCheckCode   = 0;                                                                                                    
        QString stringCheckName;                                                                                                      
        QString stringCheckLevelName;                                                                                                 
        QString stringCheckCode;                                                                                                      
        QString stringCheckLevelCode;                                                                                                 
                                                                                                                                      
        if ( !getName().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckName         = "Entity name duplicate";                                                                        
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Entity code duplicate";                                                                        
            stringCheckLevelCode    = pmodel->getValidationSeverity( stringCheckCode );                                          
            if ( !stringCheckLevelCode.isEmpty() )                                                                                    
                pbCheckCode = &bCheckCode;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( pbCheckName || pbCheckCode )                                                                                             
        {                                                                                                                             
            if ( pmodel->getDuplicate( this, pbCheckName, pbCheckCode ) )                                                             
            {                                                                                                                         
                if ( bCheckName )                                                                                                     
                    eventOutputValidate( listObjectPathItems, stringCheckLevelName, stringCheckName + " (" + getName() + ")" );
                if ( bCheckCode )                                                                                                     
                    eventOutputValidate( listObjectPathItems, stringCheckLevelCode, stringCheckCode + " (" + getName() + ")" );
            }                                                                                                                         
        }                                                                                                                             
    }                                                                                                                                 
                                                                                                                                      
    // Attribute missing...                                                                                                           
    stringCheck         = "Entity attribute missing";                                                                                 
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        QList<ADObject*> listModels =  getObjects( QString(), QString(), "LMAttribute" );                                            
                                                                                                                                      
        if ( listModels.count() < 1 )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Identifier...                                                                                                                  
    stringCheck         = "Entity primary identifier missing";                                                                        
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        QList<ADObject*>          listChildren = getObjects( QString(), QString(), "LMIdentifier" );                                 
        QPtrListIterator<ADObject>  iteratorChildren( listChildren );                                                                 
        ADObject *                  pmodel;                                                                                           
        LMIdentifier *              pidentifier;                                                                                      
                                                                                                                                      
        while ( (pmodel = iteratorChildren.current()) )                                                                               
        {                                                                                                                             
            ++iteratorChildren;                                                                                                       
            pidentifier = (LMIdentifier*)pmodel;                                                                                      
            if ( pidentifier->getPrimary() )                                                                                          
                break;                                                                                                                
        }                                                                                                                             
                                                                                                                                      
        if ( !pmodel )                                                                                                                
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 

    // doValidate all children
    ADObject::doValidate( nValueType, listObjectPathItems );
*/
}

void LMEntityNormal::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    LMRectangle::mouseReleaseHandle( pHandle, pointScene );
    if ( !pRectangle ) return;
    doSyncMenu();
    pRectangle = nullptr;
}

void LMEntityNormal::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    LMModel *pModel = (LMModel*)getParent( "LMModel" );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    int nLineHeight     = pPainter->fontMetrics().height() + 2;
    // int nMarginWidth    = pPainter->fontMetrics().width( "pk " );

    // a font we can play with...
    QFont f = pPainter->font();
    
    //
    // ENTITY
    // Draw the rectangle of the entity.
    //
    QRectF rectBounding  = boundingRect();
    pPainter->drawRect( rectBounding );
    pPainter->drawLine( rectBounding.left(), rectBounding.top() + nLineHeight, rectBounding.right(), rectBounding.top() + nLineHeight );

    //
    // ENTITY NAME
    // Draw the entity name.
    //
    QRectF rectName;
    f.setBold( true );
    pPainter->setFont( f );
    pPainter->setPen( font.color );
    rectName.setRect( rectBounding.left() + 2, rectBounding.top(), rectBounding.width() - 2, nLineHeight );
    pPainter->drawText( rectName, getTextFlags(), getName() );
    f.setBold( false );
    pPainter->setFont( f );

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
                f.setUnderline( true );
            else
                f.setUnderline( false );
            pPainter->setFont( f );

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

    // this is where we left off
    qreal nY = rectName.bottom();

    //
    // IDENTIFIERS
    // Draw the identifier names.
    // Do in reverse order starting at bottom of entity.
    // Stop early if we run out space. In such a case - draw a '+' to give indication.
    //
    {
        rectName.moveTo( rectName.x(), rectBounding.bottom() );
        rectKey.moveTo( rectKey.x(), rectBounding.bottom() );

        QList<ADObject*>    listIdentifiers = getObjects( QString(), QString(), "LMIdentifier" );
        QListIterator<ADObject*> i( listIdentifiers );
        i.toBack();
        while ( i.hasPrevious() )
        {
            LMIdentifier *pidentifier = (LMIdentifier*)i.previous();

            pPainter->setFont( f );
            rectName.moveTo( rectName.x(), rectName.y() - nLineHeight );
            rectKey.moveTo( rectKey.x(), rectKey.y() - nLineHeight );

            if ( rectName.top() < nY )
            {
                pPainter->drawText( QRectF( rectBounding.left(), rectBounding.bottom() - nLineHeight, rectBounding.width(), nLineHeight ), Qt::AlignCenter, "+" );
                pPainter->setPen( pen );
                pPainter->drawLine( rectBounding.left(), rectName.bottom(), rectBounding.right(), rectName.bottom() );
                pPainter->setPen( font.color );
                return;
            }

            pPainter->drawText( rectName, Qt::AlignLeft, pidentifier->getName() );
            if ( pidentifier->isPrimary() )
                pPainter->drawText( rectKey, Qt::AlignRight, "pi" );

        } // while

        pPainter->setPen( pen );
        pPainter->drawLine( rectBounding.left(), rectName.top(), rectBounding.right(), rectName.top() );
        pPainter->setPen( font.color );
    }
}

QSizeF LMEntityNormal::getOptimalSize()
{
    LMModel *    pModel = (LMModel*)getParent( "LMModel" );
    QFontMetrics FontMetrics( font.font );
    QSizeF       size;
    QSizeF       sizeCaption;
    QSizeF       sizeAttributes;
    int          nMargin         = 6;

    // CAPTION
    {
        font.font.setBold( true );
        QFontMetrics FontMetrics( font.font );
        sizeCaption.setWidth( FontMetrics.boundingRect( getName() ).width() );
        sizeCaption.setHeight( FontMetrics.height() + 2 );
        font.font.setBold( false );
    }

    // Attributes
    {
        int nWidthName     = 1;
        int nWidthDataType = 1;
        int nWidthKey      = 1;

        QList<ADObject*>    listAttributes = getObjects( QString(), QString(), "LMAttribute" );
        ADObject *          pObject;

        foreach( pObject, listAttributes )
        {
            LMAttribute *pattribute = (LMAttribute*)pObject;

            if ( FontMetrics.boundingRect( pattribute->getName() ).width() > nWidthName )
                nWidthName = FontMetrics.boundingRect( pattribute->getName() ).width();

            if ( FontMetrics.boundingRect( pattribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) ).width() > nWidthDataType )
                nWidthDataType = FontMetrics.boundingRect( pattribute->getDataTypeToDisplay( pModel->getDisplayFlags() ) ).width();

            if ( pattribute->getIdentifier() && pattribute->getMandatory() )
                nWidthKey = FontMetrics.boundingRect( "pi,M" ).width();
            else if ( pattribute->getIdentifier() && FontMetrics.boundingRect( "pi" ).width() > nWidthKey )
                nWidthKey = FontMetrics.boundingRect( "pi" ).width();
            else if ( pattribute->getMandatory() && FontMetrics.boundingRect( "M" ).width() > nWidthKey )
                nWidthKey = FontMetrics.boundingRect( "M" ).width();

        }

        nWidthName     += 4;
        nWidthDataType += 4;
        nWidthKey      += 4;

        sizeAttributes.setWidth( nWidthName + nWidthDataType + nWidthKey );
        sizeAttributes.setHeight( listAttributes.count() * (FontMetrics.height() + 2) );
    }

    // Identifiers
    {
        int nWidthName     = 1;
        int nWidthKey      = 1;

        QList<ADObject*>    listIdentifiers = getObjects( QString(), QString(), "LMIdentifier" );
        ADObject *          pObject;

        foreach( pObject, listIdentifiers )
        {
            LMIdentifier *pidentifier = (LMIdentifier*)pObject;

            if ( FontMetrics.boundingRect( pidentifier->getName() ).width() > nWidthName )
                nWidthName = FontMetrics.boundingRect( pidentifier->getName() ).width();

            if ( pidentifier->isPrimary() )
                nWidthKey = FontMetrics.boundingRect( "pi" ).width();
        }

        nWidthName     += 4;
        nWidthKey      += 4;

        if ( sizeAttributes.width() < (nWidthName + nWidthKey) )
            sizeAttributes.setWidth( nWidthName + nWidthKey );
        sizeAttributes.setHeight( sizeAttributes.height() + listIdentifiers.count() * (FontMetrics.height() + 2) );
    }

    // OPTIMAL SIZE
    size.setWidth( qMax( sizeCaption.width(), sizeAttributes.width() ) + nMargin );
    size.setHeight( sizeCaption.height() + sizeAttributes.height() + nMargin );

    return size;
}

void LMEntityNormal::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    if ( pHandle == vectorHandles.at( 0 ) )
    {
        if ( !pRectangle )
        {
            LMModel *pModel = (LMModel*)getParent( "LMModel" );
            pRectangle = (LMEntityNormal*)pModel->getObject( "LMEntityNormal" );
            pRectangle->setPos( pointScenePos );
            LMRelationship *pRelationship = (LMRelationship*)pModel->getObject( "LMRelationship" );  
            pRelationship->setPos( pointScenePos );
            pRelationship->setBegin( getPosCenter() );
            pRelationship->setEnd( pRectangle->getPosCenter() );
            pRelationship->doConnect( this, "Begin" );
            pRelationship->doConnect( pRectangle, "End" );
        }
        else pRectangle->setPos( pointScenePos );
        pHandle->setPos( pointScenePos );
    }
    else if ( pHandle == vectorHandles.at( 1 ) )
    { 
        if ( !pRectangle )
        {
            LMModel *pModel = (LMModel*)getParent( "LMModel" );
            pRectangle = (LMEntityAssociative*)pModel->getObject( "LMEntityAssociative" ); 
            pRectangle->setPos( pointScenePos );
            LMAssociativeLink *pAssociativeLink = (LMAssociativeLink*)pModel->getObject( "LMAssociativeLink" );  
            pAssociativeLink->setPos( pointScenePos );
            pAssociativeLink->setBegin( getPosCenter() );
            pAssociativeLink->setEnd( pRectangle->getPosCenter() );
            pAssociativeLink->doConnect( this, "Begin" );
            pAssociativeLink->doConnect( pRectangle, "End" );
        }
        else pRectangle->setPos( pointScenePos );
        pHandle->setPos( pointScenePos );
    }
}

void LMEntityNormal::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 2 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QPixmap( ":LM/Entity16x16" ).scaled( 16, 16 ) );
    vectorHandles[0] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );
    pHandle->setPixmap( QPixmap( ":LM/Association16x16" ).scaled( 16, 16 ) );
    vectorHandles[1] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}

void LMEntityNormal::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void LMEntityNormal::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[0];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[1];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );
}


