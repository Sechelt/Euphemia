#include "LibInfo.h"
#include "PMTableDerived.h"

#include "PMModel.h" 
#include "PMColumn.h"
#include "PMKey.h"

#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

PMTableDerived::PMTableDerived( PMModel *pParent )
    : PMTableBase( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMTableDerived::~PMTableDerived()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap PMTableDerived::getIcon()
{
    return QPixmap( ":PM/Table" );
}

AWDataWidget *PMTableDerived::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = PMTableBase::getObjectWidget( pWidgetParent );
    return p;
}

ADValidationDatabase PMTableDerived::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    /*
        ADValidation * pproperty;                                                                            

        switch ( nValidationLevel )                                                                          
        {                                                                                                    
        case nValidationLevelLow:                                                                            
            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name missing" );              
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code missing" );              
            doInitValidationProperty( pproperty, "" );                                                       
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table comment missing" );           
            doInitValidationProperty( pproperty, "" );                                                       
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table description missing" );       
            doInitValidationProperty( pproperty, "" );                                                       
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name duplicate" );            
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code duplicate" );            
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table attribute missing" );         
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table primary identifier missing" );
            doInitValidationProperty( pproperty, "" );                                                       
            plistValidations->append( pproperty );                                                           
            break;                                                                                           
        case nValidationLevelMedium:                                                                         
            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name missing" );              
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code missing" );              
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table comment missing" );           
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table description missing" );       
            doInitValidationProperty( pproperty, "" );                                                       
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name duplicate" );            
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code duplicate" );            
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table attribute missing" );         
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table primary identifier missing" );
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           
            break;                                                                                           
        case nValidationLevelHigh:                                                                           
            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name missing" );              
            doInitValidationProperty( pproperty, "Error" );                                                  
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code missing" );              
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table comment missing" );           
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table description missing" );       
            doInitValidationProperty( pproperty, "Information" );                                            
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table name duplicate" );            
            doInitValidationProperty( pproperty, "Error" );                                                  
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table code duplicate" );            
            doInitValidationProperty( pproperty, "Error" );                                                  
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table attribute missing" );         
            doInitValidationProperty( pproperty, "Error" );                                                  
            plistValidations->append( pproperty );                                                           

            pproperty = new ADValidation( ADValidation::PropertyCombo, "Table primary identifier missing" );
            doInitValidationProperty( pproperty, "Warning" );                                                
            plistValidations->append( pproperty );                                                           
            break;                                                                                           
        }                                                                                                    
    */
    return mapClasses;
}

/*!
    getKey
    
    Return the first key matching bPrimary.
*/
/*
PMKey * PMTableDerived::getKey( bool bPrimary )                                          
{                                                                                        
    {                                                                                    
        QList<ADObject*>          listKeys = getObjects( QString(), QString(), "PMKey" );
        ADObject *pObject;                                                               
                                                                                         
        foreach( pObject, listKeys )                                                     
        {                                                                                
            PMKey *pKey = (PMKey*)pObject;                                               
            if ( pKey->isPrimary() == bPrimary )                                         
                return pKey;                                                             
        }                                                                                
    }                                                                                    
                                                                                         
    return 0;                                                                            
}                                                                                        
*/

bool PMTableDerived::doLoad( QDomElement *pdomElemThis )
{
    PMTableBase::doLoad( pdomElemThis );

    return true;
}

void PMTableDerived::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString stringCheck;                                                                                                              
    QString stringCheckLevel;                                                                                                         
    PMModel *pmodel = (PMModel*)getParent( "PMModel" );                                                                         
                                                                                                                                      
    // Name...                                                                                                                        
    stringCheck         = "Table name missing";                                                                                      
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getName().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Code...                                                                                                                        
    stringCheck         = "Table code missing";                                                                                      
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getCode().isEmpty() )                                                                                                    
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Comment...                                                                                                                     
    stringCheck         = "Table comment missing";                                                                                   
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        if ( getComment().isEmpty() )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Description...                                                                                                                 
    stringCheck         = "Table description missing";                                                                               
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
            stringCheckName         = "Table name duplicate";                                                                        
            stringCheckLevelName    = pmodel->getValidationSeverity( stringCheckName );                                          
            if ( !stringCheckLevelName.isEmpty() )                                                                                    
                pbCheckName = &bCheckName;                                                                                            
        }                                                                                                                             
                                                                                                                                      
        if ( !getCode().isEmpty() )                                                                                                   
        {                                                                                                                             
            stringCheckCode         = "Table code duplicate";                                                                        
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
                                                                                                                                      
    // Column missing...                                                                                                           
    stringCheck         = "Table attribute missing";                                                                                 
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        QList<ADObject*> listModels =  getObjects( QString(), QString(), "PMColumn" );                                            
                                                                                                                                      
        if ( listModels.count() < 1 )                                                                                                 
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 
                                                                                                                                      
    // Key...                                                                                                                  
    stringCheck         = "Table primary identifier missing";                                                                        
    stringCheckLevel    = pmodel->getValidationSeverity( stringCheck );                                                          
    if ( !stringCheckLevel.isEmpty() )                                                                                                
    {                                                                                                                                 
        QList<ADObject*>          listChildren = getObjects( QString(), QString(), "PMKey" );                                 
        QPtrListIterator<ADObject>  iteratorChildren( listChildren );                                                                 
        ADObject *                  pmodel;                                                                                           
        PMKey *              pKey;                                                                                      
                                                                                                                                      
        while ( (pmodel = iteratorChildren.current()) )                                                                               
        {                                                                                                                             
            ++iteratorChildren;                                                                                                       
            pKey = (PMKey*)pmodel;                                                                                      
            if ( pKey->getPrimary() )                                                                                          
                break;                                                                                                                
        }                                                                                                                             
                                                                                                                                      
        if ( !pmodel )                                                                                                                
            eventOutputValidate( listObjectPathItems, stringCheckLevel, stringCheck + " (" + getName() + ")" );                
    }                                                                                                                                 

    // doValidate all children
    ADObject::doValidate( nValueType, listObjectPathItems );
*/
}


