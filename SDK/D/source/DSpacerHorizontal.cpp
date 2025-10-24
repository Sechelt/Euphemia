#include "LibInfo.h"
#include "DSpacerHorizontal.h"

DSpacerHorizontal::DSpacerHorizontal( ADObject *pObjectParent, const QString &stringName )
    : DSpacer( pObjectParent, stringName )
{
}

DSpacerHorizontal::~DSpacerHorizontal()
{
}

QPixmap DSpacerHorizontal::getIcon()
{
    return QPixmap( ":D/HSpacer" );
}

void DSpacerHorizontal::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    pPainter->setPen( pen );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    switch ( nSizeType )
    {
        case DSpacer::SizeExpanding:
        {
            qreal nY1   = rect.height() / 2 - 4;
            qreal nY2   = rect.height() / 2 + 4;
            qreal nStep = 3;

            for ( qreal nX = 0; nX < rect.width(); nX += nStep )
            {
                pPainter->drawLine( nX, nY1, nX + 1, nY2 );
            }
        }
        break;
        case DSpacer::SizeFixed:
        {
            qreal nY = rect.height() / 2;
            QPen penThick = pen;
            penThick.setWidth( 4 );
            pPainter->setPen( penThick );
            pPainter->drawLine( 0, nY, rect.width(), nY );
        }
        break;
    }
}

