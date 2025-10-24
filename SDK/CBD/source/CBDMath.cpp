/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "CBDMath.h"

/*!
 * \brief Get point converted from TopLeft to a given origin where given origin is pos in NE quadrant.
 * 
 * \author pharvey (2/28/23)
 * 
 * \param rect        
 * \param pointOrigin 
 * \param point       
 * 
 * \return QPointF 
 */
QPointF CBDMath::getTopLeftToOrigin( const QRectF &rect, const QPointF &pointOrigin, const QPointF &point )
{
    // convert origin and point to bottom left origin
    QPointF pointOriginNew( pointOrigin.x(), rect.height() - pointOrigin.y() );
    QPointF pointNew( point.x(), rect.height() - point.y() );

    // return pointNew relative to pointOriginNew
    return QPointF( pointNew.x() - pointOriginNew.x(), pointNew.y() - pointOriginNew.y() );
}

/*!
 * \brief Convert polar coordinate to cartesian coordinate for a circle.
 *  
 * The origin is probably going to be the center of a circle. 
 * The cartesian coordinate returned will be relative to this origin. 
 *  
 * \author pharvey (2/28/23)
 * 
 * \param nRadius 
 * \param nAngle    radians (nRadian = nDegree * PI/180.0)
 * 
 * \return QPointF 
 */
QPointF CBDMath::getPolarToCartesian( qreal nRadius, qreal nAngle )
{
    return QPointF( nRadius * qCos( nAngle ), nRadius * qSin( nAngle ) );
}

/*!
 * \brief Convert cartesian coordinate to polar coordinate for a circle. 
 *  
 * The origin is where x and y are 0. This is probably going to be the center 
 * of a circle. 
 *  
 * If the coordinate has a different orgin (ie topLeft of a scene) then the 
 * cartisean coordinate will have to be altered to have correct origin before 
 * calling here. 
 * 
 * \author pharvey (2/28/23)
 * 
 * \param x      
 * \param y      
 * 
 * \return CBDMath::Polar 
 */
CBDMath::Polar CBDMath::getCartesianToPolar( qreal x, qreal y )
{
    Polar p;

    // θ = tan-1 ( y / x )
    p.angle = qAtan2( y, x );
    if ( p.angle < 0 )
    {
        p.angle += (2* M_PI);
    }

    // r = √ ( x2 + y2 )
    p.radius = qSqrt( (x*x) + (y*y) );

    return p;
}

/*!
 * \brief Returns the point on a line (pointBegin,pointEnd) that is closest to the given point.
 * 
 * \author pharvey (2/28/23)
 * 
 * \param point      
 * \param pointBegin      
 * \param pointEnd      
 * 
 * \return QPointF 
 */
QPointF CBDMath::getNearestPointOnLine( const QPointF &point, const QPointF &pointBegin, const QPointF &pointEnd )
{
    QPointF a_to_p;
    QPointF a_to_b;

    a_to_p.setX( point.x() - pointBegin.x() );
    a_to_p.setY( point.y() - pointBegin.y() ); //     # Storing vector pointBegin->point  
    a_to_b.setX( pointEnd.x() - pointBegin.x() );
    a_to_b.setY( pointEnd.y() - pointBegin.y() ); //     # Storing vector pointBegin->pointEnd

    qreal atb2 = a_to_b.x() * a_to_b.x() + a_to_b.y() * a_to_b.y();
    qreal atp_dot_atb = a_to_p.x() * a_to_b.x() + a_to_p.y() * a_to_b.y(); // The dot product of a_to_p and a_to_b
    qreal t = atp_dot_atb / atb2;  //  # The normalized "distance" from a to the closest point

    qreal nX = pointBegin.x() + a_to_b.x() * t;
    qreal nY = pointBegin.y() + a_to_b.y() * t;

    // on an infinite line so lets restrict to between pointBegin and pointEnd on that line
    if (  pointBegin.x() >= pointEnd.x() )
    {
         if ( nX > pointBegin.x() ) nX = pointBegin.x();
         if ( nX < pointEnd.x() ) nX = pointEnd.x();
    }
    if (  pointBegin.y() >= pointEnd.y() )
    {
         if ( nY > pointBegin.y() ) nY = pointBegin.y();
         if ( nY < pointEnd.y() ) nY = pointEnd.y();
    }
     
    return QPointF( nX, nY );
}

qreal CBDMath::getDistance( const QPointF &pointA, const QPointF &pointB )
{
    return sqrt(pow(pointB.x() - pointA.x(), 2) + pow(pointB.y() - pointA.y(), 2) * 1.0);
}

// ie 3.14 radians = 180
qreal CBDMath::getRadiansToDegrees( qreal nRadian )
{
    return ( nRadian * 180 / M_PI );
}

// ie 180 degrees = 3.14 radians
qreal CBDMath::getDegreesToRadians( qreal nDegrees )
{
    return ( nDegrees * M_PI / 180.0 );
}

// https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html
// use QPolygonF::containsPoint instead
bool CBDMath::isPointInPolygon( const QPolygonF polygon, const QPointF &point ) 
{
  int i, j, nPoints = polygon.size();
  bool bIn = false;
  
  for(i = 0, j = nPoints - 1; i < nPoints; j = i++) 
  {
    if ( ( (polygon[i].y() >= point.y() ) != (polygon[j].y() >= point.y()) ) &&
           (point.x() <= (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) / (polygon[j].y() - polygon[i].y()) + polygon[i].x() ) )
        bIn = !bIn;
  }
  
  return bIn;
}


