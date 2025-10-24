#include "LibInfo.h"
#include "SLModel.h"

#include <AWClassNameWidget.h>
#include <AWDocumentNameWidget.h>
#include <AWFactoryWidget.h>
#include <AWObjectUsesWidget.h>
#include <AWObjectUsedByWidget.h>
#include <AWObjectChildrenWidget.h>

#include <DATAProfiles.h>
#include <DATAProfile.h>

#include <DGraphicsSceneProxy.h>

#include <PMNotesWidget.h>

#include "SLTable.h"
#include "SLView.h"
#include "SLSequence.h"
#include "SLReference.h"
#include "SLProcedure.h"
#include "SLDomain.h"
#include "SLRule.h"

#include "SLModelEditorWidget.h"

SLModel::SLModel( AWObject *pParent )
    : PMModel( pParent )
{
    mapMeta.insert( "SLTable", ADMeta( QIcon(), "SLTable", "Table" ) );
    mapMeta.insert( "SLView", ADMeta( QIcon(), "SLView", "View" ) );
    mapMeta.insert( "SLReference", ADMeta( QIcon(), "SLReference", "Reference" ) );
    mapMeta.insert( "SLDomain", ADMeta( QIcon(), "SLDomain", "Domain" ) );
    mapMeta.insert( "SLRule", ADMeta( QIcon(), "SLRule", "Rule" ) );

    stringDatabaseType = "SQLite";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

SLModel::~SLModel()
{
}

ADObject *SLModel::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "SLTable" )
    {
        p = new SLTable( this );
        p->setFolder( tr( "Tables" ) );
    }
    else if ( stringClass == "SLView" )
    {
        p = new SLView( this );
        p->setFolder( tr( "Views" ) );
    }
    else if ( stringClass == "SLSequence" )
    {
        p = new SLSequence( this );
        p->setFolder( tr( "Sequences" ) );
    }
    else if ( stringClass == "SLReference" )
    {
        p = new SLReference( this );
        p->setFolder( tr( "References" ) );
    }
    else if ( stringClass == "SLProcedure" )
    {
        p = new SLProcedure( this );
        p->setFolder( tr( "Procedures" ) );
    }
    else if ( stringClass == "SLDomain" )
    {
        p = new SLDomain( this );
        p->setFolder( tr( "Domains" ) );
    }
    else if ( stringClass == "SLRule" )
    {
        p = new SLRule( this );
        p->setFolder( tr( "Rules" ) );
    }
    else
    {
        // probably requested MarkUp object - let PMModel handle request
        return PMModel::getObject( stringClass );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", tr("Create Object..."), tr("Unknown class requested ") + s );
        return nullptr;
    }

    if ( p->inherits( "DObject" ) )
    {
        DObject *pObject = (DObject*)p; 
        DGraphicsProxyItem *pProxy = new DGraphicsProxyObject( pObject );
        pProxy->doCreateGraphicsObject();
        pProxy->setScene( pScene );
        pObject->setProxy( pProxy );
        pObject->setPos( QPointF( pScene->width() / 2, pScene->height() / 2 ) ); // to pos when created by AWFactoryWidget
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );
    // set parent modified by default (can be set back if this is a load)           
    setModified();              

    return p;
}

/*!
 * \brief Load validation configuration defaults. 
 *  
 * This is one of two ways to load the validation configuration. The 
 * other way is from XML when loading the model. The latter may be 
 * a User configured version. 
 * 
 * \author pharvey (11/29/19)
 */
void SLModel::getValidationConfiguration()
{
    mapValidationConfiguration.clear();
    getValidationConfiguration( mapValidationConfiguration );
}

/*!
 * \brief Load our validation data. 
 *  
 * Each class has a static method that is called to load its validation data. 
 * This does not do the validation just build the data representing all of the 
 * validations we can do. 
 *  
 * The validation level can then be configured. 
 *  
 * Each class will then access this data during a validation to determine what 
 * kind of a message (if any) to emit for a failed validation. 
 *  
 * \note This is maintained in the diagram as the diagram is the top level for 
 * running a validation. This may be a 'model' in the future - which will run 
 * the validation for all its diagrams. 
 * 
 * \author pharvey (11/28/19)
 * 
 * \param mapClasses 
 */
ADValidationDatabase SLModel::getValidationConfiguration( ADValidationDatabase &mapClasses )
{
    // add ourself
    ADValidationStrictness mapStrictness;
    // Low
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Information );
        mapStrictness.insert( ADValidation::Low, mapValidations );
    }

    // Medium:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Warning );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Warning );
        mapStrictness.insert( ADValidation::Medium, mapValidations );
    }

    // High:
    {
        ADValidationList mapValidations;
        mapValidations.insert( tr( "Name missing" ), ADValidation::Error );
        mapValidations.insert( tr( "Comment missing" ), ADValidation::Ignore );
        mapValidations.insert( tr( "Description missing" ), ADValidation::Information );
        mapValidations.insert( tr( "Author missing" ), ADValidation::Error );
        mapStrictness.insert( ADValidation::High, mapValidations );
    }
    mapClasses.insert( "PMModel", mapStrictness );

    // Top Level                                                                     
   SLDomain::getValidationConfiguration( mapClasses );             
   SLTable::getValidationConfiguration( mapClasses );           
   SLReference::getValidationConfiguration( mapClasses );         
   SLView::getValidationConfiguration( mapClasses );     
   SLProcedure::getValidationConfiguration( mapClasses );         
//   PMGDiagram::getValidationConfiguration( mapClasses );     
   return mapClasses;
}

bool SLModel::hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport )
{
    switch ( nFeature )
    {
        case FeatureTable:
            return true;
        case FeatureSequence:
            return false;
        case FeatureDomain:
        {
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return false;
            }
            break;
        }
        case FeatureReference:
            return true;
        case FeatureView:
            return true;
        case FeaturePrimaryKey:
            return true;
        case FeatureAlternateKey:
            return false;
        case FeatureIndex:
            return true;
        case FeatureProcedure:
            return false;
    }

    return false;
}

void SLModel::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Annotation...                                                                                                                 
    stringValidation = tr("Annotation missing");                                                                         
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAnnotation().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "SLModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getVer().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

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

bool SLModel::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new SLModelEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}


