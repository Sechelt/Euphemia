#ifndef H_LMEntityNormal
#define H_LMEntityNormal

#include "LMEntityBase.h"

class LMIdentifier;

class LMEntityNormal : public LMEntityBase
{
    Q_OBJECT
public:
    LMEntityNormal( LMModel *pmodel );
    virtual ~LMEntityNormal();

    // SETTERS
    virtual void setSelectType();
    
    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Entity" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual LMIdentifier *      getIdentifier( bool bPrimary = true );
    virtual ADObject *          getObject( const QString &s, ADObject *p = nullptr );

    // DO'RS
    virtual bool        doLoad( QDomElement *pdomElemTable );
    virtual void        doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    LMRectangle *pRectangle;    // created via shape menu and being moved... else nullptr

    virtual QSizeF getOptimalSize();

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();
};

#endif 


