#ifndef H_FLAsync
#define H_FLAsync

#include "FLRectangle.h"

class FLDiagram;

class FLAsync : public FLRectangle
{
    Q_OBJECT
public:
    FLAsync( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLAsync();

    // SETTERS

    // GETTERS
    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Async" ); }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
};

#endif 

