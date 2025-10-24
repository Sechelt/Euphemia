#ifndef H_LMInheritance
#define H_LMInheritance

#include "LMRectangle.h"
#include "LMInheritanceLink.h"

class LMModel;
class LMEntityBase;

class LMInheritance : public LMRectangle
{
    Q_OBJECT
public:
    LMInheritance( LMModel *pmodelParent );
    virtual ~LMInheritance();

    // SETTERS
    virtual void setSelectType();
    virtual void setMutuallyExclusive( bool b = true );
    virtual void setComplete( bool b = true );
    virtual void setBaseEntity( DSource *p );

    // GETTERS
    virtual QPixmap                     getIcon();
    virtual QString                     getFriendlyClassName() { return tr( "Inheritance" ); }
    virtual AWDataWidget *              getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *              getPropWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase         getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual bool                        isMutuallyExclusive() { return bMutuallyExclusive; }
    virtual bool                        isComplete() { return bComplete; }
    virtual DSource *                   getBaseEntity();
    virtual QVector<DSource*>           getDerivedEntities();
    virtual QVector<DSource*>           getEntities();
    virtual QVector<LMInheritanceLink*> getLinks();

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doLoadDefaults();
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

signals:
    void signalChangedBaseEntity( DSource * );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }    

protected:
    LMRectangle *pRectangle;    // created via shape menu and being moved... else nullptr
    bool bMutuallyExclusive;
    bool bComplete;

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();
};

#endif 

