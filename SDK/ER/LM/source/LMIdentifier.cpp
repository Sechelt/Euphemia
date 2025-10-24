#include "LibInfo.h"
#include "LMIdentifier.h"

#include <ADTransaction.h>

#include "LMModel.h" 
#include "LMEntityNormal.h"
#include "LMRule.h"

#include <AWObjectNameWidget.h>
#include "LMIdentifierOptionsWidget.h"
#include "LMAttributesUsedWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

LMIdentifier::LMIdentifier( LMEntityNormal *pentity )
    : AWObject( pentity ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    bPrimary = false;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMIdentifier::~LMIdentifier()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif


#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*
void LMIdentifier::setName( const QString &s )              
{                                                           
    AWObject::setName( s );                                 
                                                            
    ((LMModel*)getParent( "LMModel" ))->setModified( true );
}                                                           
*/


void LMIdentifier::setPrimary( bool b ) 
{
    if ( bPrimary == b ) return;
    bPrimary = b;
    setModified();
}

/*!
    getIcon
    
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap LMIdentifier::getIcon()
{
//    return QPixmap( ":Identifier16x16" );
    return QPixmap( ":LM/ID16x16" );
}

AWDataWidget *LMIdentifier::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Identifier...") );
    
    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );
    p->addTab( new LMIdentifierOptionsWidget( this, p ), tr("Options") );
    p->addTab( new LMAttributesUsedWidget( this, p ), tr( "Attributes" ) );
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->addTab( pWidget, tr("Notes") );
    }
    p->addTab( new LMRulesUsedWidget( this, p ), tr("Rules") );
    p->addTab( new AWObjectUsesWidget( this, p ), tr("Uses") );
    p->addTab( new AWObjectUsedByWidget( this, p ), tr("Used By") );
    p->addTab( new AWObjectChildrenWidget( this, p ), tr("Children") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase LMIdentifier::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
/*
    ADValidation *           pproperty;                                                               
                                                                                                      
    switch ( nValidationLevel )                                                                       
    {                                                                                                 
    case nValidationLevelLow:                                                                         
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name missing" );       
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code missing" );       
        doInitValidationProperty( pproperty, "" );                                                    
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier comment missing" );    
        doInitValidationProperty( pproperty, "" );                                                    
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier description missing" );
        doInitValidationProperty( pproperty, "" );                                                    
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier attribute missing" );  
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
        break;                                                                                        
    case nValidationLevelMedium:                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code missing" );       
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier comment missing" );    
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier description missing" );
        doInitValidationProperty( pproperty, "" );                                                    
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name duplicate" );     
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code duplicate" );     
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier attribute missing" );  
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
        break;                                                                                        
    case nValidationLevelHigh:                                                                        
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name missing" );       
        doInitValidationProperty( pproperty, "Error" );                                               
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier comment missing" );    
        doInitValidationProperty( pproperty, "Warning" );                                             
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier description missing" );
        doInitValidationProperty( pproperty, "Information" );                                         
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier name duplicate" );     
        doInitValidationProperty( pproperty, "Error" );                                               
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier code duplicate" );     
        doInitValidationProperty( pproperty, "Error" );                                               
        plistValidations->append( pproperty );                                                        
                                                                                                      
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Identifier attribute missing" );  
        doInitValidationProperty( pproperty, "Error" );                                               
        plistValidations->append( pproperty );                                                        
        break;                                                                                        
    }                                                                                                 
*/
    return mapClasses;
}

/*!
    getEntity
    
    Return the entity we belong to.
*/
LMEntityNormal *LMIdentifier::getEntity()
{
    return (LMEntityNormal*)getParent( "LMEntityNormal" );
}

/*!
    doSave
    
    Save ourself to the XML.
*/
QDomElement LMIdentifier::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    
    doSaveLMObject( pdomDoc, &domElemThis );

    if ( isPrimary() ) domElemThis.setAttribute( "Primary", "1" );

    return domElemThis;
}

/*!
    doLoad
    
    Load ourself from the given XML.
*/
bool LMIdentifier::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );

    doLoadLMObject( pdomElemThis );

    bPrimary = pdomElemThis->attribute( "Primary", "0" ).toInt();

    return true;
}

/*!
    doValidate
    
    Validate ourself.
*/
void LMIdentifier::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString     stringCheck;                                                                                                          
    QString     stringCheckLevel;                                                                                                     
    LMModel *   pmodel = (LMModel*)getParent( "LMModel" );                                                                      
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Identifier name missing";                                                                                  
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck );                                         
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Identifier code missing";                                                                                  
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Identifier comment missing";                                                                               
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Identifier description missing";                                                                           
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
            stringCheckName         = "Identifier name duplicate";                                                                    
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Identifier code duplicate";                                                                    
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
                                                                                                                                      
    // Must have at least one Attribute...                                                                                            
    stringCheck         = "Identifier attribute missing";                                                                             
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getUses( QString(), QString(), "LMAttribute" ).count() < 1 )                                                             
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
*/
}

/*!
    doGoToView
    
    Invoke our standard view.
*/
void LMIdentifier::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}


