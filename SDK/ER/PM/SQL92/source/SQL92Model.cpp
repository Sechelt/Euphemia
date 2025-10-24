#include "LibInfo.h"
#include "SQL92Model.h"

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

#include "SQL92Table.h"
#include "SQL92View.h"
#include "SQL92Sequence.h"
#include "SQL92Reference.h"
#include "SQL92Procedure.h"
#include "SQL92Domain.h"
#include "SQL92Rule.h"

#include "SQL92ModelEditorWidget.h"

SQL92Model::SQL92Model( ADObject *pParent )
    : PMModel( pParent )
{
    mapMeta.insert( "SQL92Table", ADMeta( QIcon(), "SQL92Table", "Table" ) );
    mapMeta.insert( "SQL92View", ADMeta( QIcon(), "SQL92View", "View" ) );
    mapMeta.insert( "SQL92Sequence", ADMeta( QIcon(), "SQL92Sequence", "Sequence" ) );
    mapMeta.insert( "SQL92Reference", ADMeta( QIcon(), "SQL92Reference", "Reference" ) );
    mapMeta.insert( "SQL92Procedure", ADMeta( QIcon(), "SQL92Procedure", "Procedure" ) );
    mapMeta.insert( "SQL92Domain", ADMeta( QIcon(), "SQL92Domain", "Domain" ) );
    mapMeta.insert( "SQL92Rule", ADMeta( QIcon(), "SQL92Rule", "Rule" ) );

    stringDatabaseType = "SQL92";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

SQL92Model::~SQL92Model()
{
}

ADObject *SQL92Model::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "SQL92Table" )
    {
        p = new SQL92Table( this );
        p->setFolder( tr( "Tables" ) );
    }
    else if ( stringClass == "SQL92View" )
    {
        p = new SQL92View( this );
        p->setFolder( tr( "Views" ) );
    }
    else if ( stringClass == "SQL92Sequence" )
    {
        p = new SQL92Sequence( this );
        p->setFolder( tr( "Sequences" ) );
    }
    else if ( stringClass == "SQL92Reference" )
    {
        p = new SQL92Reference( this );
        p->setFolder( tr( "References" ) );
    }
    else if ( stringClass == "SQL92Procedure" )
    {
        p = new SQL92Procedure( this );
        p->setFolder( tr( "Procedures" ) );
    }
    else if ( stringClass == "SQL92Domain" )
    {
        p = new SQL92Domain( this );
        p->setFolder( tr( "Domains" ) );
    }
    else if ( stringClass == "SQL92Rule" )
    {
        p = new SQL92Rule( this );
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
void SQL92Model::getValidationConfiguration()
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
ADValidationDatabase SQL92Model::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
   SQL92Domain::getValidationConfiguration( mapClasses );             
   SQL92Table::getValidationConfiguration( mapClasses );           
   SQL92Reference::getValidationConfiguration( mapClasses );         
   SQL92View::getValidationConfiguration( mapClasses );     
   SQL92Procedure::getValidationConfiguration( mapClasses );         
//   PMGDiagram::getValidationConfiguration( mapClasses );     
   return mapClasses;
}

bool SQL92Model::hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport )
{
    // this is verbose because new features are likely to come...
    switch ( nFeature )
    {
        case FeatureSequence:       return true;
        case FeatureDomain:         return true;
        case FeatureReference:      return true;
        case FeatureView:           return true;
        case FeaturePrimaryKey:     return true;
        case FeatureAlternateKey:   return true;
        case FeatureIndex:          return true;
        case FeatureProcedure:      return true;
        case FeatureTable:          return true;
    }

    return false;
}

void SQL92Model::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Annotation...                                                                                                                 
    stringValidation = tr("Annotation missing");                                                                         
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAnnotation().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "SQL92Model", nStrictness, stringValidation );                                                          
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

bool SQL92Model::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new SQL92ModelEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}


