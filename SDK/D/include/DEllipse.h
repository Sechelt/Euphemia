#ifndef H_DEllipse
#define H_DEllipse

#include "DRectangle.h"

class DEllipse : public DRectangle
{
    Q_OBJECT
public:
    DEllipse( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DEllipse();

    // SETTERS

    // GETTERS
    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Ellipse" ); }

    // DO'RS
    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );
};

#endif 

