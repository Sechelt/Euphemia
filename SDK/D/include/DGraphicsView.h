#ifndef H_DGraphicsView
#define H_DGraphicsView

#include <QtGui>
#include <CBD.h>

#include <WZoomWidget.h>

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"

class DGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    DGraphicsView( DGraphicsSceneProxy *pScene, QWidget *pParent = nullptr );

    qreal getOffsetH();
    qreal getScaleH();
    qreal getLengthH();

    qreal getOffsetV();
    qreal getScaleV();
    qreal getLengthV();

signals:
    void signalChangedH( qreal nOffset, qreal nScale, qreal nLength );
    void signalChangedV( qreal nOffset, qreal nScale, qreal nLength );

public slots:
    void slotZoomChanged( WZoomWidget::FitTypes nFit, int nZoom );
    void slotSceneRectChanged( const QRectF & );

protected:
    void setScale( qreal n );

    virtual void resizeEvent( QResizeEvent *pEvent );
    virtual void scrollContentsBy( int dx, int dy );
};

#endif


