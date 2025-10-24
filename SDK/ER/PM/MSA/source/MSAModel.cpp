#include "LibInfo.h"
#include "MSAModel.h"

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
#include <PMReferenceColumn.h>

#include "MSATable.h"
#include "MSAView.h"
#include "MSASequence.h"
#include "MSAReference.h"
#include "MSAProcedure.h"
#include "MSADomain.h"
#include "MSARule.h"
#include "MSAKey.h"
#include "MSAIndex.h"
#include "MSAIndexColumn.h"

#include "MSAModelEditorWidget.h"

MSAModel::MSAModel( ADObject *pParent )
    : PMModel( pParent )
{
    mapMeta.insert( "MSATable", ADMeta( QIcon(), "MSATable", "Table" ) );
    mapMeta.insert( "MSAView", ADMeta( QIcon(), "MSAView", "View" ) );
    mapMeta.insert( "MSASequence", ADMeta( QIcon(), "MSASequence", "Sequence" ) );
    mapMeta.insert( "MSAReference", ADMeta( QIcon(), "MSAReference", "Reference" ) );
    mapMeta.insert( "MSAProcedure", ADMeta( QIcon(), "MSAProcedure", "Procedure" ) );
    mapMeta.insert( "MSADomain", ADMeta( QIcon(), "MSADomain", "Domain" ) );
    mapMeta.insert( "MSARule", ADMeta( QIcon(), "MSARule", "Rule" ) );

    stringDatabaseType = "MSA";
    pProfile = g_Profiles->getProfile( stringDatabaseType );
}

MSAModel::~MSAModel()
{
}

ADObject *MSAModel::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "MSATable" )
    {
        p = new MSATable( this );
        p->setFolder( tr( "Tables" ) );
    }
    else if ( stringClass == "MSAView" )
    {
        p = new MSAView( this );
        p->setFolder( tr( "Views" ) );
    }
    else if ( stringClass == "MSASequence" )
    {
        p = new MSASequence( this );
        p->setFolder( tr( "Sequences" ) );
    }
    else if ( stringClass == "MSAReference" )
    {
        p = new MSAReference( this );
        p->setFolder( tr( "References" ) );
    }
    else if ( stringClass == "MSAProcedure" )
    {
        p = new MSAProcedure( this );
        p->setFolder( tr( "Procedures" ) );
    }
    else if ( stringClass == "MSADomain" )
    {
        p = new MSADomain( this );
        p->setFolder( tr( "Domains" ) );
    }
    else if ( stringClass == "MSARule" )
    {
        p = new MSARule( this );
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
void MSAModel::getValidationConfiguration()
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
ADValidationDatabase MSAModel::getValidationConfiguration( ADValidationDatabase &mapClasses )
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
   MSADomain::getValidationConfiguration( mapClasses );             
   MSATable::getValidationConfiguration( mapClasses );           
   MSAReference::getValidationConfiguration( mapClasses );         
   MSAView::getValidationConfiguration( mapClasses );     
   MSAProcedure::getValidationConfiguration( mapClasses );         
//   PMGDiagram::getValidationConfiguration( mapClasses );     
   return mapClasses;
}

bool MSAModel::hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport )
{
    // this is verbose because new features are likely to come...
    switch ( nFeature )
    {
        case FeatureTable:
            return true;
        case FeatureSequence:
            return false;
        case FeatureDomain:
            switch ( nSupport )
            {
                case FeatureSupportUse:
                    return true;
                case FeatureSupportCreate:
                case FeatureSupportDrop:
                    return false;
            }
            break;
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
            return true;
    }

    return false;
}

void MSAModel::doValidate( ADValidation::Strictness /* nStrictness */, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    QString stringValidation;
    ADValidation::Severity nSeverity;

    // Name...                                                                                                                        
    stringValidation = tr("Name missing");
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getName().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation );                                         

    // Comment...                                                                                                                     
    stringValidation = tr("Comment missing");
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getComment().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Description...                                                                                                                 
    stringValidation = tr("Description missing");                                                                         
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getDescription().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Annotation...                                                                                                                 
    stringValidation = tr("Annotation missing");                                                                         
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAnnotation().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Author missing");
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
    if ( nSeverity != ADValidation::Ignore && getAuthor().isEmpty() )                                                                                                
        eventOutputValidate( listObjectPathItems, nSeverity, stringValidation + " (" + getName() + ")" );                

    // Author...                                                                                                                        
    stringValidation = tr("Version missing");
    nSeverity = getValidationSeverity( "MSAModel", nStrictness, stringValidation );                                                          
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

bool MSAModel::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    pEditorWidget = new MSAModelEditorWidget( this, getEditorWidgetParent() );

    if ( isCrossHairs() ) doCreateCrossHairs();

    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}

bool MSAModel::doLoadTable( DATAConnection *pConnection, PMTableNormal *pTable )
{
    bool b1 = doLoadTableColumns( pConnection, pTable );      // PMColumn's
    bool b2 = doLoadTableIndexs( pConnection, pTable );       // PMIndex's
    return ( b1 || b2 );
}

bool MSAModel::doLoadTableIndexs( DATAConnection *pConnection, PMTableNormal *pTable )
{
    DATAStatement *pStatement = pConnection->getIndexs( QString(), QString(), pTable->getName() );
    if ( !pStatement ) return false;

    PMKey *     pKey    = nullptr;
    PMIndex *   pIndex  = nullptr;
    QString stringIndexName;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString s = pStatement->getData( 6 ).toString();                // index name
        QString stringColumn = pStatement->getData( 9 ).toString();     // column name
        QString stringOrder = pStatement->getData( 10 ).toString();     // order (for column)

        if ( s.isEmpty() && stringColumn.isEmpty() )                    // spec says this index 'is a table' when this happens - we ignore
        {
            n = pStatement->doFetch();
            continue;
        }

        // new primary key?
        if ( s == "PrimaryKey" )
        {
            if ( !pKey )
            {
                pIndex = nullptr;
                stringIndexName = s;
                pKey = (PMKey*)pTable->getObject( "Key" );
                if ( !pKey ) break;
                pKey->setName( stringIndexName );
                pKey->setPrimary( true );
            }
        }
        // new index?
        else if (!pIndex || stringIndexName != s)                            
        {
            pKey = nullptr;
            stringIndexName = s;
            pIndex = (PMIndex*)pTable->getObject( "Index" );
            if ( !pIndex ) break;
            pIndex->setName( stringIndexName );
            pIndex->setUnique( pStatement->getData( 4 ).toInt() == SQL_FALSE );
        }

        // add column
        if ( pKey )
        {
            PMColumn *pColumn = pTable->getColumn( stringName );
            if ( !pColumn )
            {
                n = pStatement->doFetch();
                continue;
            }
            pKey->doAddColumn( pColumn );
        }
        else if ( pIndex )
        {
            PMIndexColumn *pColumn = (PMIndexColumn *)pIndex->getObject("Column");
            if ( !pColumn )
            {
                n = pStatement->doFetch();
                continue;
            }
            pColumn->setName( stringColumn );
            if ( stringOrder == "A" ) pColumn->setOrder( "ASC" );
            else if ( stringOrder == "D" ) pColumn->setOrder( "DESC" );
        }

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

/*!
 * \brief Load foreign keys for the table (if any). This will create references when applicable.
 *  
 * The default is to use SQLPrimaryKeys and/or SQLForeignKeys but MS Access does not support these calls. 
 * The PrimaryKey is derived from a unique index called 'PrimaryKey'. 
 * For ForeignKeys we need to query the MSysRelationships system table. There are issues with this that 
 * need to be solved by the User jumping to MS Access application and... 
 *  
 * 1. Make system tables visible. 
 *    This is done (in MS Access circa 2019) by;
 *    - right-click on 'All Access Objects'
 *    - select 'Navigation Options'
 *  
 * 2. Make a copy of MSysRelationships called MSysRelationshipsCopy. 
 *    This can be done by running this; SELECT * INTO MSysRelationshipsCopy FROM MSysRelationships;
 *  
 * \author pharvey (2020-06-08)
 * 
 * \param pConnection 
 * \param pTableChild 
 */
bool MSAModel::doLoadTableForeignKeys( DATAConnection *pConnection, PMTableNormal *pTableChild )
{
    QString stringSQL = "SELECT * FROM [MSysRelationshipsCopy] WHERE [MSysRelationshipsCopy].[szObject] = ";
    stringSQL += "'";
    stringSQL += pTableChild->getName();
    stringSQL += "'";
    stringSQL += " ORDER BY [szReferencedObject] ASC;";

    DATAStatement *pStatement = pConnection->getExecute( stringSQL );
    if ( !pStatement ) 
    {
        doMessageBox( "ERROR", tr("ForeignKeys"), tr("Query for ForeignKeys failed.\nDid you create a copy of MSysRelationships called MSysRelationshipsCopy to avoid access restriction on MSysRelationships?") );
        return false;
    }

    PMReference *pReference = nullptr;
    PMTableNormal *pTableParent = nullptr;

    QString stringPrimaryKeyTableName;
    QString stringForeignKeyName;

    SQLRETURN n = pStatement->doFetch();
    while ( SQL_SUCCEEDED( n ) )
    {
        QString stringPKCOLUMN_NAME = pStatement->getData( 6 ).toString();
        QString stringPKTABLE_NAME  = pStatement->getData( 7 ).toString();
        QString stringFKCOLUMN_NAME = pStatement->getData( 4 ).toString();
        QString stringFK_NAME       = pStatement->getData( 8 ).toString();

        if ( !pReference || ( stringPKTABLE_NAME != stringPrimaryKeyTableName || stringFK_NAME != stringForeignKeyName ) )
        {                                                                                                                 
            stringPrimaryKeyTableName   = stringPKTABLE_NAME;                                                             
            stringForeignKeyName        = stringFK_NAME;                                                                  
            pTableParent                 = nullptr;                                                                        
                                                                                                                          
            pReference = (PMReference*)getObject( "Reference" );                                                          
            if ( !pReference ) break;                                                                                     
            // connect the reference/line to the tables                                                                   
            QList<ADObject*> l = getObjects( stringPKTABLE_NAME, QString(), "PMTableNormal" );                            
            if ( l.count() )                                                                                              
            {                                                                                                             
                pTableParent = (PMTableNormal*)l.first();     
                pReference->setPos( pTableParent->getPosCenter() );                                                             
                // avoid having point-begin == point-end as it will cause crash when painting line-ends on win
                // besides - User would be confused by this
                // \todo this could still happen so need to figure that out in the paint method to handle other cases
                if ( pTableParent == pTableChild )
                {
                    pReference->setBegin( pTableChild->getPos() );      // top left                                                            
                    pReference->setEnd( pTableParent->getPosCenter() ); // center                                                            
                }
                else
                {
                    pReference->setBegin( pTableChild->getPosCenter() );                                                            
                    pReference->setEnd( pTableParent->getPosCenter() );                                                             
                }
                pReference->doConnect( pTableChild, "Begin" );                                                            
                pReference->doConnect( pTableParent, "End" );                                                             
            }                                                                                                             
        }                                                                                                                 
                                                                                                                          
        PMReferenceColumn *pColumn = (PMReferenceColumn*)pReference->getObject( "Column" );                               
        Q_ASSERT( pColumn );                                                                                              
                                                                                                                          
        if ( pTableParent ) pColumn->setParentColumn( pTableParent->getColumn( stringPKCOLUMN_NAME ) );                      
        pColumn->setChildColumn( pTableChild->getColumn( stringFKCOLUMN_NAME ) );                                       

        // carry on
        n = pStatement->doFetch();
    }
    delete pStatement;
    return true;
}

