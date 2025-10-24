#ifndef H_MSAModel
#define H_MSAModel

#include <PMModel.h>

class MSADomain;
class MSATable;
class MSAReference;
class MSAView;
class MSAProcedure;

/*!
 * \class MSAModel
 * \brief Entity Relationship: Physical Model (MSA)
 * 
 * This model is the best choice when working with MS Access. We are still working with 
 * ODBC but we use our knowledge of MS Access to do a better job. 
 *  
 * Examples; 
 *  
 * 1. DataTypes - We support MS Access specific DataTypes. 
 *  
 * 2. PrimaryKeys - Keys are not really supported. Calling SQLPrimaryKeys() is going to be an error. 
 *    However; a PrimaryKey is implied by a unique index named 'PrimaryKey'.
 *    So the model supports PrimaryKey's but its implementation is a unique index.
 *    This knowledge is used for ReverseEngineering and ForwardEngineering.
 *  
 * 3. ForeignKeys - Keys are not really supported. Calling SQLForeignKeys() is going to be an error. 
 *  
 * 4. Domains 
 *  
 * 5. Sequences 
 *  
 * 6. Views 
 *  
 * 7. Procedures 
 *  
 * \author pharvey (2020-06-06)
 */
class MSAModel : public PMModel
{
    Q_OBJECT
public:
    MSAModel( ADObject *pParent );
    virtual ~MSAModel();

    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual void                getValidationConfiguration(); // entry point - someone (parent or app perhaps) calls this to load default configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual bool                hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport );

    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual bool slotOpenEditor();

protected:
    virtual bool doLoadTable( DATAConnection *pConnection, PMTableNormal *pTable );
    virtual bool doLoadTableIndexs( DATAConnection *pConnection, PMTableNormal *pTable );
    virtual bool doLoadTableForeignKeys( DATAConnection *pConnection, PMTableNormal *pTable );

};

#endif


