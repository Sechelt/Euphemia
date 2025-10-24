#include "LibInfo.h"
#include "ODBCModel.h"

// #include <AWClassNameWidget.h>
// #include <AWDocumentNameWidget.h>
// #include <AWFactoryWidget.h>
// #include <AWObjectUsesWidget.h>
// #include <AWObjectUsedByWidget.h>
// #include <AWObjectChildrenWidget.h>

#include <DATAProfile.h>

#include <DGraphicsSceneProxy.h>

// #include <PMNotesWidget.h>

#include "ODBCDomain.h"
#include "ODBCTable.h"
#include "ODBCReference.h"
#include "ODBCView.h"
#include "ODBCProcedure.h"
#include "ODBCModelEditorWidget.h"

ODBCModel::ODBCModel( ADObject *pParent )
    : PMModel( pParent )
{
    // configure object factory
    // - do not start by clearing these - we want the objects added in PMModel
    // - We can create some objects that are not viable for the compliance level but are useful for documentation and visualization. These objects will
    // not be included in the DDL. For example; Domain and Reference.
    mapMeta.insert( "ODBCTable", ADMeta( QIcon(), "ODBCTable", "Table" ) );
    mapMeta.insert( "ODBCReference", ADMeta( QIcon(), "ODBCReference", "Reference" ) );
    mapMeta.insert( "ODBCDomain", ADMeta( QIcon(), "ODBCDomain", "Domain" ) );
}

ODBCModel::~ODBCModel()
{
}

ADObject* ODBCModel::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "ODBCTable" )
    {
        p = new ODBCTable( this );
        p->setFolder( tr( "Tables" ) );
    }
    else if ( stringClass == "ODBCSequence" )
    {
        p = new ODBCDomain( this );
        p->setFolder( tr( "Sequences" ) );
    }
    else if ( stringClass == "ODBCDomain" )
    {
        p = new ODBCDomain( this );
        p->setFolder( tr( "Domains" ) );
    }
    else if ( stringClass == "ODBCReference" )
    {
        p = new ODBCReference( this );
        p->setFolder( tr( "References" ) );
    }
    else if ( stringClass == "ODBCView" )
    {
        p = new ODBCView( this );
        p->setFolder( tr( "Views" ) );
    }
    else if ( stringClass == "ODBCProcedure" )
    {
        p = new ODBCProcedure( this );
        p->setFolder( tr( "Procedures" ) );
    }
    else
    {
        return PMModel::getObject( stringClass );
    }

    if ( !p )
    {
        doMessageBox( "WARNING", tr( "Create Object..." ), tr( "Unknown class requested " ) + s );
        return nullptr;
    }

    if ( p->inherits( "DObject" ) )
    {
        DObject *pObject = (DObject *)p;
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

DATAInfo *ODBCModel::getInfo( const QString &stringKey )
{ 
    return pProfile->mapInfo[stringKey]; 
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
void ODBCModel::getValidationConfiguration()
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
ADValidationDatabase ODBCModel::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
    ODBCDomain::getValidationConfiguration( mapClasses );
    ODBCTable::getValidationConfiguration( mapClasses );
    ODBCReference::getValidationConfiguration( mapClasses );
    ODBCView::getValidationConfiguration( mapClasses );
    ODBCProcedure::getValidationConfiguration( mapClasses );
//   PMGDiagram::getValidationConfiguration( mapClasses );
    return mapClasses;
}

bool ODBCModel::hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport )
{
    if ( !pProfile ) return false;

    switch (nFeature)
    {
        case FeatureTable:
            return true;
        case FeatureSequence:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return false;
            }
            break;
        case FeatureDomain:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                {
                    if ( !getInfo( "SQL_CREATE_DOMAIN" )->mapValues.contains( "SQL_CDO_CREATE_DOMAIN" ) ) return false;
                    return (getInfo( "SQL_CREATE_DOMAIN" )->mapValues)["SQL_CDO_CREATE_DOMAIN"]->vValue.toString() == "Y";
                }
                case FeatureSupportDrop:
                {
                    if ( !getInfo( "SQL_DROP_DOMAIN" )->mapValues.contains( "SQL_DD_DROP_DOMAIN" ) ) return false;
                    return (getInfo( "SQL_DROP_DOMAIN" )->mapValues)["SQL_DD_DROP_DOMAIN"]->vValue.toString() == "Y";
                }
            }
            break;
        case FeatureReference:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                {
                    if ( !getInfo( "SQL_ALTER_TABLE" )->mapValues.contains( "SQL_AT_ADD_CONSTRAINT" ) ) return false;
                    return (getInfo( "SQL_ALTER_TABLE" )->mapValues)["SQL_AT_ADD_CONSTRAINT"]->vValue.toString() == "Y";
                }
            }
            break;
        case FeatureView:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                case FeatureSupportCreate:
                {
                    if ( !getInfo( "SQL_CREATE_VIEW" )->mapValues.contains( "SQL_CV_CREATE_VIEW" ) ) return false;
                    return (getInfo( "SQL_CREATE_VIEW" )->mapValues)["SQL_CV_CREATE_VIEW"]->vValue.toString() == "Y";
                }
                case FeatureSupportDrop:
                {
                    if ( !getInfo( "SQL_DROP_VIEW" )->mapValues.contains( "SQL_DV_DROP_VIEW" ) ) return false;
                    return (getInfo( "SQL_DROP_VIEW" )->mapValues)["SQL_DV_DROP_VIEW"]->vValue.toString() == "Y";
                }
            }
            break;
        case FeaturePrimaryKey:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return false;
//                    return getInfo( "SQL_CREATE_TABLE" ).contains( "SQL_CT_TABLE_CONSTRAINT" );
//                    return getInfo( "SQL_ALTER_TABLE" ).contains( "SQL_AT_ADD_CONSTRAINT" );
            }
            break;
        case FeatureAlternateKey:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return false;
//                    return getInfo( "SQL_CREATE_TABLE" ).contains( "SQL_CT_TABLE_CONSTRAINT" );
//                    return getInfo( "SQL_ALTER_TABLE" ).contains( "SQL_AT_ADD_CONSTRAINT" );
            }
            break;
        case FeatureIndex:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                case FeatureSupportCreate:
                {
                    if ( !getInfo( "SQL_DDL_INDEX" )->mapValues.contains( "SQL_DI_CREATE_INDEX" ) ) return false;
                    return (getInfo( "SQL_DDL_INDEX" )->mapValues)["SQL_DI_CREATE_INDEX"]->vValue.toString() == "Y";
                }

                case FeatureSupportDrop:
                {
                    if ( !getInfo( "SQL_DDL_INDEX" )->mapValues.contains( "SQL_DI_DROP_INDEX" ) ) return false;
                    return (getInfo( "SQL_DDL_INDEX" )->mapValues)["SQL_DI_DROP_INDEX"]->vValue.toString() == "Y";
                }
            }
            break;
        case FeatureProcedure:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return getInfo( "SQL_PROCEDURES" )->vValue.toString() == "Y";
            }
            break;
    }

    return false;
}

void ODBCModel::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getName().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation);

    // Comment...
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getComment().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")");

    // Description...
    stringValidation = tr("Description missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getDescription().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")");

    // Annotation...
    stringValidation = tr("Annotation missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getAnnotation().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")");

    // Author...
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getAuthor().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")");

    // Author...
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity("ODBCModel", nStrictness, stringValidation);
    if (nSeverity != ADValidation::Ignore && getVer().isEmpty())                                                                                                eventOutputValidate(listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")");

    // check our children
    QList<ADObject *>    listChildren = getObjects(QString(), QString(), QString());
    ADObject *pObject;
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

bool ODBCModel::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new ODBCModelEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}


