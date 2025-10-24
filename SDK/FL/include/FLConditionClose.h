#ifndef H_FLConditionClose
#define H_FLConditionClose

#include "FLRectangle.h"

class FLDiagram;
class FLConditionOpen;

class FLConditionClose : public FLRectangle
{
    Q_OBJECT
public:
    FLConditionClose( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLConditionClose();

    // SETTERS
    virtual void setOpen( FLConditionOpen *p );
    virtual void setIgnore( bool b = true ) { bIgnore = b; }

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Condition Close" ); }
    virtual FLConditionOpen *   getOpen();
    virtual FLRectangle *       getPrevious( CBD::DirectionTypes nDir = CBD::DirectionUnknown );
    virtual bool                getIgnore() { return bIgnore; }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    bool bIgnore;
    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();
};

#endif 

