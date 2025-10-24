#ifndef H_PMTableNormal
#define H_PMTableNormal

#include "PMTableBase.h"

class DSource;
class PMKey;
class PMLine;

class PMTableNormal : public PMTableBase
{
    Q_OBJECT
public:
    PMTableNormal( PMModel *pParent );
    virtual ~PMTableNormal();

    // SETTERS
    virtual void setSelectType();
    
    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Table" ); }
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'RS
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );

    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

protected:
    PMRectangle *   pRectangle;     // for doMenu
    PMLine *        pLine;          // for doMenu
    DHandle *       pEOL;           // for doMenu

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();
};

#endif 


