#ifndef H_LMModel
#define H_LMModel

#include <DDiagram.h>

#include "LMObject.h"

// stuff we can create
class LMAssociativeLink;
class LMDataItem;
class LMDomain;
class LMEntityAssociative;
class LMEntityNormal;
class LMInheritance;
class LMInheritanceLink;
class LMRelationship;
class LMRule;
class LMGDiagram;

/*!
 * \class LMModel 
 * \brief An Entity Relationship Diagram showing Logical Model. 
 *  
 * Entity Relationships - for database design
 *  
 * Conceptual Model - very abstract system conceptualization 
 * Logical Model    - adds more detail to the model but remains vendor/product agnostic 
 * Physical Model   - adds more detail to the model including vendor/product specifics 
 *  
 * \author pharvey (1/10/20)
 */
class LMModel : public DDiagram, public LMObject
{
    Q_OBJECT
public:
    // attribute display options 
    // - Primarily for maintaining display option in diagram but needed here as; LMAttribute, LMDataItem and LMDomain need to understand
    //   the flags in order to produce the desired string for the diagram.
    enum DisplayFlags 
    { 
        DisplayDataType = 1, 
        DisplayDomain = 2,
        DisplayParams = 4, 
        DisplayDefault = 8,
    };

    explicit LMModel( ADObject *pParent, const QString &stringName = QString() );
    virtual ~LMModel();

    virtual void setDisplayFlag( int nFlag );

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "ERD Logical Model" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QMenu *         getContextMenu( QWidget * );
    virtual int             getDisplayFlags() { return nDisplayFlags; }
    virtual const char **   getDataTypes();
    virtual const char **   getDataTypeSyntax();
    virtual int             getDataType( const QString &stringDataType );
    virtual const char **   getDataTypeDescs();
    virtual QStringList     getDomains();
    virtual LMDomain *      getDomain( const QString str, const bool cs = false );
    virtual bool            getIsStandardDataType( const QString &string );
    virtual bool            getIsDomainDataType( const QString &string );
    virtual ADObject *      getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );

    // validation
    virtual void getValidationConfiguration();                                                  // clear then load our default validation configuration
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); // load default configuration - all classes have this

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doClearDisplayFlag( int nFlag );                   

signals:
    void signalCreatePhysicalModel( LMModel * );

public slots:
    virtual bool slotOpenEditor();
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
    virtual void slotCreatePhysicalModel();

protected:
    int nDisplayFlags; // attribute display options

    virtual ADObject *getObject( QDomElement *pdomElem );

    // DOERS
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual bool doDragDrop( const QStringList &stringList, const QPointF &pointScene );
};

#endif


