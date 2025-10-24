#include "LibInfo.h"
#include "LMRule.h"

#include "LMModel.h" 

#include <AWObjectNameWidget.h>
#include "LMRuleWidget.h"
#include "LMNotesWidget.h"
#include "LMRulesUsedWidget.h"
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

LMRule::LMRule( LMModel *pmodel )
    : AWObject( pmodel ), LMObject( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    nRuleType = nRuleTypeDefinition;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

LMRule::~LMRule()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

/*!
    setName
    
    We replace this because we want to set the modify flag.
*/
void LMRule::setName( const QString &s )
{
    ADObject::setName( s );
}

/*!
    Return 16x16 icon for such things as browser (list view).
*/
QPixmap LMRule::getIcon()
{
    return QPixmap( ":LM/Rule16x16" );
}

AWDataWidget *LMRule::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Rule...") );
    
    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );
    p->addTab( new LMRuleWidget( this, p ), tr("Rule") );
    {
        LMNotesWidget *pWidget = new LMNotesWidget( p, getDescription(), getAnnotation() );
        connect( pWidget, SIGNAL(signalDescriptionChanged(const QString&)), SLOT(slotDescription(const QString &)) );
        connect( pWidget, SIGNAL(signalAnnotationChanged(const QString&)), SLOT(slotAnnotation(const QString &)) );
        p->addTab( pWidget, tr("Notes") );
    }
    p->addTab( new AWObjectUsesWidget( this, p ), tr("Uses") );
    p->addTab( new AWObjectUsedByWidget( this, p ), tr("Used By") );
    p->addTab( new AWObjectChildrenWidget( this, p ), tr("Children") );

    p->setCurrentIndex( 0 );

    return p;
}

/*!
    getValidations
    
    Return a list of validations we support. These should be globally unique.
*/
ADValidationDatabase LMRule::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
/*
    ADValidation *           pproperty;                                                         
                                                                                                
    switch ( nValidationLevel )                                                                 
    {                                                                                           
    case nValidationLevelLow:                                                                   
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name missing" );       
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code missing" );       
        doInitValidationProperty( pproperty, "" );                                              
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule comment missing" );    
        doInitValidationProperty( pproperty, "" );                                              
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule description missing" );
        doInitValidationProperty( pproperty, "" );                                              
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code duplicate" );     
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule not used" );           
        doInitValidationProperty( pproperty, "" );                                              
        plistValidations->append( pproperty );                                                  
        break;                                                                                  
    case nValidationLevelMedium:                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code missing" );       
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule comment missing" );    
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule description missing" );
        doInitValidationProperty( pproperty, "" );                                              
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name duplicate" );     
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code duplicate" );     
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule not used" );           
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
        break;                                                                                  
    case nValidationLevelHigh:                                                                  
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name missing" );       
        doInitValidationProperty( pproperty, "Error" );                                         
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code missing" );       
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule comment missing" );    
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule description missing" );
        doInitValidationProperty( pproperty, "Information" );                                   
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule name duplicate" );     
        doInitValidationProperty( pproperty, "Error" );                                         
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule code duplicate" );     
        doInitValidationProperty( pproperty, "Error" );                                         
        plistValidations->append( pproperty );                                                  
                                                                                                
        pproperty = new ADValidation( ADValidation::PropertyCombo, "Rule not used" );           
        doInitValidationProperty( pproperty, "Warning" );                                       
        plistValidations->append( pproperty );                                                  
        break;                                                                                  
    }                                                                                           
*/
    return mapClasses;
}

/*!
    Save ourself to the XML.
*/
QDomElement LMRule::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );
    
    doSaveLMObject( pdomDoc, &domElemThis );

    domElemThis.setAttribute( "RuleType", nRuleType );

    // Expression Client
    if ( !stringExpressionClient.isEmpty() )
    {
        unsigned long nLen = 0;
        domElemThis.setAttribute( "ExpressionClient", CBDPersistUtility::getEncodedText( stringExpressionClient, &nLen ) );
        domElemThis.setAttribute( "ExpressionClientSize", QString::number( nLen ) );
    }

    // Expression Server
    if ( !stringExpressionServer.isEmpty() )
    {
        unsigned long nLen = 0;
        domElemThis.setAttribute( "ExpressionServer", CBDPersistUtility::getEncodedText( stringExpressionServer, &nLen ) );
        domElemThis.setAttribute( "ExpressionServerSize", QString::number( nLen ) );
    }

    return domElemThis;
}

/*!
    Load ourself from the given XML.
*/
bool LMRule::doLoad( QDomElement *pdomElemThis )
{
    AWObject::doLoad( pdomElemThis );

    doLoadLMObject( pdomElemThis );

    // rule type
    nRuleType = (enumRuleTypes)pdomElemThis->attribute( "RuleType", "0" ).toInt();

    // Expression Client
    QString stringAttribute;
    stringAttribute = pdomElemThis->attribute( "ExpressionClientSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemThis->attribute( "ExpressionClient" );
        if ( !stringAttribute.isNull() )
            stringExpressionClient = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    // Expression Server
    stringAttribute = pdomElemThis->attribute( "ExpressionServerSize" );
    if ( !stringAttribute.isNull() )
    {
        ulong nLen = stringAttribute.toULong();

        stringAttribute = pdomElemThis->attribute( "ExpressionServer" );
        if ( !stringAttribute.isNull() )
            stringExpressionServer = CBDPersistUtility::getDecodedText( stringAttribute, nLen );
    }

    return true;
}

/*!
    doValidate
    
    Validate ourself.
*/
void LMRule::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString     stringCheck;                                                                                                          
    QString     stringCheckLevel;                                                                                                     
    LMModel *   pmodel = (LMModel*)getParent( "LMModel" );                                                                      
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Rule name missing";                                                                                        
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Rule code missing";                                                                                        
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Rule comment missing";                                                                                     
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Rule description missing";                                                                                 
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
            stringCheckName         = "Rule name duplicate";                                                                          
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Rule code duplicate";                                                                          
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
                                                                                                                                      
    // Not Used...                                                                                                                    
    stringCheck         = "Rule not used";                                                                                            
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( listUsedBy.count() < 1 )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
*/

}

/*!
    Invoke our standard view.
*/
void LMRule::doGoToView( ADObjectPathList, uint )
{
    slotObjectDialog();
}


