#ifndef H_DSpacerHorizontal
#define H_DSpacerHorizontal

#include "DSpacer.h"

class DSpacerHorizontal : public DSpacer
{
    Q_OBJECT
public:
    explicit DSpacerHorizontal( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DSpacerHorizontal();

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Spacer Horizontal" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );
};

#endif


