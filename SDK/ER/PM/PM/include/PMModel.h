#ifndef H_PMModel
#define H_PMModel

#include <DATADataType.h>
#include <DATASystem.h>
#include <DATAEnvironment.h>
#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATAProfileWidget.h>

#include <DDiagram.h>

#include "PMObject.h"
#include "PMDDLManager.h"
// #include "PMDataTypeWidget.h"

class LMModel;
class LMEntityBase;
class LMEntityNormal;
class LMDataItem;
class LMIdentifier;
class LMAttribute;

class PMTableNormal;
class PMView;
class PMReference;
class PMTableDerived;
class PMInherit;
class PMProcedure;
class PMDomain;
class PMSequence;
class PMRule;
class PMKey;
class PMColumn;

/*!
 * \class PMModel 
 * \brief An abstract base class for a Physical Model. 
 *  
 * Conceptual Model - very abstract system conceptualization 
 * Logical Model    - adds more detail to the model but remains vendor/product agnostic 
 * Physical Model   - adds more detail to the model including vendor/product specifics 
 *  
 * Standards and vendor specific Physical Models are derived from this. 
 *  
 * \author pharvey (1/10/20)
 */
class PMModel : public DDiagram, public PMObject, public PMDDLManager
{
    Q_OBJECT
//    friend class PMDataTypeWidget;
public:
    // column display options 
    // - Primarily for maintaining display option in diagram but needed here as; PMColumn, and PMDomain need to understand
    //   the flags in order to produce the desired string for the diagram.
    enum enumDisplayFlags
    {
        DisplayDataType = 1, 
        DisplayDomain = 2,
        DisplayParams = 4, 
        DisplayDefault = 8,
        DisplayNull = 16
    };

    enum FeatureTypes
    {
        FeatureTable,           // ie CREAT TABLE (this is always supported in an ERD)
        FeatureSequence,        // ie CREATE SEQUENCE
        FeatureDomain,          // ie CREATE DOMAIN
        FeatureReference,       // ie ALTER TABLE ADD CONSTRAINT FOREIGN KEY
        FeatureView,            // ie CREATE VIEW
        FeaturePrimaryKey,      // ie CREATE TABLE ( ... PRIMARY KEY(column,...) )
        FeatureAlternateKey,
        FeatureIndex,           // ie CREATE INDEX
        FeatureProcedure        // ie CREATE PROCEDURE
    };

    enum FeatureSupportTypes
    {
        FeatureSupportUse,         // can use object (may or may not support in DDL but we can at least use for documentation purposes)
        FeatureSupportCreate,      // supports create in DDL
        FeatureSupportDrop         // supports drop in DDL
    };

    PMModel( ADObject *pParent, const QString &stringName = QString() );
    virtual ~PMModel();

    virtual void setDisplayFlag( int nFlag );

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "ERD Physical Model" ); }
    virtual AWClassWidget *     getClassWidget( QWidget *pWidgetParent = nullptr );
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QMenu *             getContextMenu( QWidget * );
    virtual QList<QString>      getTypeNames();
    virtual DATADataTypeSpec *  getDataTypeSpec( const QString &stringTYPE_NAME );
    virtual QStringList         getDomains();
    virtual PMDomain *          getDomain( const QString str, const bool cs = false );
    virtual int                 getDisplayFlags() { return nDisplayFlags; }
    virtual bool                getIsStandardDataType( const QString &stringTYPE_NAME );
    virtual bool                getIsDomainDataType( const QString &string );
    virtual bool                getScriptCreateComments() { return bScriptCreateComments; }
    virtual bool                getScriptCreateScriptComments() { return bScriptCreateScriptComments; }
    virtual QString             getDatabaseType() { return stringDatabaseType; }
    virtual QString             getDatabaseName() { return stringDatabaseName; }
    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();
    virtual DATAProfile *       getProfile() { return pProfile; }
    virtual void                getValidationConfiguration(); // entry point - someone (parent or app perhaps) calls this to load default configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual bool                hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport ) = 0;
    virtual bool                isDataConnected();

    virtual void doSave( LMModel *pModel );
    virtual void doLoad( LMModel *pModel );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doClearDisplayFlag( int nFlag );                   
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual bool doDataConnect();
    virtual void doDataDisconnect();

public slots:
    virtual void slotReverseEngineer();
    virtual void slotForwardEngineer();
    virtual void slotCreateLogicalModel();
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    DATAProfile *       pProfile;
    DATASystem *        pSystem;
    DATAEnvironment *   pEnvironment;
    DATAConnection *    pConnection;
    int nPK = 0;        // Counter to create PrimaryKey names during reverse engineering.
    int nRF = 0;        // Counter to create Reference names during reverse engineering.
    int nDisplayFlags;  // column display options

    // General information about the driver and data source.
    // =====================================================
    // 
    // ODBC (Min, Core, Ext)
    //      - load this based upon the ODBC specification for given compliance level
    //      - use keys that are same as SQLGetInfo
    // ODBC (Drv)
    //      - nothing loaded until we get a connection
    //      - load this using DATAConnection::getInfo()
    //      - use keys that are same as SQLGetInfo but add additional as needed
    //        For example; add some with prefix SQL92_ because of SQL_SQL_CONFORMANCE
    //
    // \sa g_Profiles->mapProfiles[].mapInfo;

    // Data types supported by the data source.
    // ========================================
    //
    // ODBC (Min, Core, Ext)
    //      - load this based upon the ODBC doc for given compliance level
    //      - use SQLGetTypeInfo to load additional info if a connection provided
    // ODBC (Drv)
    //      - nothing loaded until we get a connection
    //      - load this using SQLGetTypeInfo 
    // OTHERS
    //      - probably load this based upon implicit knowledge of product/standard
    //      - DATADataTypeSpec could be extended to include additional fields as needed
    //
    // Example of key; "CHAR()", "VARBINARY", "CHAR() FOR BIT DATA". 
    //
    // \sa g_Profiles->mapProfiles[].mapDataTypes;

    // CLI functions and whether they are supported by DM/DRV
    // \sa g_Profiles->mapProfiles[].mapFunctions;

    // Create database options
    bool    bScriptCreateComments;
    bool    bScriptCreateScriptComments;

    // The database we want to work with. This may differ from values found in a connection; SQL_DATABASE_NAME.
    QString stringDatabaseName;
    // This is the type as we know it in the Model. This may differ from values found in a connection; SQL_DBMS_NAME, SQL_DBMS_VER. 
    // Used as Profile Key.
    QString stringDatabaseType;


    virtual bool doLoadTables( DATAConnection *pConnection, const QStringList &listTables, QPoint &pointCell );
    virtual bool doLoadViews( DATAConnection *pConnection, const QStringList &listViews, QPoint &pointCell );
    virtual bool doLoadProcedures( DATAConnection *pConnection, const QStringList &listProcedures );

    virtual bool doLoadForeignKeys( DATAConnection *pConnection );
    virtual bool doLoadTableForeignKeys( DATAConnection *pConnection, PMTableNormal *pTable );

    virtual bool doLoadTable( DATAConnection *pConnection, PMTableNormal *pTable );
    virtual bool doLoadTableColumns( DATAConnection *pConnection, PMTableNormal *pTable );
    virtual bool doLoadTablePrimaryKey( DATAConnection *pConnection, PMTableNormal *pTable );
    virtual bool doLoadTableIndexs( DATAConnection *pConnection, PMTableNormal *pTable );

    virtual bool doLoadViewColumns( DATAConnection *pConnection, PMView *pView );

    virtual void doSaveDomains( LMModel *pModel );
    virtual void doSaveTables( LMModel *pModel );
    virtual void doSaveReferences( LMModel *pModel );

    virtual void doSaveTableColumns( PMTableNormal *pTable, LMEntityNormal *pEntity );
    virtual void doSaveTableColumn( PMColumn *pColumn, LMEntityNormal *pEntity );
    virtual void doSaveTableColumn( PMColumn *pColumn, LMDataItem *pDataItem );
    virtual void doSaveTableColumn( PMColumn *pColumn, LMAttribute *pAttribute );

    virtual void doSaveTableKeys( PMTableNormal *pTable, LMEntityNormal *pEntity );
    virtual void doSaveTableKey( PMKey *pKey, LMIdentifier *pIdentifier );

    virtual void doLoadDomains( LMModel *pModel );
    virtual void doLoadTables( LMModel *pModel );
    virtual void doLoadAssociativeTables( LMModel *pModel );
    virtual void doLoadReferences( LMModel *pModel );

    virtual void doLoadTableColumns( LMEntityBase *pEntity, PMTableNormal *pTable );
    virtual void doLoadTableColumn( LMAttribute *pAttribute, PMTableNormal *pTable );

    virtual void doLoadTableKeys( LMEntityNormal *pEntity, PMTableNormal *pTable );
    virtual void doLoadTableKey( LMIdentifier *pIdentifier, PMKey *pKey );

    virtual DATADataType mapDataTypeToLogical( const DATADataType DataType );
    virtual DATADataType mapDataTypeToPhysical( const DATADataType DataType );

    // virtual ADObject *getObject( QDomElement *pdomElem );

    virtual bool doDragDrop( const QStringList &stringList, const QPointF &pointScene );

protected slots:
    virtual void slotMessage( DATAMessage * );
    virtual void slotDiagnostic( DATADiagnostic * );

};


class PMSelectObjectsWidget : public QWidget
{
    Q_OBJECT
public:
    PMSelectObjectsWidget( DATAConnection *p, QWidget *pParent );

    QStringList getTables();
    QStringList getViews();
    QStringList getProcedures();

protected:
    DATAConnection *pConnection;

    QListWidget *pTables;
    QListWidget *pViews;
    QListWidget *pProcedures;

    void doLoadTables();
    void doLoadViews();
    void doLoadProcedures();
    void doLoadForeignKeys();
};

class PMSelectObjectsDialog : public QDialog
{
    Q_OBJECT
public:
    PMSelectObjectsDialog( DATAConnection *p, QWidget *pParent );

    QStringList getTables()     { return pWidget->getTables(); }
    QStringList getViews()      { return pWidget->getViews(); }
    QStringList getProcedures() { return pWidget->getProcedures(); }

protected:
    PMSelectObjectsWidget *pWidget;

    void doSaveState();
    void doLoadState();

protected slots:
    void slotAccepted();
    void slotRejected();
};

#endif


