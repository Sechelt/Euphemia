#ifndef H_FLSync
#define H_FLSync

#include "FLRectangle.h"

class FLDiagram;

class FLSync : public FLRectangle
{
    Q_OBJECT
public:
    FLSync( FLDiagram *pDiagram, const QString &stringName = QString() );
    virtual ~FLSync();

    // SETTERS

    // GETTERS
    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Sync" ); }

    // DO'RS
    virtual void paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget );
};

#endif 

