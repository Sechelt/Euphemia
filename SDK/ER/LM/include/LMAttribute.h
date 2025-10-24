#ifndef H_LMAttribute
#define H_LMAttribute

#include <AWObject.h>

#include "LMObject.h"
#include "LMDataItem.h"

class LMModel;
class LMEntityBase;

/*! 
 * \class LMAttribute 
 * \brief An Attribute of an Entity. 
 *  
 * This is largely dependent upon, and mapped to, a LMDataItem. 
 * The LMDataItem is cached in here but its the object reference to the LMDataItem that is important. 
 * In other words; LMAttribute does not 'own' its data definition... LMDataItem does. 
 * 
 * \author pharvey (1/10/20)
 */
class LMAttribute : public AWObject, LMObject
{
    Q_OBJECT
public:
    LMAttribute( LMEntityBase *pentity );
    virtual ~LMAttribute();

    // SETTERS
    virtual void setName( const QString &s );
    virtual void setComment( const QString &s );
    virtual void setCode( const QString &s );
    virtual void setDomain( LMDomain *pdomain ); // this should only come from LMDataItem
    virtual void setDataTypeObject( const DATADataType &DataType );
    virtual void setDataType( const QString &s );
    virtual void setLength( const QString &s );
    virtual void setPrecision1( const QString &s );
    virtual void setPrecision2( const QString &s );
    virtual void setScale( const QString &s );
    virtual void setMinimum( const QString &s );
    virtual void setMaximum( const QString &s );
    virtual void setDefault( const QString &s );
    virtual void setFormat( const QString &s );
    virtual void setUnit( const QString &s );
    virtual void setForceUpperCase( bool b );
    virtual void setForceLowerCase( bool b );
    virtual void setRestrictModify( bool b );
    virtual void setValues( const QStringList &s );
    virtual void setServerCheck( const QString &s );
    virtual void setClientCheck( const QString &s );
    virtual void setDescription( const QString &s );
    virtual void setAnnotation( const QString &s );
    virtual void setIdentifier( bool b );           
    virtual void setMandatory( bool b );
    virtual void setHidden( bool b );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Attribute" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual LMEntityBase *      getEntity();
    virtual LMDataItem *        getDataItem();
    virtual QString             getName();
    virtual QString             getComment();
    virtual QString             getCode();
    virtual QString             getDescription();
    virtual QString             getAnnotation();
    virtual DATADataType        getDataTypeObject();
    virtual QString             getDataType();
    virtual QString             getDataTypeToDisplay( int nFlags );
    virtual QString             getDataTypeWithArgs();
    virtual QString             getLength();
    virtual QString             getPrecision1();
    virtual QString             getPrecision2();
    virtual QString             getScale();
    virtual LMDomain *          getDomain();
    virtual bool                getIdentifier();
    virtual bool                getMandatory();
    virtual bool                getHidden();
    virtual QString             getMinimum();
    virtual QString             getMaximum();
    virtual QString             getDefault();
    virtual QString             getFormat();
    virtual QString             getUnit();
    virtual bool                getForceUpperCase();
    virtual bool                getForceLowerCase();
    virtual bool                getRestrictModify();
    virtual QStringList         getValues();
    virtual QString             getServerCheck();
    virtual QString             getClientCheck();
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doUsesAdd( ADObject *pmodel, const QString &stringName = QString() );
    virtual void doUsesRemove( ADObject *pmodel, const QString &stringName = QString() );
    // virtual void doUsedByAdd( ADObject *pmodel ) { ADObject::doUsedByAdd( pmodel ); }

public slots:
    virtual void slotDomain( LMDomain *p )              { setDomain( p );       }
    virtual void slotDataType( const QString &s )       { setDataType( s );     }
    virtual void slotLength( const QString &s )         { setLength( s );       }
    virtual void slotPrecision1( const QString &s )     { setPrecision1( s );   }
    virtual void slotPrecision2( const QString &s )     { setPrecision2( s );   }
    virtual void slotScale( const QString &s )          { setScale( s );        }
    virtual void slotValues( const QStringList &l )     { setValues( l );       }
    virtual void slotClientCheck( const QString &s )    { setClientCheck( s );  }
    virtual void slotServerCheck( const QString &s )    { setServerCheck( s );  }
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
    virtual void slotModifiedReference( ADObject * );

protected:
    // This is actually stored in listUses but we use it often (i.e. for painting) so
    // we also store it here (see doUsesAdd()) for faster reference.
    LMDataItem *    pDataItem;

    // These in addition to Data Item.
    bool            bIdentifier;
    bool            bMandatory;
    bool            bHidden;

    // SETTERS

    // GETTERS

    // DO'rs
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );

};

#endif 


