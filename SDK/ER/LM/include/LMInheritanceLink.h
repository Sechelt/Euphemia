#ifndef H_LMInheritanceLink
#define H_LMInheritanceLink

#include <LMLine.h>

class LMModel;

class LMInheritanceLink : public LMLine
{
    Q_OBJECT
public:
    LMInheritanceLink( LMModel *pmodelParent );
    virtual ~LMInheritanceLink();

    // SETTERS

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Inheritance Link" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual DSource *           getDerivedEntity();
    virtual DSource *           getBaseEntity();
    virtual DSource *           getEntity();
    virtual DSource *           getInheritance();

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doLoadDefaults();
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    
};


#endif


