#ifndef H_DTitle
#define H_DTitle

#include "DRectangle.h"

class DTitle : public DRectangle
{
    Q_OBJECT
public:
    DTitle( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DTitle();

    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Title" ); }

    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

protected:
    virtual QString getFieldValue( const QString &s ); 
};

#endif 

