#ifndef H_SQL92Model
#define H_SQL92Model

#include <PMModel.h>

class SQL92Domain;
class SQL92Table;
class SQL92Reference;
class SQL92View;
class SQL92Procedure;

/*!
 * \class SQL92Model
 * \brief Entity Relationship: Physical Model (SQL92)
 *  
 */
class SQL92Model : public PMModel
{
    Q_OBJECT
public:
    SQL92Model( ADObject *pParent );
    virtual ~SQL92Model();

    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual void                getValidationConfiguration(); // entry point - someone (parent or app perhaps) calls this to load default configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this
    virtual bool                hasFeature( FeatureTypes nFeature, FeatureSupportTypes nSupport );

    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual bool slotOpenEditor();
};

#endif


