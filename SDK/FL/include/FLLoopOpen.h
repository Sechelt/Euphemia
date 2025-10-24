#ifndef H_FLLoopOpen
#define H_FLLoopOpen

#include "FLRectangle.h"

class FLDiagram;
class FLLoopClose;

class FLLoopOpen : public FLRectangle
{
    Q_OBJECT
public:
    FLLoopOpen( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLLoopOpen();

    // SETTERS
    virtual void setClose( FLLoopClose *p );

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Loop Open" ); }
    virtual FLLoopClose *   getClose();

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
};

#endif 

