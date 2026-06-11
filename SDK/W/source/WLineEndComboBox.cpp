/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WLineEndComboBox.h"

WLineEndComboBox::WLineEndComboBox( W::WLineEndStyles n, QWidget *pParent )
    : QComboBox( pParent )
{
    addItem( "None", (int)W::WLineEndNone  );
    setItemIcon( 0, getIcon( W::WLineEndNone ) );

    addItem( "Arrow", (int)W::WLineEndArrow );
    setItemIcon( 1, getIcon( W::WLineEndArrow ) );

    addItem( "Ball", (int)W::WLineEndBall );
    setItemIcon( 2, getIcon( W::WLineEndBall ) );

    setValue( n );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );
}

void WLineEndComboBox::setValue( W::WLineEndStyles n )
{
    setCurrentIndex( findData( n, int(Qt::UserRole) ) );
}

W::WLineEndStyles WLineEndComboBox::getValue()
{
    return (W::WLineEndStyles)itemData( currentIndex(), Qt::UserRole ).toInt();
}

QPixmap WLineEndComboBox::getIcon( W::WLineEndStyles nStyle, int nWidth, int nHeight )
{
    QPixmap pixmap( nWidth, nHeight );
    QPainter painter( &pixmap );

    QPen pen;
    QBrush brush;

    pen.setStyle( Qt::SolidLine );
    pen.setColor( Qt::black );
    brush.setStyle( Qt::SolidPattern );
    brush.setColor( Qt::white );

    painter.setPen( pen );
    painter.setBrush( brush );

    painter.fillRect( 0, 0, nWidth, nHeight, brush );

    painter.setBrush( Qt::black );

    QLineF line( 0, nHeight / 2, nWidth, nHeight / 2 );
    painter.drawLine( line );

    switch ( nStyle ) 
    {
        case W::WLineEndArrow:
        {
            qreal arrowSize = 10;
            const qreal Pi = 3.14;
            double angle = ::acos( line.dx() / line.length() );

            if ( line.dy() >= 0 )
                angle = (Pi * 2) - angle;

            QPointF arrowP1 = line.p1() + QPointF( sin( angle + Pi / 3 ) * arrowSize,
                                                   cos( angle + Pi / 3 ) * arrowSize );
            QPointF arrowP2 = line.p1() + QPointF( sin( angle + Pi - Pi / 3 ) * arrowSize,
                                                   cos( angle + Pi - Pi / 3 ) * arrowSize );

            QPolygonF polygonArrow;
            polygonArrow << line.p1() << arrowP1 << arrowP2;
            painter.drawPolygon( polygonArrow );
        }
        break;
        case W::WLineEndBall:
            painter.drawEllipse( line.p1().x(), line.p1().y() - 5, 10, 10 );
            break;
        case W::WLineEndComposed:
        case W::WLineEndNone:
            break;
    }
    return pixmap;
}

void WLineEndComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    W::WLineEndStyles n = (W::WLineEndStyles)(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
