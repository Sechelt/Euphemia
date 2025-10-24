#ifndef H_LMDomain
#define H_LMDomain

#include <AWObject.h>

#include <DATADataType.h>

#include "LMObject.h"

class LMModel;

class LMDomain : public AWObject, public LMObject
{
    Q_OBJECT
public:
    LMDomain( LMModel *pmodel );
    virtual ~LMDomain();

    // SETTERS
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

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Domain" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual DATADataType    getDataTypeObject()     { return DataType;                  }
    virtual QString         getDataType()           { return DataType.stringDataType;            }
    virtual QString         getLength()             { return DataType.stringLength;              }
    virtual QString         getPrecision1()         { return DataType.stringPrecision1;          }
    virtual QString         getPrecision2()         { return DataType.stringPrecision2;          }
    virtual QString         getScale()              { return DataType.stringScale;               }
    virtual QString         getMinimum()            { return stringMinimum;             }
    virtual QString         getMaximum()            { return stringMaximum;             }
    virtual QString         getDefault()            { return stringDefault;             }
    virtual QString         getFormat()             { return stringFormat;              }
    virtual QString         getUnit()               { return stringUnit;                }
    virtual bool            getForceUpperCase()     { return bForceUpperCase;           }
    virtual bool            getForceLowerCase()     { return bForceLowerCase;           }
    virtual bool            getRestrictModify()     { return bRestrictModify;           }
    virtual QStringList     getValues()             { return stringlistValues;          }
    virtual QString         getServerCheck()        { return stringServerCheck;         }
    virtual QString         getClientCheck()        { return stringClientCheck;         }
    virtual QString         getDataTypeWithArgs();
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

public slots:
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

protected:
    // General
    DATADataType    DataType;

    // Standard Checks
    QString         stringMinimum;
    QString         stringMaximum;
    QString         stringDefault;
    QString         stringFormat;
    QString         stringUnit;
    bool            bForceUpperCase;
    bool            bForceLowerCase;
    bool            bRestrictModify;
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


