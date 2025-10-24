#ifndef H_FLBegin
#define H_FLBegin

#include "FLRectangle.h"

class FLDiagram;

class FLBegin : public FLRectangle
{
    Q_OBJECT
public:
    FLBegin( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLBegin();

    // SETTERS

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Begin" ); }
    virtual FLRectangle *   getPrevious( CBD::DirectionTypes ) { return nullptr; }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );

protected:
    virtual void doSinksCreate();
    virtual void doSinksSync();
    virtual void doSinksSyncRaw();
};

#endif 

