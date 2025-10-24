#include "LibInfo.h"
#include "PMInherit.h"

#include <AWObjectNameWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include "PMModel.h"

#include "PMDDLWidget.h"
#include "PMNotesWidget.h"

PMInherit::PMInherit( PMModel *pParent )
    : PMLine( pParent ), PMDDLManager( this )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // override inherited value; make our details solid and allow User to change later
    // slate green theme
    font.color = QColor( Qt::black );
    pen.setColor( Qt::black );
    brush.setColor( "#637877" );
    brush.setStyle( Qt::SolidPattern );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMInherit::~PMInherit()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

QPixmap PMInherit::getIcon()
{
    return QPixmap( ":PM/Inherit" );
}

AWDataWidget *PMInherit::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = PMLine::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMInherit::getValidationConfiguration( ADValidationDatabase &mapClasses )
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

AWPropWidget *PMInherit::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = PMLine::getPropWidget( pWidgetParent );

    // remove 'Connector' added by DLine
    // - we can have an arrow on one end (when connected to parent) or nothing at all 
    //   (when connected to a child) and this will be set automatically
    pPropWidget->removeWidget( tr("Connector") );                                                                                 

    return pPropWidget;

}

QDomElement PMInherit::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = PMLine::doSave( pdomDoc, pdomElemParent );
    doSavePMDDLManager( pdomDoc, &domElemThis );

    return domElemThis;
}

bool PMInherit::doLoad( QDomElement *pdomElemThis )
{
    PMLine::doLoad( pdomElemThis );
    doLoadPMDDLManager( pdomElemThis );

    return true;
}

void PMInherit::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
/*
    QString     stringCheck;                                                                                                          
    QString     stringCheckLevel;                                                                                                     
    PMModel *   pmodel = (PMModel*)getParent( "PMModel" );                                                                      
                                                                                                                                      
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
    PMTableBase *  pentity         = 0;                                                                                              
    LMGInheritance * pinheritance    = 0;                                                                                              
                                                                                                                                      
    if ( pobjectAtBegin )                                                                                                             
    {                                                                                                                                 
        if ( pobjectAtBegin->inherits( "PMTableBase" ) )                                                                             
            pentity = (PMTableBase*)pobjectAtBegin;                                                                                  
        else if ( pobjectAtBegin->inherits( "LMGInheritance" ) )                                                                       
            pinheritance = (LMGInheritance*)pobjectAtBegin;                                                                            
    }                                                                                                                                 
                                                                                                                                      
    if ( pobjectAtEnd )                                                                                                               
    {                                                                                                                                 
        if ( pobjectAtEnd->inherits( "PMTableBase" ) )                                                                               
            pentity = (PMTableBase*)pobjectAtEnd;                                                                                    
        else if ( pobjectAtEnd->inherits( "LMGInheritance" ) )                                                                         
            pinheritance = (LMGInheritance*)pobjectAtEnd;                                                                              
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

void PMInherit::doLoadDefaults()
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

void PMInherit::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
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

    PMLine::paint( pPainter, nOption, pWidget );
}



