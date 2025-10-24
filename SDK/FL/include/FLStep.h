#ifndef H_FLStep
#define H_FLStep

#include "FLRectangle.h"

class FLDiagram;

class FLStep : public FLRectangle
{
    Q_OBJECT
public:
    FLStep( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLStep();

    // SETTERS

    // GETTERS
    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Step" ); }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
};

#endif 

