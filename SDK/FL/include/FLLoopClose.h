#ifndef H_FLLoopClose
#define H_FLLoopClose

#include "FLRectangle.h"

class FLDiagram;
class FLLoopOpen;

class FLLoopClose : public FLRectangle
{
    Q_OBJECT
public:
    FLLoopClose( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLLoopClose();

    // SETTERS
    virtual void setOpen( FLLoopOpen *p );

    // GETTERS
    virtual QPixmap     getIcon();
    virtual QString     getFriendlyClassName() { return tr( "Loop Close" ); }
    virtual FLLoopOpen *getOpen();

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
};

#endif 

