#ifndef H_FLConditionOpen
#define H_FLConditionOpen

#include "FLRectangle.h"

class FLDiagram;
class FLConditionClose;

class FLConditionOpen : public FLRectangle
{
    Q_OBJECT
public:
    FLConditionOpen( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLConditionOpen();

    // SETTERS
    virtual void setClose( FLConditionClose *p );
    virtual void setStretch( CBD::DirectionTypes nDir, qreal n = 0 );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Condition Open" ); }
#ifdef DEBUG_PROPDOCK
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
#endif
    virtual FLConditionClose *  getClose();
    virtual FLRectangle *       getNext( CBD::DirectionTypes nDir = CBD::DirectionUnknown );  
    virtual qreal               getFarthestX( CBD::DirectionTypes nDirStart, CBD::DirectionTypes nDir );
    virtual qreal               getStretch( CBD::DirectionTypes nDir );

    virtual bool                isEmpty();
    virtual bool                isEmpty( CBD::DirectionTypes nDir );

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    // amount (pixels) to stretch elbows when no content (default is 0)
    qreal nStretchLeft; 
    qreal nStretchRight; 

    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();

    virtual void doMenu( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doInitMenu();
    virtual void doFiniMenu();
    virtual void doSyncMenu();

    virtual qreal getFarthestX( CBD::DirectionTypes nDir, qreal nMax, FLRectangle *pNext );
};

#endif 

