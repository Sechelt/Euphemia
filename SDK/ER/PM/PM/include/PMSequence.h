#ifndef H_PMSequence
#define H_PMSequence

#include <AWObject.h>
#include "PMObject.h"
#include "PMDDLManager.h"

class PMModel;

class PMSequence : public AWObject, public PMObject, public PMDDLManager
{
    Q_OBJECT
public:
    PMSequence( PMModel *pParent );
    virtual ~PMSequence();

    // SETTERS
    virtual void setInc( const QString &s );
    virtual void setMin( const QString &s );
    virtual void setMax( const QString &s );
    virtual void setStart( const QString &s );
    virtual void setCache( const QString &s );
    virtual void setCycle( const QString &s );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Sequence" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );
    virtual QString             getInc()    { return stringInc;     }
    virtual QString             getMin()    { return stringMin;     }
    virtual QString             getMax()    { return stringMax;     }
    virtual QString             getStart()  { return stringStart;   }
    virtual QString             getCache()  { return stringCache;   }
    virtual QString             getCycle()  { return stringCycle;   }
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'RS
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemColumn );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

protected:
    // as strings so we can use empty to indicate not to use in DDL
    QString stringInc;
    QString stringMin;
    QString stringMax;
    QString stringStart;
    QString stringCache;
    QString stringCycle;    // bool true when !empty

    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
};

#endif 


