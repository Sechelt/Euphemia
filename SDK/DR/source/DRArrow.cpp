/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "DRArrow.h"

#include <LGraphicsObject.h>
#include <LGraphicsView.h>

#include "DRDraw.h"

DRArrow::DRArrow( DRDraw *pCanvas, const QPointF &pointPos )
    : DRLine( pCanvas, pointPos )
{
    pSettingArrow = (SArrow*)g_SSettings->getSetting( "SArrow" ); Q_ASSERT( pSettingArrow );
    slotRefreshArrow();
}

DRArrow::~DRArrow()
{
}

QPixmap DRArrow::getIcon()
{
    return QPixmap( ":DR/Arrow" );
}

QDomElement DRArrow::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = DRLine::doSave( pdomDoc, pdomElemParent );

    domElemThis.setAttribute( "ArrowSize", nArrowSize );

    return domElemThis;
}

bool DRArrow::doLoad( QDomElement *pdomElemThis )
{
    DRLine::doLoad( pdomElemThis );

    nArrowSize = pdomElemThis->attribute( "ArrowSize", "0" ).toDouble(); 

    return true;
}

void DRArrow::doDraw( QPainter *pPainter )
{
    DRLine::doDraw( pPainter );

    if ( nArrowSize ) doDrawArrow( pPainter, QLineF( pointBegin, pointEnd ) );
}

void DRArrow::doDrawArrow( QPainter *pPainter, const QLineF &line )
{
    // apply context
    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    // draw at end of line
    const qreal Pi = 3.14;
    double angle = ::acos( line.dx() / line.length() );

    if ( line.dy() >= 0 ) angle = (Pi * 2) - angle;

    QPointF arrowP1 = line.p1() + QPointF( sin( angle + Pi / 3 ) * nArrowSize,
                                           cos( angle + Pi / 3 ) * nArrowSize );
    QPointF arrowP2 = line.p1() + QPointF( sin( angle + Pi - Pi / 3 ) * nArrowSize,
                                           cos( angle + Pi - Pi / 3 ) * nArrowSize );

    QPolygonF polygonArrow;
    polygonArrow << line.p1() << arrowP1 << arrowP2;
    pPainter->drawPolygon( polygonArrow );
}

void DRArrow::doConnectSettings()
{
    DRLine::doConnectSettings();

    pSettingArrow->setSelected( nArrowSize );
    connect( pSettingArrow, SIGNAL(signalChanged()), this, SLOT(slotRefreshArrow()) );
}

void DRArrow::doDisconnectSettings()
{
    DRLine::doDisconnectSettings();

    disconnect( pSettingArrow, SIGNAL(signalChanged()), this, SLOT(slotRefreshArrow()) );
}

void DRArrow::slotRefreshArrow()
{
    nArrowSize = pSettingArrow->property( "nSize" ).value<qreal>();
    if ( pGraphics ) pGraphics->update();
}


