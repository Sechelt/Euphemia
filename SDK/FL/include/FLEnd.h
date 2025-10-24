#ifndef H_FLEnd
#define H_FLEnd

#include "FLRectangle.h"

class FLDiagram;

class FLEnd : public FLRectangle
{
    Q_OBJECT
public:
    FLEnd( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLEnd();

    // SETTERS
    virtual void setSelectType() {}

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "End" ); }
    virtual FLRectangle *   getNext( CBD::DirectionTypes ) { return nullptr; }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();
};

#endif 

