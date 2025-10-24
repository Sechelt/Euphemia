#ifndef H_SLModel
#define H_SLModel

#include <PMModel.h>

class SLDomain;
class SLTable;
class SLReference;
class SLView;
class SLProcedure;

/*!
 * \class SLModel
 * \brief Entity Relationship: Physical Model (SL)
 * 
 * This is for working with SQLite.
 * 
 * DATA TYPES
 *
 *      Online Documentation Reports
 *      https://www.tutorialspoint.com/sqlite/sqlite_data_types.htm
 *
 *          Data Type	          | Affinity
 *          ----------------------+------------
 *          INT                   |  INTEGER
 *          INTEGER
 *          TINYINT
 *          SMALLINT
 *          MEDIUMINT
 *          BIGINT
 *          UNSIGNED BIG INT
 *          INT2
 *          INT8
 *          
 *          CHARACTER(20)           TEXT
 *          VARCHAR(255)
 *          VARYING CHARACTER(255)
 *          NCHAR(55)
 *          NATIVE CHARACTER(70)
 *          NVARCHAR(100)
 *          TEXT
 *          CLOB
 *          
 *          BLOB                    NONE
 *          no datatype specified
 *          
 *          REAL                    REAL
 *          DOUBLE
 *          DOUBLE PRECISION
 *          FLOAT
 *          
 *          NUMERIC                 NUMERIC
 *          DECIMAL(10,5)
 *          BOOLEAN
 *          DATE
 *          DATETIME
 *
 *      ODBC Driver Reports
 *
 *          bigint
 *          bit
 *          char
 *          numeric
 *          float
 *          integer
 *          longvarbinary
 *          text
 *          smallint
 *          tinyint
 *          date
 *          time
 *          timestamp
 *          varbinary
 *          varchar
 *
 */


class SLModel : public PMModel
{
    Q_OBJECT
public:
    SLModel( AWObject *pParent );
    virtual ~SLModel();

    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual void                getValidationConfiguration(); // entry point - someone (parent or app perhaps) calls this to load default configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual bool                hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport );

    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual bool slotOpenEditor();
};

#endif


