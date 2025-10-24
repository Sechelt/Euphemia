#ifndef H_DText
#define H_DText

#include "DRectangle.h"

class DText : public DRectangle
{
    Q_OBJECT
public:
    DText( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DText();

    virtual QPixmap getIcon();
    virtual QString getFriendlyClassName() { return tr( "Text" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

};

#endif 

