#ifndef H_LMEntityAssociative
#define H_LMEntityAssociative

#include "LMEntityBase.h"

class LMModel;

/*!
    An Entity which only exists as a consequence of the relationship two other entities share.
*/
class LMEntityAssociative : public LMEntityBase
{
    Q_OBJECT
public:
    LMEntityAssociative( LMModel *pmodelParent );
    virtual ~LMEntityAssociative();

    // SETTERS
    virtual void setSelectType();

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Entity Associative" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses );

    // DO'rs
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    LMRectangle *pRectangle;    // created via shape menu and being moved... else nullptr

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();
};

#endif 

