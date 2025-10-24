#ifndef H_DSpacerVertical
#define H_DSpacerVertical

#include "DSpacer.h"

class DSpacerVertical : public DSpacer
{
    Q_OBJECT
public:
    explicit DSpacerVertical( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DSpacerVertical();

    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Spacer Vertical" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );
};

#endif


