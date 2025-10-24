#ifndef H_PMInherit
#define H_PMInherit

#include "PMLine.h"

#include "PMObject.h"
#include "PMDDLManager.h"

class PMModel;

class PMInherit : public PMLine, public PMDDLManager
{
    Q_OBJECT
public:
    PMInherit( PMModel *pParent );
    virtual ~PMInherit();

    // SETTERS

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Inherit" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );

    virtual QString             getCreateAuto() = 0;
    virtual QString             getDropAuto() = 0;

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doLoadDefaults();

    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
};


#endif


