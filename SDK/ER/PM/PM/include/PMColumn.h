#ifndef H_PMColumn
#define H_PMColumn

#include <AWObject.h>

#include <DATADataType.h>

#include "PMObject.h"
#include "PMDDLManager.h"

class PMTableBase;
class PMDomain;

/*!
 * \class PMColumn 
 * \brief A Column in a Table. 
 *  
 * This is similar to PMColumn. 
 * The majour difference is that PMColumn 'owns' its data definition. 
 * PMColumn can, optionally, rely on a PMDomain for some of its data definition. 
 * 
 * \author pharvey (1/10/20)
 */
class PMColumn : public AWObject, public PMObject, public PMDDLManager
{
    Q_OBJECT
public:
    PMColumn( PMTableBase *pParent );
    virtual ~PMColumn();

    // SETTERS
    virtual void setDomain( PMDomain *pdomain );      
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
    virtual void setMandatory( bool b );
    virtual void setHidden( bool b );
    virtual void setValues( const QStringList &s );  
    virtual void setServerCheck( const QString &s ); 
    virtual void setClientCheck( const QString &s ); 

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName()  { return tr( "Column" );            }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual DATADataType        getDataTypeObject()     { return DataType;                  }
    virtual DATADataTypeSpec *  getDataTypeSpec();                                          
    virtual QString             getDataTypeWithArgs();                                      
    virtual QString             getDataType()           { return DataType.stringDataType;   }  
    virtual QString             getDataTypeToDisplay( int nFlags );                         
    virtual QString             getLength()             { return DataType.stringLength;     }  
    virtual QString             getPrecision1()         { return DataType.stringPrecision1; }  
    virtual QString             getPrecision2()         { return DataType.stringPrecision2; }  
    virtual QString             getScale()              { return DataType.stringScale;      }  
    virtual PMDomain *          getDomain();
    virtual QString             getMinimum()            { return stringMinimum;             }
    virtual QString             getMaximum()            { return stringMaximum;             }
    virtual QString             getDefault()            { return stringDefault;             }
    virtual QString             getFormat()             { return stringFormat;              }
    virtual QString             getUnit()               { return stringUnit;                }
    virtual bool                isForceUpperCase()      { return bForceUpperCase;           }
    virtual bool                isForceLowerCase()      { return bForceLowerCase;           }
    virtual bool                isRestrictModify()      { return bRestrictModify;           }
    virtual bool                isMandatory()           { return bMandatory;                }
    virtual bool                isPrimaryKey();         
    virtual bool                isAlternateKey();       
    virtual bool                isForeignKey();         
    virtual bool                isHidden()              { return bHidden;                   }
    virtual QStringList         getValues()             { return stringlistValues;          }
    virtual QString             getServerCheck()        { return stringServerCheck;         }
    virtual QString             getClientCheck()        { return stringClientCheck;         }
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
    virtual void slotDomain( PMDomain *p )              { setDomain( p );       }
    virtual void slotDataType( const QString &s )       { setDataType( s );     }
    virtual void slotValues( const QStringList &l )     { setValues( l );       }
    virtual void slotClientCheck( const QString &s )    { setClientCheck( s );  }
    virtual void slotServerCheck( const QString &s )    { setServerCheck( s );  }
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    // General
    DATADataType        DataType;
    QVector<QString>    vectorParams;   // Data-type params (values) in same order as PMDataType.vParams. String so we check of null. \sa PMDataType

    // Standard Checks
    QString         stringMinimum;
    QString         stringMaximum;
    QString         stringDefault;
    QString         stringFormat;
    QString         stringUnit;
    bool            bForceUpperCase;
    bool            bForceLowerCase;
    bool            bRestrictModify;
    bool            bMandatory;
    bool            bHidden;
    QStringList     stringlistValues;
    // Additional Checks
    QString         stringServerCheck;
    QString         stringClientCheck;

    // SETTERS

    // GETTERS

    // DO'rs
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual bool doLoadValueList( QDomElement *pdomElemValueList );
};

#endif 



