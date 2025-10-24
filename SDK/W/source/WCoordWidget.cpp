/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WCoordWidget.h"

WCoordWidget::WCoordWidget( QWidget *pParent, const QString &stringLabel )
    : QLabel( pParent )
{
    this->stringLabel = stringLabel;
}

void WCoordWidget::slotCoord( int nCellX, int nCellY )
{
    if ( stringLabel.isEmpty() )
        setText( QString( "%1,%2" ).arg( nCellX ).arg( nCellY ) );
    else
        setText( stringLabel + QString( ": %1,%2" ).arg( nCellX ).arg( nCellY ) );
}

void WCoordWidget::slotCoord( qreal nPixelX, qreal nPixelY )
{
    if ( stringLabel.isEmpty() )
        setText( QString("%1,%2").arg( nPixelX ).arg( nPixelY ) );
    else
        setText( stringLabel + QString(": %1,%2").arg( nPixelX ).arg( nPixelY ) );
}

void WCoordWidget::slotCoord( const QPoint &pointCell )
{
    if ( stringLabel.isEmpty() )
        setText( QString("%1,%2").arg( pointCell.x() ).arg( pointCell.y() ) );
    else
        setText( stringLabel + QString(": %1,%2").arg( pointCell.x() ).arg( pointCell.y() ) );
}

void WCoordWidget::slotCoord( const QPointF &pointPixel )
{
    if ( stringLabel.isEmpty() )
        setText( QString("%1,%2").arg( pointPixel.x() ).arg( pointPixel.y() ) );
    else
        setText( stringLabel + QString(": %1,%2").arg( pointPixel.x() ).arg( pointPixel.y() ) );
}


