#include "LibInfo.h"
#include "DLine.h"

#include <math.h>

#include <WLineEndComboBox.h>
#include <AWPropWidget.h>

#include "DDiagram.h"
#include "DRectangle.h"
#include "DGraphicsProxyItem.h"
#include "DGraphicsSceneProxy.h"
#include "DLineWidget.h"

#include "DPointsWidget.h"
#include "DSourcesWidget.h"

DLine::DLine( ADObject *pObjectParent, const QString &stringName )
    : DObject( pObjectParent, stringName )
{
    DDiagram *pDiagram = nullptr;
    if ( pObjectParent->inherits( "DDiagram" ) ) pDiagram = (DDiagram*)pObjectParent;
    else pDiagram = (DDiagram *)getParent( "DDiagram" );
    Q_ASSERT( pDiagram );

    // \NOTE Sources can NOT be created in here as they need proxy. 
    //       Sources are created once... in setProxy()... which should be called right after adding to scene.
    //       Source handles are not stored with other handles - they are stored in the source.
    //       Sources create their handle when setSelected( true ).
    //       Sources delete their handle when setSelected( false ).
    //       Sources are deleted in destructor.
    //      
    //      \sa DSource
    //      \sa setProxy()
    //      \sa setSelected()

    pen.setColor( Qt::black );
    brush.setColor( Qt::black );
    brush.setStyle( Qt::SolidPattern );
    bAutoRouted     = false;
    nPadding        = 20;
    bTruncateBegin  = false;
    bTruncateEnd    = false;
    nStyleBegin     = W::WLineEndNone;
    nStyleEnd       = W::WLineEndNone;

    // start with a simple, 2 point, line
    polygon.append( QPoint( 0, -pDiagram->getCell().height() / 2 ) );
    polygon.append( QPoint( 0, pDiagram->getCell().height() / 2 ) );

    // floating text
    pointDeltaFloatingTextCenter = QPoint( 15, 15 );
    pFloatingTextCenter = nullptr;
}

DLine::~DLine()
{
    doSourcesDelete();
}

void DLine::setName( const QString &s )
{
    if ( pFloatingTextCenter && getTextDisplay() == TextDisplayName )
        pFloatingTextCenter->setPlainText( s );

    // do this sort of thing last as typically triggers setModified
    DObject::setName( s );
}

void DLine::setCode( const QString &s )
{
    if ( pFloatingTextCenter && getTextDisplay() == TextDisplayCode )
        pFloatingTextCenter->setPlainText( s );

    // do this sort of thing last as typically triggers setModified
    DObject::setCode( s );
}

void DLine::setComment( const QString &s )
{
    if ( pFloatingTextCenter && getTextDisplay() == TextDisplayComment )
        pFloatingTextCenter->setPlainText( s );

    // do this sort of thing last as typically triggers setModified
    DObject::setComment( s );
}

/*!
 * \brief Set pos.
 *  
 * Disconnect any connected sources. 
 *  
 * Change pos and sync handle/source positions (as they are not a child of this). 
 *  
 * \author pharvey (11/20/19)
 * 
 * \param point 
 */
void DLine::setPos( const QPointF &point )
{
    // unlike rectangle (via DSink) - we do NOT bring connections along for the ride
    DSource *pSource;

    pSource = vectorSources.first();
    if ( pSource->isGlued() && !pSource->getRectangle()->isSelected() )
        doDisconnect( CBD::EOLBegin );

    pSource = vectorSources.last();
    if ( pSource->isGlued() && !pSource->getRectangle()->isSelected() )
        doDisconnect( CBD::EOLEnd );

    // set our pos
    DObject::setPos( point );

    // sync
    if ( isSelected() ) doHandlesSync();
    doSourcesSync();
}

/*!
 * \brief Set pos without disconnecting handles.
 *  
 * Change pos and sync handles/sources. 
 *  
 * This is useful when moving all objects such as when creating/removing pages. 
 *  
 * \author pharvey (11/20/19)
 * 
 * \param point 
 */
void DLine::setPosRaw( const QPointF &point )
{   
    DObject::setPosRaw( point );

    if ( isSelected() ) doHandlesSync();
    doSourcesSync();
}

/*!
 * \brief Move the source. 
 *  
 * This is for DSink. The sink is moving (presumably rectangle moving) so it calls here 
 * to ensure that a connected source comes along. 
 *  
 * At present - it is NOT used for any other purpose. 
 *  
 * \author pharvey (11/22/19)
 * 
 * \param pSource The source to be moved (not moved here).
 * \param pointScene The desired destination.
 */
void DLine::setPos( DSource *pSource, const QPointF &pointScene )
{
    // Call doRoute to route to destination and move pSource there 
    doRoute( pSource, pointScene );
    return;
}

void DLine::setProxy( DGraphicsProxyItem *p )
{
    DObject::setProxy( p );
    doSourcesCreate();
}

/*!
 * \brief Set selected true/false. 
 *  
 * We do the default and then handle the fact that we have source handles. 
 * 
 * \author pharvey (11/18/20)
 * 
 * \param b 
 */
void DLine::setSelected( bool b )
{
    if ( b == isSelected() ) return;
    DObject::setSelected( b );

    if ( b )
    {
        if ( getSelectType() == SelectResize ) doSourcesVisible( true );
    }
    else
    {
        if ( vectorSources.first()->isVisible() ) doSourcesVisible( false );
    }
}

void DLine::setVisible( bool b ) 
{ 
    getProxy()->setVisible( b ); 
    if ( pFloatingTextCenter ) pFloatingTextCenter->setVisible( b ); 
}

void DLine::setBegin( const QPointF &pointScene )
{
    // \todo disconnect if connected
    doMoveSource( getSource( CBD::EOLBegin ), pointScene );
}

void DLine::setEnd( const QPointF &pointScene )
{
    // \todo disconnect if connected
    doMoveSource( getSource( CBD::EOLEnd ), pointScene );
}

void DLine::setStyle( CBD::EOLTypes nEOL, W::WLineEndStyles nStyle )
{
    if ( nEOL == CBD::EOLBegin )
    { 
        if ( nStyleBegin == nStyle ) return; 
        nStyleBegin = nStyle; 
        getProxy()->update();
        setModified();
    }
    else if ( nEOL == CBD::EOLEnd )
    { 
        if ( nStyleEnd == nStyle ) return; 
        nStyleEnd = nStyle; 
        getProxy()->update();
        setModified();
    }
}

QPixmap DLine::getIcon()
{
    return QPixmap( ":D/Line" );
}

/*!
 * \brief Get the line segment that lies within the connected rectangle.
 *  
 * Different strategies that can be used here... 
 *  
 * 1. based off of a line from first point to last point 
 * 2. based off of a line from first point to next point 
 * 3. based off of a line from first point to closest point on closest rectangle edge 
 *  
 * These can be combined... 
 *  
 * a. IF center THEN use (1) ELSE (3) 
 * b. IF static sink and not center THEN use (3) ELSE use (1) 
 *  
 * \note Only (1) is implemented at this time. This makes sense when there is no importance to where/which/type sink is used.
 *  
 * \author pharvey (5/16/20)
 * 
 * \param n 
 * 
 * \return DIntersectedLine 
 */
DIntersectedLine DLine::getIntersectedLine( CBD::EOLTypes nEOL )
{
    // get DSource and attached DRectangle
    DSource *pSource = getSource( nEOL );
    if ( !pSource ) return DIntersectedLine();
    DRectangle *pRectangle = pSource->getRectangle();
    if ( !pRectangle ) return DIntersectedLine();
    DSink *pSink = pSource->getSink();

    // get DSource and DRectangle scene coordinates    
    QPointF pointScene  = pSink->getPos();
    QRectF  rectScene   = pRectangle->getProxy()->mapRectToScene( pRectangle->boundingRect() ); 

    DSource *pSourceOtherEnd = getSource( nEOL == CBD::EOLBegin ? CBD::EOLEnd : CBD::EOLBegin );
    QLineF  lineSegment( pointScene, pSourceOtherEnd->getPos() );
    QPointF pointIntersected;
    
    if (  QLineF( rectScene.topLeft(), rectScene.topRight() ).intersects( lineSegment, &pointIntersected ) == QLineF::BoundedIntersection )
        return DIntersectedLine( nEOL, lineSegment.p1(), pointIntersected, CBD::DirectionN );
    else if (  QLineF( rectScene.bottomLeft(), rectScene.bottomRight() ).intersects( lineSegment, &pointIntersected ) == QLineF::BoundedIntersection )
        return DIntersectedLine( nEOL, lineSegment.p1(), pointIntersected, CBD::DirectionS );
    else if (  QLineF( rectScene.topRight(), rectScene.bottomRight() ).intersects( lineSegment, &pointIntersected ) == QLineF::BoundedIntersection )
        return DIntersectedLine( nEOL, lineSegment.p1(), pointIntersected, CBD::DirectionE );
    else if (  QLineF( rectScene.topLeft(), rectScene.bottomLeft() ).intersects( lineSegment, &pointIntersected ) == QLineF::BoundedIntersection )
        return DIntersectedLine( nEOL, lineSegment.p1(), pointIntersected, CBD::DirectionW );

    // most probable cause of getting here is that the other EOL is in same rectangle
qDebug() << "[PAH]" << __FILE__ << __FUNCTION__ << __LINE__ << "both ends in same rectangle?";

   return DIntersectedLine();
}

DSource *DLine::getSource( CBD::EOLTypes n ) 
{ 
    return (n == CBD::EOLBegin ? vectorSources.first() : vectorSources.last()); 
}

DSource *DLine::getSource( const QString &stringName )
{
    DSource *pSource;
    foreach( pSource, vectorSources )
    {
       if ( pSource->objectName() == stringName ) return pSource;
    }
    return nullptr;
}

DSource *DLine::getSource( DHandle *p )
{
    DSource *pSource;
    foreach( pSource, vectorSources )
    {
        if ( p == pSource->getHandle() ) return pSource;
    }

    return nullptr;
}

DRectangle *DLine::getRectangle( CBD::EOLTypes n )
{
    DSource *pSource = getSource( n );
    return pSource->getRectangle();
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We introduce a list of 'Sources' (\sa DSource) that we maintain. 
 * Example; at begin and end of line. 
 *  
 * We bypass DObject and use AWObject because we implement pen/brush/font widgets differently. 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DLine::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->removeByClass( "AWFactoryWidget" );
    p->insertTab( 1, new DPointsWidget( this, p ), tr("Points") );
    p->insertTab( 1, new DSourcesWidget( this, p ), tr("Sources") );
    p->insertTab( 1, new DLineWidget( this, p ), tr("Line") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DLine::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DObject::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Text"), new DLineTextWidget( this, pPropWidget ) );
    pPropWidget->addWidget( tr("Connector"), new DLineConnectorWidget( this, pPropWidget ) );

    return pPropWidget;

}

void DLine::doRoute()
{
    getProxy()->doPrepareGeometryChange();
    DSource *pSource = getSource( CBD::EOLEnd );
    doRoute( pSource, pSource->getPos() );
}

void DLine::doGroupSync()
{
    if ( isSelected() ) doHandlesSync();
    doSourcesSync();

    return;
}


/*!
 * \brief We are in a group and the group has changed size - so scale ourself.
 *  
 * We are a just a simple line with exactly 2 points. A connected line-end is adjusted 
 * when the rectangle adjusts itself so all we need to be concerned about 
 * is a line-end which is not connected. 
 * 
 * \sa DGroup::doScale 
 *  
 * \author pharvey (10/31/20)
 * 
 * \param sizeFactor 
 */
void DLine::doGroupScale( const QSizeF &sizeFactor )
{
    getProxy()->doPrepareGeometryChange();

    // adjust pos
    {
        QPointF pointNew( pointPos.x() * sizeFactor.width(), pointPos.y() * sizeFactor.height() );
        DObject::setPosRaw( pointNew );
    }

    // adjust points
    for ( int n = 0; n < polygon.count(); n++ )
    {
        if ( n == 0 && getSource( CBD::EOLBegin )->isGlued() ) continue;
        if ( n == polygon.count() - 1 && getSource( CBD::EOLEnd )->isGlued() ) continue;
        QPointF point = polygon[n];
        polygon[n] = QPointF( point.x() * sizeFactor.width(), point.y() * sizeFactor.height() );
    }

    if ( isSelected() ) doHandlesSync();
    doSourcesSync();

    return;
}

/*!
 * \brief We have just been added to a group - make any needed adjustments. 
 *  
 * We are a just a simple line with exactly 2 points. A connected line-end is adjusted 
 * when the rectangle adjusts itself so all we need to be concerned about 
 * is a line-end which is not connected. 
 * 
 * \sa DGroup::doGroup 
 *  
 * \author pharvey (11/2/20)
 * 
 * \param pointDelta 
 */
void DLine::doGroupIn( const QPointF &pointDelta )
{
    // set pos without disconnecting any connected source/sinks
    if ( getRectangle( CBD::EOLBegin ) || getRectangle( CBD::EOLEnd ) )
    {
        // we have connection(s) 
        // any connected rectangles included in the group have already been moved and this has already adjusted point so we ignore 
        // any connected rectangles NOT included in the group are not adjusted and neither is the point so we ignore
        for ( int n = 0; n < polygon.count(); n++ )
        {
            if ( n == 0 && getRectangle( CBD::EOLBegin ) ) continue;
            if ( n == polygon.count() - 1 && getRectangle( CBD::EOLEnd ) ) continue;
            polygon[n] -= pointDelta;
        }
        // ensure pos is center of bounding rect - this useful for debugging if nothing else
        // also does     getProxy()->doPrepareGeometryChange();
        doNormalize();
    }
    else
    {
        // no connections so just use standard call
        setPos( pointPos - pointDelta );
    }
}

/*!
 * \brief We have just been removed from a group - make any needed adjustments. 
 *  
 * We are a just a simple line with exactly 2 points. A connected line-end is adjusted 
 * when the rectangle adjusts itself so all we need to be concerned about 
 * is a line-end which is not connected. 
 * 
 * \sa DGroup::doUngroup 
 *  
 * \author pharvey (11/2/20)
 * 
 * \param pointDelta 
 */
void DLine::doGroupOut( const QPointF &pointDelta )
{
    // set pos without disconnecting any connected source/sinks
    if ( getRectangle( CBD::EOLBegin ) || getRectangle( CBD::EOLEnd ) )
    {
        // we have connection(s) 
        // any connected rectangles included in the group have already been moved and this has already adjusted point so we ignore 
        // any connected rectangles NOT included in the group are not adjusted and neither is the point so we ignore
        for ( int n = 0; n < polygon.count(); n++ )
        {
            if ( n == 0 && getRectangle( CBD::EOLBegin ) ) continue;
            if ( n == polygon.count() - 1 && getRectangle( CBD::EOLEnd ) ) continue;
            polygon[n] += pointDelta;
        }
        // ensure pos is center of bounding rect - this useful for debugging if nothing else
        // also does     getProxy()->doPrepareGeometryChange();
        doNormalize();
    }
    else
    {
        // no connections so just use standard call
        setPos( pointPos + pointDelta );
    }
}

/*!
 * \brief Connect source to sink.
 *  
 * This; 
 *  
 * 1. 'glues' the Source to the Sink 
 * 2. 'connects' (creates a reference) the line to the rectangle
 *  
 * \author pharvey (11/9/20)
 * 
 * \param pSource 
 * \param pSink 
 * 
 * \return bool 
 */
bool DLine::doConnect( DSource *pSource, DSink *pSink )
{
    // move source to same pos as sink
    doMoveSource( pSource, pSink->getPos() );

    // glue source handle 
    if ( !pSink->getRectangle()->doGlue( pSource ) ) return false;

    // make the reference (doUsesAdd/doUsedByAdd)
    DObject::doConnect( pSink->getRectangle(), pSource->objectName() );

    return true;
}

/*!
 * \brief Make a reference to another object.
 *  
 * We do some validation here. 
 * Glue one of our sources to a sink (new or existing) in pObject. 
 * Then call DObject to make the reference. 
 *  
 * \author pharvey (2/4/20)
 * 
 * \param pObject 
 * \param stringName 
 * 
 * \return bool 
 */
bool DLine::doConnect( ADObject *pObject, const QString &stringName )
{
    // sanity check
    if ( !pObject ) return false; 
    // in this case we can only make reference to a DRectangle based object
    if ( !pObject->inherits( "DRectangle" ) ) return false; 
    DRectangle *pRectangle = (DRectangle*)pObject;
    // in this case stringName indicates Begin or End of line - use it to get the source handle
    DSource *pSource = ( stringName == "Begin" ? vectorSources.first() : ( stringName == "End" ? vectorSources.last() : nullptr ) );
    if ( !pSource ) return false;
    // glue source handle 
    if ( !pRectangle->doGlue( pSource ) ) return false;

    // make the reference (doUsesAdd/doUsedByAdd)
    DObject::doConnect( pRectangle, stringName );

    return true;
}

bool DLine::doDisconnect( ADObject *pObject, const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass )
{
    Q_UNUSED(stringClassName);
    Q_UNUSED(stringInheritsClass);

    // sanity check
    if ( !pObject ) return false; 
    // in this case we can only make reference to a DRectangle based object
    if ( !pObject->inherits( "DRectangle" ) ) return false; 
    DRectangle *pRectangle = (DRectangle*)pObject;
    // in this case stringName indicates Begin or End of line - use it to get the source handle
    DSource *pSource = ( stringName == "Begin" ? vectorSources.first() : ( stringName == "End" ? vectorSources.last() : nullptr ) );
    if ( !pSource ) return false;
    // unglue source handle 
    if ( !pRectangle->doUnglue( pSource, pSource->getSink() ) ) return false;

    // remove the reference
    DObject::doDisconnect( pObject, stringName );

    return true;
}

/*!
 * \brief Disconnect line-end from rectangle. 
 *  
 * A supporting method which tries to harmonize glueing and connecting. 
 *  
 * \author pharvey (12/19/19)
 * 
 * \param n 
 * \param bTryModel 
 * \param bDoAnyway 
 */
void DLine::doDisconnect( CBD::EOLTypes n )
{
    Q_ASSERT( n == CBD::EOLBegin || n == CBD::EOLEnd );

    DSource *  pSource     = ( n == CBD::EOLBegin ? vectorSources.first() : vectorSources.last() );
    QString    stringName  = ( n == CBD::EOLBegin ? "Begin" : "End" );

    Q_ASSERT( pSource );
    Q_ASSERT( pSource->isGlued() );
    Q_ASSERT( isConnected( nullptr, stringName ) );

    // because model has notified us it disconnected or we are doing cleanup or some other reason
    pSource->getRectangle()->doUnglue( pSource, pSource->getSink() );
    DObject::doDisconnect( pSource->getRectangle(), stringName );
}

QRectF DLine::boundingRect() const
{
    return shape().boundingRect();

    QRectF rect = polygon.boundingRect();
    // shape has padded line width but ensure that we have something
    if ( rect.height() < nPadding ) rect.setHeight( nPadding );
    if ( rect.width() < nPadding ) rect.setWidth( nPadding );

    return rect.normalized();
}

QPainterPath DLine::shape() const
{
    // draw our line into a path (perhaps make the path reusable to speed up code in the future)
    QPainterPath path;
    path.addPolygon( polygon ); // QPainterPath adds as unclosed by default (unlike QPainter) - so we are good
    // use a stroker to increase the width a bit to make the line easier to click on
    QPainterPathStroker stroker;
    stroker.setWidth( nPadding );

    // return the, now slightly fatter, line
    return stroker.createStroke( path );
}

void DLine::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    QLineF lineIntersectedBegin;
    QLineF lineIntersectedEnd;
    QPolygonF polygonItem = getTruncated( lineIntersectedBegin, lineIntersectedEnd );

    if ( polygonItem.first() == polygonItem.last() && polygonItem.count() < 3 )
    {
        // most importantly - trying to draw line ends will crap out app - at least on win
        qWarning( "[PAH][%s][%s][%d] Can not draw line as it has only two points and they are the same.\n", __FILE__, __FUNCTION__, __LINE__ );
        return;
    }

    // LINE
    pPainter->drawPolyline( polygonItem );

    // draw line ends (if any) last (on top of line)
    if ( getStyle( CBD::EOLBegin ) != W::WLineEndNone )
    {
        QLineF lineSegment( polygonItem.first(), polygonItem.at( 1 ) );
        doDrawBegin( pPainter, lineSegment, lineIntersectedBegin );
    }

    if ( getStyle( CBD::EOLEnd ) != W::WLineEndNone )
    {
        QLineF lineSegment( polygonItem.last(), polygonItem.at( polygonItem.count() - 2 ) );
        doDrawEnd( pPainter, lineSegment, lineIntersectedEnd );
    }

    if ( pFloatingTextCenter ) 
    {
        pFloatingTextCenter->setDefaultTextColor( font.color );
        pFloatingTextCenter->setFont( font.font );
    }

    return;

    // for debugging...
    
    // draw center point in boundingRect
    {
        QRectF r( 0, 0, 16, 16 );
        r.moveCenter( boundingRect().center() );
        pPainter->setPen( QPen( Qt::red ) );
        pPainter->setBrush( Qt::NoBrush );
        pPainter->drawEllipse( r );
    }

    // draw pos
    {
        QRectF r( 0, 0, 8, 8 );
        r.moveCenter( getProxy()->mapFromParent( getPos() ) );
        pPainter->setPen( QPen( Qt::darkYellow ) );
        pPainter->setBrush( Qt::NoBrush );
        pPainter->drawEllipse( r );
    }
}

void DLine::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    // do nothing as we do not support this feature
}

QDomElement DLine::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff, children, and any references...
    QDomElement domElemThis = DObject::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    domElemThis.setAttribute( "x", getProxy()->pos().x() );
    domElemThis.setAttribute( "y", getProxy()->pos().y() );
    domElemThis.setAttribute( "StyleBegin", getStyle( CBD::EOLBegin ) );
    domElemThis.setAttribute( "StyleEnd", getStyle( CBD::EOLEnd ) );
    domElemThis.setAttribute( "TruncateBegin", isTruncate( CBD::EOLBegin ) );
    domElemThis.setAttribute( "TruncateEnd", isTruncate( CBD::EOLEnd ) );
    domElemThis.setAttribute( "FloatingTextCenter.x", pointDeltaFloatingTextCenter.x() );
    domElemThis.setAttribute( "FloatingTextCenter.y", pointDeltaFloatingTextCenter.y() );
    
    // save additional nodes...    
        
    // points
    {
        QDomElement domElemPoint;
        for ( int n = 0; n < polygon.count(); n++ )
        {
            domElemPoint = pdomDoc->createElement( "Point" );
            domElemPoint.setAttribute( "x", polygon[n].x() );
            domElemPoint.setAttribute( "y", polygon[n].y() );
            domElemThis.appendChild( domElemPoint );
        }
    }

    // sources
    // - includes any info about being 'glued' to a rectangle
    // - the 'glue' compliments/supports a reference but is presentation so is independent of a reference
    {
        DSource *pSource;
        for ( int nIndex = 0; nIndex < vectorSources.count(); nIndex++ )
        {
            pSource = vectorSources[nIndex];
            QDomElement domElemSource = pdomDoc->createElement( "Source" );
            domElemSource.setAttribute( "Index", nIndex );
            domElemSource.setAttribute( "Type", pSource->getType() );
            domElemSource.setAttribute( "x", pSource->getPos().x() );
            domElemSource.setAttribute( "y", pSource->getPos().y() );
            if ( pSource->isGlued() )
            {
                if ( !g_Transaction->canCopy() || pSource->getRectangle()->isSelected() )
                { 
                    domElemSource.setAttribute( "Rectangle", pSource->getRectangle()->getPathString( g_Transaction->pRoot ) );
                    domElemSource.setAttribute( "Sink", pSource->getSink()->getIndex() );
                }
            }
            domElemThis.appendChild( domElemSource );
        }
    }

    return domElemThis;
}

bool DLine::doLoad( QDomElement *pdomElemObject )
{
    if ( !DObject::doLoad( pdomElemObject ) ) return false;

    getProxy()->doPrepareGeometryChange();

    QString stringAttribute;

    //
    // OUR ROOT ELEMENT
    // - position 
    
    nStyleBegin         = W::WLineEndStyles(pdomElemObject->attribute( "StyleBegin", "0" ).toInt()); 
    nStyleEnd           = W::WLineEndStyles(pdomElemObject->attribute( "StyleEnd", "0" ).toInt()); 
    bTruncateBegin      = bool(pdomElemObject->attribute( "TruncateBegin", "0" ).toInt()); 
    bTruncateEnd        = bool(pdomElemObject->attribute( "TruncateEnd", "0" ).toInt()); 
    pointDeltaFloatingTextCenter.setX( pdomElemObject->attribute( "FloatingTextCenter.x", "15" ).toDouble() );
    pointDeltaFloatingTextCenter.setY( pdomElemObject->attribute( "FloatingTextCenter.y", "15" ).toDouble() );

    // load our position
    {
        QPointF point;
        stringAttribute = pdomElemObject->attribute( "x" );
        if ( !stringAttribute.isNull() ) point.setX( stringAttribute.toDouble() );

        stringAttribute = pdomElemObject->attribute( "y" );
        if ( !stringAttribute.isNull() ) point.setY( stringAttribute.toDouble() );

        // bypass our setPos
        getProxy()->setPos( point );
        pointPos = point;
    }
 
    //
    // OUR CHILD NODES
    // - points
    // - sources

    // clear all points
    polygon.resize( 0 );
    // clear sources
    qDeleteAll( vectorSources );
    vectorSources.clear();

    //
    QPointF     point;
    QDomElement domElem;
    QDomNode    domNode;

    domNode = pdomElemObject->firstChild();

    while ( !domNode.isNull() )
    {
        domElem = domNode.toElement();
        if ( !domElem.isNull() )
        {
            // create point
            if ( domElem.tagName() == "Point" )
            {
                point.setX( -1 );
                point.setY( -1 );

                stringAttribute = domElem.attribute( QString( "x" ) );
                if ( !stringAttribute.isNull() )
                    point.setX( stringAttribute.toDouble() );

                stringAttribute = domElem.attribute( QString( "y" ) );
                if ( !stringAttribute.isNull() )
                    point.setY( stringAttribute.toDouble() );

                polygon.append( point );
            } 
            else if ( domElem.tagName() == "Source" )
            {
                int nIndex                  = domElem.attribute( "Index" ).toInt();
                CBD::EOLTypes nType  = (CBD::EOLTypes)domElem.attribute( "Type" ).toInt();
                QPointF pointPos            = QPointF( domElem.attribute( "x" ).toDouble(), domElem.attribute( "y" ).toDouble() );

                DSource *pSource = new DSource( this, nType ); 
                pSource->setPos( pointPos );
                if ( nIndex >= vectorSources.count() ) vectorSources.resize( nIndex + 1 );
                vectorSources[nIndex] = pSource;

                // is it to be connected?
                QString stringRectangle = domElem.attribute( "Rectangle" ); // path to rectangle
                QString stringSink      = domElem.attribute( "Sink" );      // index to sink
                if ( !stringRectangle.isEmpty() && !stringSink.isEmpty() )
                { 
                    // store connect info for doPostLoad()
                    pSource->setProperty( "Rectangle", stringRectangle );
                    pSource->setProperty( "Sink", stringSink.toInt() );
                }
            }
        } // if element
        domNode = domNode.nextSibling();
    } // while

    // ensure we have at least 2 points
    if ( polygon.count() == 0 )
        polygon.append( QPointF( 10, 10 ) );
    if ( polygon.count() == 1 )
        polygon.append( QPointF( -10, -10 ) );

    // no need to sync
    if ( pFloatingTextCenter ) pFloatingTextCenter->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );

    return true;
}

/*!
 * \brief Do stuff we could not do until all objects loaded/created. 
 *  
 * We connect sources to sinks as needed. It is easier to do this in 
 * a DLine than in a DRectangle as a source is only connected to 0-1 
 * sinks (a sink can be connected to 0-n sources).  
 *  
 * The rectangle OID and Index to its sink were stored in this line's
 * properties during doLoad(). 
 *  
 * In the case of a load for a paste operation the top-level OID's have 
 * to be changed to be unique. The original OID and new OID are stored 
 * during doLoad(). This translation is silently handled within 
 * getObject(int nOID).  
 * 
 * \author pharvey (11/17/19)
 */
void DLine::doPostLoad()
{
    // resolve our references
    // do children (if any)
    DObject::doPostLoad();

    // do self

    // Glue line ends to rectangles if needed. This is done independently of references.
    // Our doLoad put this info in our sources. So lets process our sources.

    DSource *pSource;
    foreach( pSource, vectorSources )
    {
        // get rectangle OID and its sink Index from our properties
        if ( pSource->property( "Rectangle" ).isNull() ) continue;
        QString stringRectanglePath = pSource->property( "Rectangle" ).toString();  // path
        int nSinkIndex = pSource->property( "Sink" ).toInt();                       // Index
        Q_ASSERT( nSinkIndex >=0 );
        // find rectangle
        ADObject *pObject;
        if ( g_Transaction->canPaste() )
        {
            QString s = g_Transaction->mapXRef[stringRectanglePath];
            pObject = g_Transaction->pRoot->getObject( s.split( ',' ) );
        }
        else
        {
            pObject = g_Transaction->pRoot->getObject( stringRectanglePath.split( ',' ) );
        }

        Q_ASSERT( pObject );
        Q_ASSERT( pObject->inherits( "DRectangle" ) );

        DRectangle *pRectangle = (DRectangle*)pObject;

        // find sink with Index
        Q_ASSERT( nSinkIndex < pRectangle->getSinks().count() );
        DSink *pSink = pRectangle->getSinks().at( nSinkIndex );
        Q_ASSERT( pSink );

        // connect 
        pRectangle->doGlue( pSource, pSink );
        // cleanup (setting to null will remove temp properties)
        pSource->setProperty( "Rectangle", QVariant() );
        pSource->setProperty( "Sink", QVariant() );
    }

    // we now have a reference to model (DLine) so set floating text 
    if ( pFloatingTextCenter ) pFloatingTextCenter->setPlainText( getFloatingTextText() );
}

/*!
 * \brief Catch case where DLine (our model) is connected/disconnect to/from a DRectangle.
 *  
 * Connects signals so that we are notified when the DLine (our model) connects/disconnects to/from a DRectangle. 
 * In such a case; we want to consider doing the same to the corresponding DRectangle. 
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void DLine::doUsesAdd( ADObject *pObject, const QString &s )
{
    DObject::doUsesAdd( pObject, s );

    // We should have exactly one of these
    if ( pObject->inherits( "DLine" ) )
    {
        connect( pObject, SIGNAL(signalConnected(ADObject*,ADObject*,const QString &)), this, SLOT(slotConnected(ADObject*,ADObject*,const QString &)) );
        connect( pObject, SIGNAL(signalDisconnected(ADObject*,ADObject*,const QString &)), this, SLOT(slotDisconnected(ADObject*,ADObject*,const QString &)) );
    }
}

/*!
 * \brief Catch case where we are being disconnected from DLine.
 *  
 * Connects signals so that we are notified when the DLine connects/disconnects. 
 *  
 * \author pharvey (12/21/19)
 * 
 * \param pmodel 
 * \param s 
 */
void DLine::doUsesRemove( ADObject *pObject, const QString &s  )
{
    DObject::doUsesRemove( pObject, s );

    //
    if ( pObject->inherits( "DLine" ) )
    {
        disconnect( pObject, SIGNAL(signalConnected(ADObject*,ADObject*,const QString &)), this, SLOT(slotConnected(ADObject*,ADObject*,const QString &)) );
        disconnect( pObject, SIGNAL(signalDisconnected(ADObject*,ADObject*,const QString &)), this, SLOT(slotDisconnected(ADObject*,ADObject*,const QString &)) );
    }
}

void DLine::mousePressHandle( DHandle *pHandle, const QPointF &pointScene )
{
    Q_UNUSED(pHandle);
    Q_UNUSED(pointScene);
}

void DLine::mouseMoveHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // is the handle a source?
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        // find the source
        DSource *pSource;
        foreach( pSource, vectorSources )
        {
            if ( pHandle == pSource->getHandle() )
            {
                doMoveSource( pSource, pointScene );
                return;
            }
        }
        return; 
    }

    // must be for moving (ie text handle)
    doHandleMove( pHandle, pointScene );
}

void DLine::mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene )
{
    // is the handle a source?
    if ( pHandle->getConnectType() == DHandle::ConnectTypeSource )
    {
        // find the source
        DSource *pSource;
        foreach( pSource, vectorSources )
        {
            if ( pHandle == pSource->getHandle() )
            {
                doMoveSource( pSource, pointScene );
                return;
            }
        }
        return; 
    }
}

void DLine::slotStyleBegin( W::WLineEndStyles n ) 
{
    setStyle( CBD::EOLBegin, n ); 
}

void DLine::slotStyleEnd( W::WLineEndStyles n )  
{ 
    setStyle( CBD::EOLEnd, n ); 
}

void DLine::slotTruncateBegin( Qt::CheckState nState ) 
{ 
    bTruncateBegin = ( nState==0 ? false : true ); 
    getProxy()->update(); 
    setModified();
}

void DLine::slotTruncateEnd( Qt::CheckState nState ) 
{ 
    bTruncateEnd = ( nState==0 ? false : true ); 
    getProxy()->update(); 
    setModified();
}

void DLine::slotTextDisplay( int n )
{
    nTextDisplay = (TextDisplayTypes)n;
    if ( pFloatingTextCenter ) pFloatingTextCenter->setPlainText( getFloatingTextText() );
    setModified();
}

void DLine::doMoveSource( DSource *pSource, const QPointF &pointScenePos )
{
    getProxy()->doPrepareGeometryChange();
    // update appropriate point in our line
    if ( pSource->isBegin() )
        polygon[0] = getProxy()->mapFromScene( pointScenePos ); 
    else if ( pSource->isEnd() )
        polygon[polygon.count()-1] = getProxy()->mapFromScene( pointScenePos ); 
    doNormalize();
    if ( isSelected() ) doHandlesSync();
    doSourcesSync();
}

/*!
 * \brief Calc and return scene pos for center text handle/item.
 *  
 * In this case its relative to center point or center of the middle line segment.
 *  
 * \author pharvey (11/27/19)
 * 
 * \return QPointF 
 */
QPointF DLine::getFloatingTextCenterPos()
{
    Q_ASSERT( polygon.count() > 1 );

    // odd number of points?
    double nWhole;
    double nFrac = modf( double(polygon.count()) / 2, &nWhole );
    if ( nFrac > 0 )
    {
        // use center point
        return getProxy()->mapToScene( polygon[polygon.count() / 2] );
    }

    // use center of middle line segment

    // determine the first point of the middle line segment
    int n = polygon.count() / 2 - 1;
    // make a rect out of line segment
    QRectF r;
    r.setTopLeft( polygon[n] );
    r.setBottomRight( polygon[n + 1] );
    // return relative to center of rect
    return getProxy()->mapToScene( r.normalized().center() ); 
}

QString DLine::getFloatingTextText()
{
    switch ( getTextDisplay() )
    {
        case TextDisplayName:
            return getName();
        case TextDisplayCode:
            return getCode();
        case TextDisplayComment:
            return getComment();
        case TextDisplayNone:
            break;
        default:
            qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " Invalid value from getTextDisplay().";
    }
    return QString();
}

void DLine::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    switch ( getSelectType() )
    {
        case SelectResize:
            // Center floating text handle?
            if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
            {
                // adjust delta and reposition
                pointDeltaFloatingTextCenter = pointScenePos - getFloatingTextCenterPos();
                pHandle->setPos( pointScenePos );
                pFloatingTextCenter->setPos( pointScenePos );
                return;
            }

            // other handles
            {
                int nPoint = pHandle->getIndex();
                polygon[nPoint] = getProxy()->mapFromScene( pointScenePos );
                getProxy()->doPrepareGeometryChange();
                if ( isSelected() ) doHandlesSync();
                return;
            }
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
        case SelectOnly:
            break;
    }
}

void DLine::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            // Center handle for floating text
            { 
                DHandle *pHandle = new DHandle( this, DHandle::FunctionFloatingTextCenter, DHandle::ConnectTypeNone, getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );
                vectorHandles.append( pHandle );
                getProxy()->scene()->addItem( pHandle );
            }
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doInitSelect();
            break;
    }
}

void DLine::doHandlesDelete()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            qDeleteAll( vectorHandles );
            vectorHandles.clear();
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doFiniSelect();
            break;
    }
}

void DLine::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    switch ( getSelectType() )
    {
        case SelectResize:
            DHandle *pHandle;
            foreach( pHandle, vectorHandles )
            {
                // int nPoint = pHandle->getIndex();
                if ( pHandle->getFunction() == DHandle::FunctionFloatingTextCenter )
                {
                   pHandle->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter ); 
                   pFloatingTextCenter->setPos( pHandle->pos() );
                }
            }
            break;
        case SelectTransformRotate:
        case SelectTransformShear:
        case SelectMenu:
            break;
        case SelectOnly:
            doSyncSelect();
            break;
    }
}

void DLine::doSourcesCreate()
{
    Q_ASSERT( vectorSources.count() == 0 );

    vectorSources.resize( 2 );
    vectorSources[0] = new DSource( this, CBD::EOLBegin );
    vectorSources[1] = new DSource( this, CBD::EOLEnd );

    // floating text
    pFloatingTextCenter = new QGraphicsTextItem( getFloatingTextText() );
    pFloatingTextCenter->setDefaultTextColor( font.color );
    pFloatingTextCenter->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter );
    pFloatingTextCenter->setPlainText( getFloatingTextText() );
    getDiagram()->getScene()->addItem( pFloatingTextCenter );
}

void DLine::doSourcesVisible( bool b )
{
    // created in setProxy and then always exist
    Q_ASSERT( vectorSources.count() > 0 );

    DSource *pSource;
    foreach( pSource, vectorSources )
    {
        pSource->setVisible( b );
    }
}

void DLine::doSourcesDelete()
{
    //Q_ASSERT( vectorSources.count() > 0 );

    // disconnect from any connected sinks
    DSource *pSource;

    foreach( pSource, vectorSources )
    {
        // we are just cleaning up before we disappear so unglue & disconnect without trying to alter model
        if ( pSource->isGlued() ) doDisconnect( pSource->getType() );
    }

    if ( pFloatingTextCenter )
    { 
        delete pFloatingTextCenter;
        pFloatingTextCenter = nullptr;
    }

    // they should delete any DHandle they may have on their way out
    qDeleteAll( vectorSources );
    vectorSources.resize( 0 );
}

/*!
 * \brief Ensure sources are at begin/end points.
 *  
 * Begin and/or End points have moved.
 * 
 * \author pharvey (10/9/19)
 */
void DLine::doSourcesSync()
{
    Q_ASSERT( vectorSources.count() > 0 );

    vectorSources.at( 0 )->setPos( getProxy()->mapToScene( polygon.first() ) );
    vectorSources.at( 1 )->setPos( getProxy()->mapToScene( polygon.last() ) );

    if ( pFloatingTextCenter ) pFloatingTextCenter->setPos( getFloatingTextCenterPos() + pointDeltaFloatingTextCenter ); 
}

/*!
 * \brief Route our line so that pSource resides at pointScene.
 *  
 * The default is to simply move the source (and its point) to pointScene. This implicitly makes an 
 * adjustment to the line segment with pSource. So nothing fancy - no intelligence. 
 *  
 * Derived classes can override this to do something more intelligent. 
 *  
 * \author pharvey (2/27/20)
 * 
 * \param p 
 * \param pointScene 
 */
void DLine::doRoute( DSource *pSource, const QPointF &pointScene )
{
    doMoveSource( pSource, pointScene );
}

/*!
 * \brief Merge points (presumably after an AutoRoute). 
 *  
 * The points are in Cell coordinates. 
 * Only intermediate points are wanted - no Begin/End points (as they will not change). 
 * 
 * \author pharvey (3/7/20)
 * 
 * \param listCells 
 */
void DLine::doMergePoints( const QList<QPoint> &listCells )
{
    bool bHandles = ( vectorHandles.count() > 0 );
    if ( bHandles ) doHandlesDelete();
    QPointF pointBegin = polygon.first();
    QPointF pointEnd = polygon.last();
    polygon.clear();
    polygon.append( pointBegin );
    QPoint pointCell;
    foreach( pointCell, listCells )
    {
        polygon.append( getProxy()->mapFromScene( getDiagram()->mapCellToScene( pointCell ) ) );
    }
    polygon.append( pointEnd );
    if ( bHandles ) doHandlesCreate();

    return;
}

void DLine::doMergePoints( const QList<QPointF> &listPoints )
{
    bool bHandles = ( vectorHandles.count() > 0 );
    if ( bHandles ) doHandlesDelete();
    QPointF pointBegin = polygon.first();
    QPointF pointEnd = polygon.last();
    polygon.clear();
    polygon.append( pointBegin );
    QPointF point;
    foreach( point, listPoints )
    {
        polygon.append( getProxy()->mapFromScene( point ) );
    }
    polygon.append( pointEnd );
    if ( bHandles ) doHandlesCreate();

    return;
}

/*!
 * \brief Ensure pos is center of boundingRect.
 *  
 * In most cases we do not care where the pos is.
 * However; it matters when scaling, such as when in a group, as points are relative to pos. 
 *  
 * We set the pos to be center of bounding rect and then adjust all points accordingly. 
 *  
 * \author pharvey (11/5/20)
 */
void DLine::doNormalize()
{
    QPointF point = getProxy()->mapToParent( boundingRect().center() );
    if ( point == pointPos ) return;

    getProxy()->doPrepareGeometryChange();

    QPointF pointDelta = pointPos - point;

    DObject::setPosRaw( point );
    polygon.translate( pointDelta );
    if ( isSelected() ) doHandlesSync();
    doSourcesSync();
}

void DLine::doDrawBegin( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected )
{
    // needed for WLineEndComposed and not needed here
    // derived classes can use it
    Q_UNUSED(lineIntersected);

    W::WLineEndStyles nStyle = getStyle( CBD::EOLBegin );

    switch ( nStyle )
    {
        case W::WLineEndArrow:
            doDrawArrow( pPainter, lineSegment );
            break;
        case W::WLineEndBall:
            doDrawBall( pPainter, lineSegment );
            break;
        case W::WLineEndNone:
        case W::WLineEndComposed:
            break;
    };
}

void DLine::doDrawEnd( QPainter *pPainter, const QLineF &lineSegment, const QLineF &lineIntersected )
{
    // needed for WLineEndComposed and not needed here
    // derived classes can use it
    Q_UNUSED(lineIntersected);

    W::WLineEndStyles nStyle = getStyle( CBD::EOLEnd );

    switch ( nStyle )
    {
        case W::WLineEndArrow:
            doDrawArrow( pPainter, lineSegment );
            break;
        case W::WLineEndBall:
            doDrawBall( pPainter, lineSegment );
            break;
        case W::WLineEndNone:
        case W::WLineEndComposed:
            break;
    };
}

/*!
 * \brief Draws an arrow at one end of the line.
 *  
 * This was derived from code in Qt example; diagramscene. Another example 
 * can be found in SystemArchitect v3 code. That version includes drawing 
 * of a variety ends and attempts to orientate the ends. However; that code 
 * may be integrated with the intersect/truncate code. 
 *  
 * \author pharvey (11/13/19)
 * 
 * \param pPainter 
 * \param line 
 */
void DLine::doDrawArrow( QPainter *pPainter, const QLineF &line )
{
    // draw at end of line
    qreal arrowSize = 6;
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
    pPainter->drawPolygon( polygonArrow );
}

void DLine::doDrawBall( QPainter *pPainter, const QLineF &line )
{
    // draw at end of line
    pPainter->drawEllipse( line.p1(), 4, 4 );
}

QPolygonF DLine::getTruncated( QLineF &lineIntersectedBegin, QLineF &lineIntersectedEnd )
{
    //
    // \note    We work in scene coords in here. 
    //          But the return values are in item coords.
    //

    // convert to scene coords
    QPolygonF polygonScene = getProxy()->mapToScene( polygon );

    // truncate as needed
    QPointF pointIntersectBegin;
    if ( isTruncate( CBD::EOLBegin ) && getSource( CBD::EOLBegin )->isGlued() )
    {
        DRectangle *pRectangle  = getSource( CBD::EOLBegin )->getRectangle();
        QRectF      rect        = pRectangle->getProxy()->mapRectToScene( pRectangle->boundingRect() );
        QLineF      line( polygonScene.at( 1 ), polygonScene.first() );

        pointIntersectBegin  = getIntersect( line, rect, lineIntersectedBegin );
        if ( !pointIntersectBegin.isNull() )
            polygonScene[0] = pointIntersectBegin; 
    }

    // truncate as needed
    QPointF pointIntersectEnd;
    if ( isTruncate( CBD::EOLEnd ) && getSource( CBD::EOLEnd )->isGlued() )
    {
        DRectangle *pRectangle  = getSource( CBD::EOLEnd )->getRectangle();
        QRectF      rect        = pRectangle->getProxy()->mapRectToScene( pRectangle->boundingRect() );
        QLineF      line( polygonScene.at( polygonScene.count() - 2 ), polygonScene.last() );

        pointIntersectEnd  = getIntersect( line, rect, lineIntersectedEnd );
        if ( !pointIntersectEnd.isNull() )
            polygonScene[polygonScene.count()-1] = pointIntersectEnd; 
    }

    // convert to item coords
    QPolygonF polygonItem = getProxy()->mapFromScene( polygonScene );

    if ( !lineIntersectedBegin.isNull() )
        lineIntersectedBegin = QLineF( getProxy()->mapFromScene( lineIntersectedBegin.p1() ), getProxy()->mapFromScene( lineIntersectedBegin.p2() ) );

    if ( !lineIntersectedEnd.isNull() )
        lineIntersectedEnd = QLineF( getProxy()->mapFromScene( lineIntersectedEnd.p1() ), getProxy()->mapFromScene( lineIntersectedEnd.p2() ) );

    return polygonItem;
}

/*!
 * \brief Determine the point where given line intersects with the given rect. 
 *  
 * This is used to support painting when a DLine, connected to a DRectangle, needs 
 * to be truncated. 
 *  
 * This relies upon QLineF::intersect. It is used for each of the 4 sides of the rect. 
 *  
 * This is a simplified version of the solution used in the Qt example; diagramscene. 
 *  
 * Assumptions: 
 * 1. there will be exactly 1 intersection 
 * 2. the intersection will be QLineF::BoundedIntersection 
 * 
 * \author pharvey (11/13/19)
 * 
 * \param line segment (begin or end) of DLine
 * \param rect bounding rectangle of DRectangle 
 * \param lineIntersected Returns the rectangle line intersected when intersection point !isNull 
 * 
 * \return QPointF the intersection point 
 *                 isNull should be checked
 */
QPointF DLine::getIntersect( const QLineF &line, const QRectF &rect, QLineF &lineIntersected )
{
    // is entire line in rect? if so - we can not truncate
    if ( rect.contains( line.p1() ) && rect.contains( line.p2() ) )
    {
        return QPointF();
    }
     
    QPointF     pointIntersect;
    QPolygonF   polygonRect( rect );

    // assumption:
    // 1 - there will be exactly 1 intersection
    // 2 - it will be bounded

    // check each of the 4 sides of the rect for an intersection
    for ( int nSide = 0; nSide < 4; nSide++ ) 
    {
        lineIntersected = QLineF( polygonRect.at( nSide ), polygonRect.at( nSide+1 ) );
#if QT_VERSION < 0x060000
    QLineF::IntersectType nIntersectType = lineIntersected.intersect( line, &pointIntersect );
#else
    QLineF::IntersectType nIntersectType = lineIntersected.intersects( line, &pointIntersect );
#endif
        if ( nIntersectType == QLineF::BoundedIntersection )
            break;
    }

    if ( pointIntersect.isNull() )
    {
        lineIntersected = QLineF();
    }

    return pointIntersect;
}

Qt::Orientation DLine::getOrientation( const QPointF &point1, const QPointF &point2 )
{
    // only use this if guessing is ok when points are equal (or this never happens)
    // only use this if guessing is ok when line on a 45 degree angle (or this never happens)
    if ( abs( point1.x() - point2.x() ) < abs( point1.y() - point2.y() ) ) return Qt::Vertical;

    return Qt::Horizontal;
}

void DLine::doDumpHandlesVector()
{
    qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
    for ( int n = 0; n < vectorHandles.count(); n++ )
    {
       qDebug() << " Index: " << n << " -> " << vectorHandles[n]->getIndex() << " Function: " << vectorHandles[n]->getFunction()  << " \t = " << vectorHandles[n];
    }
}

void DLine::doDumpPoints()
{
    qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
    for ( int n = 0; n < polygon.count(); n++ )
    {
        qDebug() << " Index: " << n << "\t = " << polygon[n];
    }
}

void DLine::doDumpPointsScene()
{
    qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
    if ( !getProxy() ) return;
    for ( int n = 0; n < polygon.count(); n++ )
    {
        qDebug() << " Index: " << n << "\t = " << getProxy()->mapToScene( polygon[n] );
    }
}


