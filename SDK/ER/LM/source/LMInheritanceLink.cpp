#include "LibInfo.h"
#include "LMInheritanceLink.h"

#include <DRectangle.h>
#include <DSource.h>

#include "LMLineWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"

LMInheritanceLink::LMInheritanceLink( LMModel *pmodelParent )
    : LMLine( pmodelParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMInheritanceLink::~LMInheritanceLink()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap LMInheritanceLink::getIcon()
{
    return QPixmap( ":LM/InheritanceLink16x16" );
}

AWDataWidget *LMInheritanceLink::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = LMLine::getObjectWidget( pWidgetParent );

    // replace DLineWidget with LMLineWidget
    p->removeByClass( "DLineWidget" );
    p->insertTab( 1, new LMLineWidget( this, p ), tr("Line") );

    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->insertTab( 1, pWidget, tr("Notes") );
    }
    p->insertTab( 1, new LMRulesUsedWidget( this, p ), tr("Rules") );
//    p->insertTab( 1, new LMRelationshipWidget( this, Qt::Horizontal, p ), tr("Relationship") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *LMInheritanceLink::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = LMLine::getPropWidget( pWidgetParent );

    // replace 'Connector' added by DLine with 'Relationship'
    pPropWidget->removeWidget( tr("Connector") );                                                                                 

    // pPropWidget->addWidget( tr("Relationship"), new LMRelationshipWidget( this, Qt::Vertical, pPropWidget ) );                                                            

    return pPropWidget;
}

ADValidationDatabase LMInheritanceLink::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
/*
    ADValidation *           pproperty;                                                                     
                                                                                                            
    switch ( nValidationLevel )                                                                             
    {                                                                                                       
    case nValidationLevelLow:                                                                               
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link comment missing" );    
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link description missing" );
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link entity missing" );     
        doInitValidationProperty( pproperty, "Information" );                                               
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link inheritance missing" );
        doInitValidationProperty( pproperty, "Information" );                                               
        plistValidations->append( pproperty );                                                              
        break;                                                                                              
    case nValidationLevelMedium:                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link comment missing" );    
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link description missing" );
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link entity missing" );     
        doInitValidationProperty( pproperty, "Warning" );                                                   
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link inheritance missing" );
        doInitValidationProperty( pproperty, "Warning" );                                                   
        plistValidations->append( pproperty );                                                              
        break;                                                                                              
    case nValidationLevelHigh:                                                                              
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code missing" );       
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link comment missing" );    
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link description missing" );
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link name duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link code duplicate" );     
        doInitValidationProperty( pproperty, "" );                                                          
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link entity missing" );     
        doInitValidationProperty( pproperty, "Error" );                                                     
        plistValidations->append( pproperty );                                                              
                                                                                                            
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Inheritance Link inheritance missing" );
        doInitValidationProperty( pproperty, "Error" );                                                     
        plistValidations->append( pproperty );                                                              
        break;                                                                                              
    }                                                                                                       
*/
    return mapClasses;
}

/*!
 * \brief Get the source on the LMEntityNormal end and indicated as derived (with some verification).
 *  
 * Base is indicated by the line-end not having decoration (ie an arrow). 
 *  
 * The other end MUST be connected to a LMInheritance. 
 *  
 * Will return nullptr if failed any kind of verification. 
 *  
 * We return a \sa DSource because it provides quick access to line (\sa LMInheritanceLink) 
 * the entity (\sa LMEntityNormal) and the line-end (to change to/from arrow). 
 *  
 * \author pharvey (5/14/20)
 * 
 * \return DSource* 
 */
DSource *LMInheritanceLink::getDerivedEntity()
{
    DSource *pSource = getEntity();
    if ( !pSource ) return nullptr;

    if ( getStyle( pSource->getType() ) == W::WLineEndNone ) return pSource;

    return nullptr;
}

/*!
 * \brief Get the source on the LMEntityNormal end and indicated as base (with some verification).
 *  
 * Base is indicated by the line-end having an arrow. 
 *  
 * The other end MUST be connected to a LMInheritance. 
 *  
 * Will return nullptr if failed any kind of verification. 
 *  
 * We return a \sa DSource because it provides quick access to line (\sa LMInheritanceLink) 
 * the entity (\sa LMEntityNormal) and the line-end (to change to/from arrow). 
 *  
 * \author pharvey (5/14/20)
 * 
 * \return DSource* 
 */
DSource *LMInheritanceLink::getBaseEntity()
{
    DSource *pSource = getEntity();
    if ( !pSource ) return nullptr;

    if ( getStyle( pSource->getType() ) == W::WLineEndArrow ) return pSource;

    return nullptr;
}

/*!
 * \brief Get the source on the LMEntityNormal end (with some verification).
 *  
 * The other end MUST be connected to a LMInheritance. 
 *  
 * Will return nullptr if failed any kind of verification. 
 *  
 * We return a \sa DSource because it provides quick access to line (\sa LMInheritanceLink) 
 * the entity (\sa LMEntityNormal) and the line-end (to change to/from arrow). 
 *  
 * \author pharvey (5/14/20)
 * 
 * \return DSource* 
 */
DSource *LMInheritanceLink::getEntity()
{
    DSource *p = getInheritance();
    if ( !p ) return nullptr;

    if ( p->getType() == CBD::EOLBegin )
    { 
        DSource *pSource = getSource( CBD::EOLEnd );
        DRectangle *pRectangle = pSource->getRectangle();
        if ( pRectangle && pRectangle->inherits( "LMEntityNormal" ) ) return pSource;
    }
    else
    { 
        DSource *pSource = getSource( CBD::EOLBegin );
        DRectangle *pRectangle = pSource->getRectangle();
        if ( pRectangle && pRectangle->inherits( "LMEntityNormal" ) ) return pSource;
    }

    return nullptr;
}

/*!
 * \brief Get the source on the LMInheritance end (with some verification).
 * 
 * Will return nullptr if failed any kind of verification. 
 *  
 * \author pharvey (5/14/20)
 * 
 * \return DSource* 
 */
DSource *LMInheritanceLink::getInheritance()
{
    DSource *pSource = getSource( CBD::EOLBegin );
    DRectangle *pRectangle = pSource->getRectangle();
    if ( pRectangle && pRectangle->inherits( "LMInheritance" ) ) return pSource;

    pSource = getSource( CBD::EOLEnd );
    pRectangle = pSource->getRectangle();
    if ( pRectangle && pRectangle->inherits( "LMInheritance" ) ) return pSource;

    return nullptr;
}

QDomElement LMInheritanceLink::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = LMLine::doSave( pdomDoc, pdomElemParent );
    doSaveLMObject( pdomDoc, &domElemThis );

    return domElemThis;
}

bool LMInheritanceLink::doLoad( QDomElement *pdomElemThis )
{
    LMLine::doLoad( pdomElemThis );
    LMObject::doLoadLMObject( pdomElemThis );

    return true;
}

void LMInheritanceLink::doLoadDefaults()
{
/*
    CBDSettings         settings;                                                 
    QString             stringPath = "/CodeByDesign/LM/Defaults/InheritanceLink/";
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

void LMInheritanceLink::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);

/*
    QString     stringCheck;                                                                                                          
    QString     stringCheckLevel;                                                                                                     
    LMModel *   pmodel = (LMModel*)getParent( "LMModel" );                                                                      
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Inheritance Link name missing";                                                                            
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Inheritance Link code missing";                                                                            
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Inheritance Link comment missing";                                                                         
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Inheritance Link description missing";                                                                     
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
            stringCheckName         = "Inheritance Link name duplicate";                                                              
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Inheritance Link code duplicate";                                                              
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
                                                                                                                                      
    // Get pentity and pinheritance...                                                                                                
    DGObject *       pobjectAtEnd    = getObjectAtEnd();                                                                               
    DGObject *       pobjectAtBegin  = getObjectAtBegin();                                                                             
    LMEntityBase *  pentity         = 0;                                                                                              
    LMInheritance * pinheritance    = 0;                                                                                              
                                                                                                                                      
    if ( pobjectAtBegin )                                                                                                             
    {                                                                                                                                 
        if ( pobjectAtBegin->inherits( "LMEntityBase" ) )                                                                             
            pentity = (LMEntityBase*)pobjectAtBegin;                                                                                  
        else if ( pobjectAtBegin->inherits( "LMInheritance" ) )                                                                       
            pinheritance = (LMInheritance*)pobjectAtBegin;                                                                            
    }                                                                                                                                 
                                                                                                                                      
    if ( pobjectAtEnd )                                                                                                               
    {                                                                                                                                 
        if ( pobjectAtEnd->inherits( "LMEntityBase" ) )                                                                               
            pentity = (LMEntityBase*)pobjectAtEnd;                                                                                    
        else if ( pobjectAtEnd->inherits( "LMInheritance" ) )                                                                         
            pinheritance = (LMInheritance*)pobjectAtEnd;                                                                              
    }                                                                                                                                 
                                                                                                                                      
    // Entity?                                                                                                                        
    stringCheck         = "Inheritance Link entity missing";                                                                          
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( !pentity )                                                                                                               
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Inheritance?                                                                                                                   
    stringCheck         = "Inheritance Link inheritance missing";                                                                     
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( !pinheritance )                                                                                                          
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
*/
}

void LMInheritanceLink::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

/*
    DGObject *pobjectBegin = getObjectAtBegin();         
//    DGObject *pobjectEnd   = getObjectAtEnd();         
                                                        
    nEndArrowType = DLine::ArrowNormalSolid;            
    nEndArrowType = DLine::ArrowNone;                   
                                                        
    if ( pobjectBegin )                                 
    {                                                   
        if ( pobjectBegin->inherits( "LMGInheritance" ) )
            nEndArrowType   = DLine::ArrowNormalSolid;  
    }                                                   
*/

    LMLine::paint( pPainter, nOption, pWidget );
}


