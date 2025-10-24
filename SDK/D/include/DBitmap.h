#ifndef H_DBitmap
#define H_DBitmap

#include <W.h>

#include "DRectangle.h"

class DBitmap : public DRectangle
{
    Q_OBJECT
public:
    explicit DBitmap( ADObject *pObjectParent, const QString &stringName = QString() );
    virtual ~DBitmap();
 
    // SETTERS
 
    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Bitmap" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual AWPropWidget *  getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QPixmap         getPixmap();
    virtual W::WImagePlacements getPlacement();

    // DO'RS
    virtual void paintAt( QPainter *pPainter, const QPointF &pointPos );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );

public slots:
    virtual void slotPixmap( const QPixmap &pixmap );
    virtual void slotPlacement( int n );

protected:
    QPixmap              pixmap;
    QPixmap              resizedPixmap;
    W::WImagePlacements  nPlacement;
 
    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
};
 
#endif


