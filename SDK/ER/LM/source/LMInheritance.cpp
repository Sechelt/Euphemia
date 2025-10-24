#include "LibInfo.h"
#include "LMInheritance.h"

#include <ADTransaction.h>

#include <DGraphicsProxyItem.h>

#include "LMModel.h" 
#include "LMEntityNormal.h"
#include "LMInheritanceLink.h"
#include "LMRule.h"
#include "LMInheritanceWidget.h"
#include "LMRulesUsedWidget.h"
#include "LMNotesWidget.h"

LMInheritance::LMInheritance( LMModel *pmodelParent )
    : LMRectangle( pmodelParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    pRectangle = nullptr;
    bMutuallyExclusive  = false;
    bComplete           = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMInheritance::~LMInheritance()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void LMInheritance::setSelectType()
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

void LMInheritance::setMutuallyExclusive( bool b )
{
    if ( b == bMutuallyExclusive ) return;
    bMutuallyExclusive = b;
    setModified();
    getProxy()->update();
}

void LMInheritance::setComplete( bool b )
{
    if ( b == bComplete ) return;
    bComplete = b;
    setModified();
    getProxy()->update();
}

void LMInheritance::setBaseEntity( DSource *p )
{
    DSource *pSource = getBaseEntity();
    if ( pSource == p ) return;

    if ( pSource ) pSource->getLine()->setStyle( pSource->getType(), W::WLineEndNone );

    if ( p ) p->getLine()->setStyle( p->getType(), W::WLineEndArrow );

    emit signalChangedBaseEntity( p );
}

QPixmap LMInheritance::getIcon()
{
    return QPixmap( ":LM/Inheritance16x16" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties. 
 *  
 * Here we add \sa LMInheritanceWidget.
 *  
 * \author pharvey (5/12/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *LMInheritance::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMRectangle::getObjectWidget( pWidgetParent );

    p->insertTab( 1, new LMRulesUsedWidget( this, p ), tr("Rules") );
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->insertTab( 1, pWidget, tr("Notes") );
    }
    p->insertTab( 1, new LMInheritanceWidget( this, p ), tr("Inheritance") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *LMInheritance::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = LMRectangle::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Inheritance"), new LMInheritanceWidget( this, pPropWidget ) );                                                            

    return pPropWidget;

}

ADValidationDatabase LMInheritance::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
/*
    ADValidation *           pproperty;                                                                
                                                                                                       
    switch ( nValidationLevel )                                                                        
    {                                                                                                  
    case nValidationLevelLow:                                                                          
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name missing" );       
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code missing" );       
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance comment missing" );    
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance description missing" );
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance link missing" );       
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
        break;                                                                                         
    case nValidationLevelMedium:                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code missing" );       
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance comment missing" );    
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance description missing" );
        doInitValidationProperty( pproperty, "" );                                                     
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance link missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
        break;                                                                                         
    case nValidationLevelHigh:                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name missing" );       
        doInitValidationProperty( pproperty, "Error" );                                                
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance comment missing" );    
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance description missing" );
        doInitValidationProperty( pproperty, "Information" );                                          
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance name duplicate" );     
}
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance code duplicate" );     
        doInitValidationProperty( pproperty, "Warning" );                                              
        plistValidations->append( pproperty );                                                         
                                                                                                       
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance link missing" );       
        doInitValidationProperty( pproperty, "Error" );                                                
        plistValidations->append( pproperty );                                                         
        break;                                                                                         
    }                                                                                                  
*/
    return mapClasses;
}

/*!
 * \brief Get the base entity (if any).
 *  
 * The base entity is connected via a LMInheritanceLink which has an arrow. 
 * There should only be 0-1 of these but if there are more - this will return the first one. 
 * 
 * \author pharvey (5/14/20)
 * 
 * \return QList&lt;LMEntityBase*&gt; 
 */
DSource *LMInheritance::getBaseEntity()
{
    QVector<LMInheritanceLink*> vectorLinks = getLinks();
    LMInheritanceLink *pLink;
    foreach( pLink, vectorLinks )
    {
        DSource *pSource = pLink->getBaseEntity();
        if ( pSource ) return pSource;
    }
    return nullptr;
}

/*!
 * \brief Get a list of derived entities. 
 *  
 * Returns a list of pointers to DSource. Each DSource is at the end of a LMInheritanceLink and connected 
 * to a LMEntityNormal. The line-end has no decoration (arrow) so the LMEntityNormal is considered to be 
 * derived. 
 *  
 * We return a list of sources because it provides quick access to line (\sa LMInheritanceLink) 
 * the entity (\sa LMEntityNormal) and the line-end (to change to/from arrow). 
 * 
 * \author pharvey (5/14/20)
 * 
 */
QVector<DSource*> LMInheritance::getDerivedEntities()
{
    QVector<DSource*> v;

    QVector<LMInheritanceLink*> vectorLinks = getLinks();
    LMInheritanceLink *pLink;
    foreach( pLink, vectorLinks )
    {
        DSource *pSource = pLink->getDerivedEntity();
        if ( !pSource ) continue;
        v.append( pSource );
    }

    return v;
}

/*!
 * \brief Get a list of entities.
 *  
 * Returns a list of pointers to DSource. Each DSource is at the end of a LMInheritanceLink and connected 
 * to a LMEntityNormal. The result will include all entities (base and derived). 
 *  
 * We return a list of sources because it provides quick access to line (\sa LMInheritanceLink) 
 * the entity (\sa LMEntityNormal) and the line-end (to change to/from arrow). 
 *  
 * \author pharvey (5/14/20)
 * 
 * \return QList&lt;DSource*&gt; 
 */
QVector<DSource*> LMInheritance::getEntities()
{
    QVector<DSource*> v;

    QVector<LMInheritanceLink*> vectorLinks = getLinks();
    LMInheritanceLink *pLink;
    foreach( pLink, vectorLinks )
    {
        DSource *pSource = pLink->getEntity();
        if ( !pSource ) continue;
        v.append( pSource );
    }

    return v;
}

/*!
 * \brief Get a list of lines (\sa LMInheritanceLink only) attached to us.
 * 
 * \author pharvey (5/14/20)
 * 
 * \return QList&lt;LMInheritanceLink*&gt; 
 */
QVector<LMInheritanceLink*> LMInheritance::getLinks()
{
    QVector<LMInheritanceLink*> v;

    QVector<DSink*> vectorSinks = getSinks();
    DSink *pSink;
    foreach( pSink, vectorSinks )
    {
        QList<DSource*> listSources = pSink->getSources();
        DSource *pSource;
        foreach( pSource, listSources )
        {
            DLine *pLine = pSource->getLine();
            if ( !pLine->inherits( "LMInheritanceLink" ) ) continue;
            v.append( (LMInheritanceLink*)pLine );
        }
    }

    return v;
}

/*!
    Save ourself to the given XML.
*/    
QDomElement LMInheritance::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRectangle::doSave( pdomDoc, pdomElemParent );
    doSaveLMObject( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "MutuallyExclusive", QString::number( isMutuallyExclusive() ) );
    domElemThis.setAttribute( "Complete", QString::number( isComplete() ) );

    return domElemThis;
}

/*!
    Load ourself from the given XML.
*/    
bool LMInheritance::doLoad( QDomElement *pdomElemThis )
{
    DRectangle::doLoad( pdomElemThis );
    LMObject::doLoadLMObject( pdomElemThis );

    bMutuallyExclusive  = pdomElemThis->attribute( "MutuallyExclusive", QString::number( bMutuallyExclusive ) ).toInt();
    bComplete           = pdomElemThis->attribute( "Complete", QString::number( bComplete ) ).toInt();

    return true;
}

void LMInheritance::doLoadDefaults()
{
/*
    CBDSettings         settings;                                             
    QString             stringPath = "/CodeByDesign/LM/Defaults/Inheritance/";
    QPen                pen;                                                  
    QBrush              brush;                                                
                                                                              
                                                                              
    pen.setColor( "#0000ff" );                                                
    brush.setColor( "#9932cc" );                                              
    brush.setStyle( Qt::SolidPattern );                                       
    pen = settings.readLinePrefEntry( stringPath, pen );                      
    brush = settings.readFillPrefEntry( stringPath, brush);                   
                                                                              
    setPen(pen);                                                              
    setBrush( brush );                                                        
    setDefaultsLoaded( true );                                                
*/
}

void LMInheritance::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString     stringCheck;                                                                                                          
    QString     stringCheckLevel;                                                                                                     
    LMModel *   pmodel = (LMModel*)getParent( "LMModel" );                                                                      
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Inheritance name missing";                                                                                 
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck );                                         
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Inheritance code missing";                                                                                 
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Inheritance comment missing";                                                                              
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Inheritance description missing";                                                                          
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
            stringCheckName         = "Inheritance name duplicate";                                                                   
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Inheritance code duplicate";                                                                   
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
                                                                                                                                      
    // Need at least two links...                                                                                                     
    stringCheck         = "Inheritance link missing";                                                                                 
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        QList<ADObject*>          listHandles = getObjects( QString(), QString(), "DHandle" );                                       
        QPtrListIterator<ADObject>  iteratorHandles( listHandles );                                                                   
        DHandle *                   phandleSink;                                                                                      
        int                         nInheritanceLinks = 0;                                                                            
                                                                                                                                      
        while ( (phandleSink = (DHandle*)iteratorHandles.current()) )                                                                 
        {                                                                                                                             
            ++iteratorHandles;                                                                                                        
            if ( phandleSink->getHandleConnectType() == DHandle::Sink )                                                               
            {                                                                                                                         
                DHandle *phandleSource  = phandleSink->phandleGlued;                                                                  
                DGObject *pobject        = phandleSource->getParent();                                                                 
                                                                                                                                      
                if ( pobject->inherits( "LMInheritanceLink" ) )                                                                       
                {                                                                                                                     
//                    LMAssociativeLink *     passociativelink    = (LMAssociativeLink*)pobject;                                      
//                    LMEntityNormal *        pentitynormal       = passociativelink->getEntityNormal();                              
//                    LMEntityAssociative *   pentityassociative  = passociativelink->getEntityAssociative();                         
                    nInheritanceLinks++;                                                                                              
                                                                                                                                      
                                                                                                                                      
                }                                                                                                                     
            }                                                                                                                         
        }                                                                                                                             
                                                                                                                                      
        if ( nInheritanceLinks < 2 )                                                                                                  
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
*/
}

void LMInheritance::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    LMRectangle::mouseReleaseHandle( pHandle, pointScene );
    if ( !pRectangle ) return;
    doSyncMenu();
    pRectangle = nullptr;
}

void LMInheritance::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    QRectF r = boundingRect();

    pPainter->drawPie( r.x(), r.y(), r.width(), r.height() * 2, 0, (180 * 16) );
    r.moveTo( r.x() + 2, r.y() + 2 );
    r.setWidth( r.width() - 4 );
    r.setHeight( r.height() - 4 );

    if ( isMutuallyExclusive() )
    {
        QRectF r2( r.x(), r.y(), r.width() / 2, r.height() / 2 );
        r2.moveCenter( r.center() );
        pPainter->drawLine( r2.bottomLeft(), r2.topRight() ); 
        pPainter->drawLine( r2.topLeft(), r2.bottomRight() ); 
    }
    if ( isComplete() )
    { 
        pPainter->drawRoundedRect( r.left(), r.bottom() - 6, r.width(), 6, 2, 2 );
    }

    pPainter->setPen( font.color );
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( r, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( r, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( r, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            break;
    }
}

void LMInheritance::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    if ( pHandle == vectorHandles.at( 0 ) )
    {
        if ( !pRectangle )
        {
            LMModel *pModel = (LMModel*)getParent( "LMModel" );
            pRectangle = (LMEntityNormal*)pModel->getObject( "LMEntityNormal" );
            pRectangle->setPos( pointScenePos );
            LMInheritanceLink *pInheritanceLink = (LMInheritanceLink*)pModel->getObject( "LMInheritanceLink" );  
            pInheritanceLink->setPos( pointScenePos );
            pInheritanceLink->setBegin( getPos() );
            pInheritanceLink->setEnd( pointScenePos );
            pInheritanceLink->doConnect( this, "Begin" );
            pInheritanceLink->doConnect( pRectangle, "End" );
        }
        else pRectangle->setPos( pointScenePos );
        pHandle->setPos( pointScenePos );
    }
}

void LMInheritance::doInitMenu()
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

void LMInheritance::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void LMInheritance::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[0];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );
}


