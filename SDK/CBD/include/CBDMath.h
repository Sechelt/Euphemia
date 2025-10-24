/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_CBDMath
#define H_CBDMath

#include "CBD.h"

class CBDMath 
{
public:
    struct Polar
    {
        qreal radius;
        qreal angle;        // radians
    };

    static QPointF  getTopLeftToOrigin( const QRectF &rect, const QPointF &pointOrigin, const QPointF &point );
    static QPointF  getPolarToCartesian( qreal nRadius, qreal nAngle );
    static Polar    getCartesianToPolar( qreal x, qreal y );
    static QPointF  getNearestPointOnLine( const QPointF &pointBegin, const QPointF &pointEnd, const QPointF &point );
    static qreal    getDistance( const QPointF &pointA, const QPointF &pointB );
    static qreal    getRadiansToDegrees( qreal nRadian );
    static qreal    getDegreesToRadians( qreal nDegree );
    static bool     isPointInPolygon( const QPolygonF polygon, const QPointF &point );

};

#endif
