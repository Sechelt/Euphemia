#include "LibInfo.h"
#include "DArc.h"

#include "DDiagram.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"

#include <math.h>

#if !defined(M_PI)
    #define M_PI 3.14159265358979323846
#endif

DArc::DArc( ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
    //
    // \NOTE Sources and Handles can not be created in here as they need proxy. \sa DLine
    //

    pointArcDelta.setX( 0 );
    pointArcDelta.setY( 0 );
}

DArc::~DArc()
{
}

/*!
 * \brief Set our position. 
 *  
 * Mostly the same as DLine. We just want to ensure that polygonArc is updated. 
 * 
 * \author pharvey (11/23/19)
 * 
 * \param point 
 */
void DArc::setPos( const QPointF &point ) 
{ 
    DLine::setPos( point ); 
    polygonArc = getArc();
}

void DArc::setStyle( CBD::EOLTypes nEOL, W::WLineEndStyles nStyle )
{
    DLine::setStyle( nEOL, nStyle );
    polygonArc = getArc(); 
}

QPixmap DArc::getIcon()
{
    return QPixmap( ":D/Arc" );
}

QRectF DArc::boundingRect() const
{
    return shape().boundingRect();

    QRectF rect = polygonArc.boundingRect();
    if ( rect.height() < 5 )
        rect.setHeight( 5 );
    if ( rect.width() < 5 )
        rect.setWidth( 5 );

    return rect.normalized();
}

QPainterPath DArc::shape() const
{
    // draw our line into a path (perhaps make the path reusable to speed up code in the future)
    QPainterPath path;
    path.addPolygon( polygonArc ); // QPainterPath adds as unclosed by default (unlike QPainter) - so we are good
    // use a stroker to increase the width a bit to make the line easier to click on
    QPainterPathStroker stroker;
    stroker.setWidth( 20 );

    // return the, now slightly fatter, line
    return stroker.createStroke( path );
}

void DArc::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED(nOption);
    Q_UNUSED(pWidget);

    if ( pFloatingTextCenter ) pFloatingTextCenter->setPlainText( getFloatingTextText() );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    // draw arc
    // polygonArc is kept up-to-date via timely calls to getArc() 
    // so its ready to paint as-is
    pPainter->drawPolyline( polygonArc );

    // draw line ends (if any) last (on top of line)
    if ( getStyle( CBD::EOLBegin ) != W::WLineEndNone )
    {
        QLineF line( polygonArc.first(), polygonArc.at( 1 ) );
        doDrawBegin( pPainter, line, lineIntersectedBegin );
    }

    if ( getStyle( CBD::EOLEnd ) != W::WLineEndNone )
    {
        QLineF line( polygonArc.last(), polygonArc.at( polygonArc.count() - 2 ) );
        doDrawEnd( pPainter, line, lineIntersectedEnd );
    }
}

/*!
    doSave

    Save the standard DObject stuff and our points/handles. 
    
    This is almost the same as DLine except here we want to save an
    "adjusthandle" just before the "texthandle".
*/    
QDomElement DArc::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    //
    QDomElement domElemThis = DLine::doSave( pdomDoc, pdomElemParent );

    // handle for expressing arc
    {
        QDomElement domElem;
        domElem = pdomDoc->createElement( "adjustHandle" );
        domElem.setAttribute( "x", pointArcDelta.x() );
        domElem.setAttribute( "y", pointArcDelta.y() );
        domElemThis.appendChild( domElem );
    }

    // do this in case child has changed
    domElemThis.setAttribute( "NextOID", nNextOID );

    return domElemThis;
}

/*!
    doLoad
    
    Load the standard DObject stuff and our points/handles. 

    This is almost the same as DLine except here we want to load an
    "adjusthandle".
*/    

/*!
 * \brief Load ourself from XML.
 *  
 * Let DLine do most of the work... just ensure get the stuff specific to this.
 *  
 * \author pharvey (11/22/19)
 * 
 * \param pdomElemLine 
 * 
 * \return bool 
 */
bool DArc::doLoad( QDomElement *pdomElemLine )
{
    // 
    DLine::doLoad( pdomElemLine );

    pointArcDelta = QPointF( 0, 0 );

    // look for stuff of interest to us
    QDomElement domElem;
    QDomNode    domNode;
    domNode = pdomElemLine->firstChild();
    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();

        if ( domElem.isNull() )
        {
            domNode = domNode.nextSibling();
            continue;
        }
        // handle for expressing arc
        if ( domElem.tagName() == "adjustHandle" )
        {
            pointArcDelta.setX( domElem.attribute( "x", "0" ).toDouble() );
            pointArcDelta.setY( domElem.attribute( "y", "0" ).toDouble() );
        }
        domNode = domNode.nextSibling();
    }

    // we have our 3 points... calc the arc
    polygonArc = getArc();

    return true;
}

void DArc::slotTruncateBegin( Qt::CheckState n ) 
{ 
    DLine::slotTruncateBegin( n );
    polygonArc = getArc(); 
}

void DArc::slotTruncateEnd( Qt::CheckState n ) 
{ 
    DLine::slotTruncateEnd( n );
    polygonArc = getArc(); 
}

void DArc::doMoveSource( DSource *pSource, const QPointF &pointScenePos )
{
    DLine::doMoveSource( pSource, pointScenePos );
    polygonArc = getArc();
}

/*!
 * \brief Move a handle.
 *  
 * This replaces the version in DLine as our vectorHandles is completely different. 
 *  
 * \author pharvey (11/22/19)
 */
void DArc::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    // move handle to express arc
    if ( pHandle == vectorHandles[0] )
    {
        // update our delta
        QPointF pointReference = getProxy()->mapToScene( polygon.boundingRect().center() );
        pointArcDelta = pointScenePos - pointReference; // may want to do some sanity checks here
        // move the handle
        pHandle->setPos( pointScenePos );
        polygonArc = getArc();
        return;
    }

    if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
    {
        pointDeltaFloatingTextCenter = pointScenePos - getFloatingTextCenterPos();
        pHandle->setPos( pointScenePos );
        pFloatingTextCenter->setPos( pointScenePos );
        return;
    }
}

/*!
 * \brief Create handles.
 *  
 * This replaces the version in DLine as our vectorHandles is completely different. 
 *  
 * \author pharvey (11/22/19)
 */
void DArc::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    // create handle to express arc
    {
        QPointF pointScenePos = getProxy()->mapToScene( polygon.boundingRect().center() ) + pointArcDelta;
        DHandle *pHandle = new DHandle( this, DHandle::FunctionManipulateOther, DHandle::ConnectTypeNone, pointScenePos );
        vectorHandles.append( pHandle );
        getProxy()->scene()->addItem( pHandle );
    }

    // handle for floating text
    { 
        DHandle *pHandle = new DHandle( this, DHandle::FunctionFloatingTextCenter, DHandle::ConnectTypeNone, getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );
        vectorHandles.append( pHandle );
        getProxy()->scene()->addItem( pHandle );
    }
}

/*!
 * \brief Ensure handles are where they are supposed to be (we may have been moved or similar).
 *  
 * This replaces the version in DLine as our vectorHandles is completely different. 
 *  
 * \author pharvey (11/22/19)
 */
void DArc::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    // the pos of the handle for expressing the arc is calculated using a delta 
    // from the center of the boundingRect
    QPointF pointReference = getProxy()->mapToScene( polygon.boundingRect().center() );
    vectorHandles[0]->setPos( pointReference + pointArcDelta );

    // floating text
    vectorHandles[1]->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter ); 
}

/*!
 * \brief Calculates set of points to create an arc.
 *  
 * We use the begin and end points inherited from DLine and the Arc expression 
 * handle we introduced in DArc. 
 *  
 * The begin and end points are adjusted if line is truncated. In such a case 
 * they become the intersection point.
 *  
 * \author pharvey (11/22/19)
 * 
 * \return QPolygonF The arc in item coords - ready to paint.
 */
QPolygonF DArc::getArc()
{
    getProxy()->doPrepareGeometryChange();

    // truncated (item coords)
    QPolygonF polygonItem = getTruncated( lineIntersectedBegin, lineIntersectedEnd );

    // 3 points we need to construct arc
    QPointF pointBegin      = polygonItem[0];
    QPointF pointEnd        = polygonItem[1];

    QPointF pointReference  = getProxy()->mapToScene( polygon.boundingRect().center() );
    QPointF pointArc        = getProxy()->mapFromScene( pointReference + pointArcDelta );

    return getArc2( pointBegin, pointEnd, pointArc );
}

/*!
 * \brief Calculates set of points to create an arc.
 *  
 * Points used are; 
 * 1. Begin point   (or begin truncation point)
 * 2. End point     (or end truncation point)
 * 3. point expressing arc
 *  
 * This is to support \sa getArc(). 
 *  
 * \author pharvey (11/22/19)
 * 
 * \return QPolygonF The arc in item coords - ready to paint.
 */
QPolygonF DArc::getArc2( const QPointF &pointBegin, const QPointF &pointEnd, const QPointF &pointArc )
{
    //
    // \note All calculations in here are in item coords.
    //

    //
    // \note Begin and End points are already truncated as needed.
    //

    // final product
    QPolygonF polygonArc;

    // truncate line ends as needed
    float x0,y0,x1,y1,x2,y2;
    QPolygonF polygonTruncated(3); // = polygon.copy();
    polygonTruncated[0] = pointBegin;
    polygonTruncated[1] = pointArc;
    polygonTruncated[2] = pointEnd;

    if ( !polygonArc.count() ) polygonArc.resize( DARC_SEG_MAX );

    QPointF junk(0,0);
    QPointF border1;
    QPointF border2;
    QPointF border3;
    QPointF border4;


    // lets draw curve
    QPointF v1 = polygonTruncated[0] - polygonTruncated[1];
    QPointF v2 = polygonTruncated[2] - polygonTruncated[1];
    QPointF c = polygonTruncated[1];
    x0 = v1.x();
    y0 = v1.y();
    x1 = y1 = 0;
    x2 = v2.x();
    y2 = v2.y();

    // parametric Lagrange polynomial
    long t;
    long t0 = 0;
    long t2 = 1024;
    long r0, r1, r2;
    int i = 0;
    double d1 = sqrt(((double)x1-x0)*(x1-x0)+((double)y1-y0)*(y1-y0));
    double d2 = sqrt(((double)x1-x2)*(x1-x2)+((double)y1-y2)*(y1-y2));
    long t1 = 1L > (long)((d1*1024.0)/(d1+d2)) ? 1L : (long)((d1*1024.0)/(d1+d2));
    long tinc = 1024L / DARC_N_STEPS;

    if (t1 > 1023L)
        t1 = 1023L;

    i = 0;
    polygonArc.resize(DARC_SEG_MAX);

    for (t = 0; t <= 1024; t += tinc)
    {
        r0 = (((t-t0)*(t-t1))<<10)/((t2-t0)*(t2-t1));
        r1 = (((t-t1)*(t-t2))<<10)/((t0-t1)*(t0-t2));
        r2 = (((t-t0)*(t-t2))<<10)/((t1-t0)*(t1-t2));

        if (i>DARC_SEG_MAX)
            polygonArc.resize(i+1);

        polygonArc[i++] = QPointF( c.x() + ((int)(r0 * x2 + r1 * x0 + r2 * x1) >> 10), 
                                   c.y() + ((int)(r0 * y2 + r1 * y0 + r2 * y1) >> 10) );

    }

    polygonArc.resize(i);

    return polygonArc;
}

/*!
 * \brief Is point on our arc?
 * 
 * \author pharvey (11/22/19)
 * 
 * \param point 
 * 
 * \return bool 
 */
bool DArc::objectUnderPoint( const QPoint &point )
{

    QPointF s,e;
    // variables for ax+by+c=0(line equation)
    float a,b,c,k;
    int x = point.x();
    int y = point.y();

    for (int i=1;i<(int)polygonArc.count();i++)
    {
        s = polygonArc.at(i-1);
        e = polygonArc.at(i);

        // first part
        a = e.y() - s.y();
        b = s.x() - e.x();
        c = -(s.x()*e.y() - s.y()*e.x());
        k = sqrt(a*a + b*b);

        if (k != 0)
        {
            // normalize equation
            a /= k;
            b /= k;
            c /= k;

            // if X lies between X1 & X2 it means that (x-X1)(x-X2) < 0
            // the same for Y
            if (((x-s.x())*(x-e.x()) <= 0) && ((y-s.y())*(y-e.y()) <= 0))
                //
                // if the distance to line less then 10
                if (fabs((a*x + b*y + c)) < 10.)
                    return true;
        }
    }

    return false;
}

/*!
    returns the angle(in degrees)) between Ox and given vector
    NOTE: Make this a static func in DArc?
*/
float DArc::get_angle(QPoint v)
{

    float angle;
    angle = atan2( (double)v.y(), (double)v.x());
    
    if (angle < 0)
        angle += 2*M_PI;

    angle = angle*180/M_PI;
    return angle;
}

