#include "LibInfo.h"
#include "PMTableNormal.h"

#include <DGraphicsProxyItem.h>
#include <DSource.h>

#include "PMModel.h" 
#include "PMColumn.h"
#include "PMKey.h"
#include "PMIndex.h"
#include "PMReference.h"
#include "PMLine.h"

#include "PMKeysWidget.h"
#include "PMIndexesWidget.h"
#include "PMDDLWidget.h"
#include "PMRulesUsedWidget.h"
#include "PMNotesWidget.h"
#include "PMColumnsWidget.h"

PMTableNormal::PMTableNormal( PMModel *pParent )
    : PMTableBase( pParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    pRectangle = nullptr;
    pLine = nullptr;
    pEOL = nullptr;

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

PMTableNormal::~PMTableNormal()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void PMTableNormal::setSelectType()
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

QPixmap PMTableNormal::getIcon()
{
    return QPixmap( ":PM/Table" );
}

AWDataWidget *PMTableNormal::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = PMTableBase::getObjectWidget( pWidgetParent );
    
    p->insertTab( 1, new PMRulesUsedWidget( this, p ), tr( "Rules" ));
    p->insertTab( 1, new PMNotesWidget( this, p ), tr("Notes") );
    p->insertTab( 1, new PMDDLWidget( this, getCreateAuto(), getDropAuto(), p ), tr("DDL") );
    // p->insertTab( 1, new PMAdditionalChecksWidget( this, p, getClientCheck(), getServerCheck() ), tr("Additional Checks") );
    if ( !getClass( "Index" ).isEmpty() ) p->insertTab( 1, new PMIndexesWidget( this, p ), tr("Indexs") );
    if ( !getClass( "Key" ).isEmpty() ) p->insertTab( 1, new PMKeysWidget( this, p ), tr("Keys") );
    p->insertTab( 1, new PMColumnsWidget( this, p ), tr("Columns") );

    p->setCurrentIndex( 0 );

    return p;
}

ADValidationDatabase PMTableNormal::getValidationConfiguration( ADValidationDatabase &mapClasses )
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

QString PMTableNormal::getCreateAuto()
{
    PMModel *pModel = (PMModel*)getParent( "PMModel" );

    QString     stringSQL          = "";
    QString     stringSQLComments  = "";                                                          
    QString     stringPrimaryKeys  = "";                                                          
                                                                                                  
    // TABLE                                                                                      
    stringSQL += "CREATE TABLE " + getName() + "\n(\n";                                           
                                                                                                  
    // COLUMNS                                                                                    
    QString stringDataTypeParams;                                                                 
    QString stringParam;                                                                          
                                                                                                  
    QList<ADObject*> l = getObjects( QString(), QString(), "PMColumn" );
    ADObject *pObject;
    foreach( pObject, l )
    {
        PMColumn *pColumn = (PMColumn*)pObject;
        
        if ( pObject != l.first() ) stringSQL += ",\n";                                                                   
                                                                                                  
        if ( pColumn->isCreateManual() )                                              
            stringSQL += "  " + pColumn->getCreateManual();                              
        else                                                                                      
            stringSQL += "  " + pColumn->getCreateAuto();                                          
                                                                                                  
        if ( pColumn->isPrimaryKey() )                                                           
        {                                                                                         
            if ( stringPrimaryKeys.isEmpty() ) stringPrimaryKeys = "  PRIMARY KEY (";                                            
            else stringPrimaryKeys += ",";                                                         
            stringPrimaryKeys += pColumn->getName();                                              
        }                                                                                         
                                                                                                  
    } // for columns                                                                              
                                                                                                  
    // put it together
    if ( pModel->hasFeature( PMModel::FeaturePrimaryKey, PMModel::FeatureSupportCreate ) )
    {
        if ( !stringPrimaryKeys.isEmpty() ) stringSQL += ",\n" + stringPrimaryKeys + ")";
    }
    stringSQL += "\n)";                                                                           
    stringSQL += ";\n";                                                                           
                                                                                                  
    // INDEXS                                                                                     
    l = getObjects( QString(), QString(), "PMIndex" );  
    foreach( pObject, l )
    {                                                                                             
        PMIndex *pIndex = (PMIndex*)pObject;
                                                                                                  
        if ( !pIndex->isIncludeCreate() ) continue;                                                                             
        if ( pIndex->isCreateManual() ) stringSQL += pIndex->getCreateManual();                                      
        else stringSQL += pIndex->getCreateAuto();                                                  
    }                                                                                             

    return stringSQL;
}

QString PMTableNormal::getDropAuto()
{
    QString stringSQL;

    // INDEXS                                                                                   
    QList<ADObject*> l = getObjects( QString(), QString(), "PMIndex" );
    ADObject *pObject;
    foreach( pObject, l )
    {
        PMIndex *pIndex = (PMIndex*)pObject;
                                                                                                   
        if ( !pIndex->isIncludeDrop() ) continue;                                                                           
                                                                                                
        if ( pIndex->isCreateManual() ) stringSQL += pIndex->getDropManual();                                      
        else stringSQL += pIndex->getDropAuto();                                                  
    }                                                                                           
                                                                                                
    stringSQL = "DROP TABLE " + getName() + ";\n";                                              

    return stringSQL;
}

void PMTableNormal::doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
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

void PMTableNormal::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    PMRectangle::mouseReleaseHandle( pHandle, pointScene );

    if ( pRectangle )
    { 
        doSyncMenu();
        getDiagram()->getSelectionManager()->setSelected( this, false );
        pRectangle = nullptr;
    }
    else if ( pLine )
    { 
        pLine->mouseReleaseHandle( pEOL, pointScene );
        doSyncMenu();
        getDiagram()->getSelectionManager()->setSelected( this, false );
        pLine = nullptr;
        pEOL = nullptr;
    }
}

void PMTableNormal::doMenu( DHandle *pHandle, const QPointF &pointScenePos )
{
    if ( pHandle == vectorHandles.at( 0 ) )
    {
        if ( !pRectangle )
        {
            PMModel *pModel = (PMModel*)getParent( "PMModel" );
            pRectangle = (PMTableNormal*)pModel->getObject( "Table" );
            Q_ASSERT(pRectangle);
            pRectangle->setPos( pointScenePos );
            PMReference *pReference = (PMReference*)pModel->getObject( "Reference" ); 
            Q_ASSERT(pReference); 
            pReference->setPos( pointScenePos );
            pReference->setBegin( getPosCenter() );
            pReference->setEnd( pRectangle->getPosCenter() );
            pReference->doConnect( this, "Begin" );
            pReference->doConnect( pRectangle, "End" );
            getDiagram()->getSelectionManager()->setSelected( pRectangle );
        }
        else pRectangle->setPos( pointScenePos );
        pHandle->setPos( pointScenePos );
    }
    else if ( pHandle == vectorHandles.at( 1 ) )
    {
        if ( !pLine )
        {
            PMModel *pModel = (PMModel*)getParent( "PMModel" );
            pLine = (PMReference*)pModel->getObject( "Reference" ); 
            Q_ASSERT(pLine); 
            getDiagram()->getSelectionManager()->setSelected( pLine );
            pLine->setPos( pointScenePos );
            {
                pEOL = pLine->getSource( CBD::EOLBegin )->getHandle();
                pLine->mousePressHandle( pEOL, pointScenePos );
                pLine->mouseMoveHandle( pEOL, getPosCenter() /*getPos()*/ );
                pLine->mouseReleaseHandle( pEOL, getPosCenter() /* getPos() */ );
                pLine->doConnect( this, "Begin" );
            }
            {
                pEOL = pLine->getSource( CBD::EOLEnd )->getHandle();
                pLine->mousePressHandle( pEOL, pointScenePos );
                pLine->mouseMoveHandle( pEOL, pointScenePos );
            }
        }
        else pLine->mouseMoveHandle( pEOL, pointScenePos );
    }
    else if ( pHandle == vectorHandles.at( 2 ) )
    {
        if ( !pLine )
        {
            PMModel *pModel = (PMModel*)getParent( "PMModel" );
            pLine = (PMReference*)pModel->getObject( "Reference" ); 
            Q_ASSERT(pLine); 
            getDiagram()->getSelectionManager()->setSelected( pLine );
            pLine->setPos( pointScenePos );
            {
                pEOL = pLine->getSource( CBD::EOLEnd )->getHandle();
                pLine->mousePressHandle( pEOL, pointScenePos );
                pLine->mouseMoveHandle( pEOL, getPosCenter() /*getPos()*/ );
                pLine->mouseReleaseHandle( pEOL, getPosCenter() /* getPos() */ );
                pLine->doConnect( this, "End" );
            }
            {
                pEOL = pLine->getSource( CBD::EOLBegin )->getHandle();
                pLine->mousePressHandle( pEOL, pointScenePos );
                pLine->mouseMoveHandle( pEOL, pointScenePos );
            }
        }
        else pLine->mouseMoveHandle( pEOL, pointScenePos );
    }
}

void PMTableNormal::doInitMenu()
{
    if ( vectorHandles.count() > 0 ) return;
    if ( !getProxy() ) return;

    vectorHandles.resize( 3 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( rect.topRight() ) );
    pHandle->setPixmap( QPixmap( ":PM/Table" ).scaled( 16, 16 ) );
    vectorHandles[0] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );
    pHandle->setPixmap( QPixmap( ":PM/Reference" ).scaled( 16, 16 ) );
    vectorHandles[1] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionMenu, DHandle::ConnectTypeNone, getProxy()->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 36 ) ) );
    pHandle->setPixmap( QPixmap( ":PM/ReferenceRotated" ).scaled( 16, 16 ) );
    vectorHandles[2] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}

void PMTableNormal::doFiniMenu()
{
    qDeleteAll( vectorHandles );
    vectorHandles.clear();
}

void PMTableNormal::doSyncMenu()
{
    if ( vectorHandles.count() < 1 ) return;
    if ( !getProxy() ) return;

    DHandle *pHandle;

    pHandle = vectorHandles[0];
    pHandle->setPos( pProxy->mapToScene( rect.topRight() ) );

    pHandle = vectorHandles[1];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 18 ) ) );

    pHandle = vectorHandles[2];
    pHandle->setPos( pProxy->mapToScene( QPoint( rect.topRight().x(), rect.topRight().y() + 36 ) ) );
}


