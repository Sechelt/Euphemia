#include "LibInfo.h"
#include "DCallOut.h"

#include "DGraphicsProxyItem.h"
#include "DSource.h"
#include "DRectangle.h"
#include "DSourcesWidget.h"
#include "DCallOutWidget.h"

DCallOut::DCallOut( ADObject *pObjectParent, const QString &stringName )
    : DLine( pObjectParent, stringName )
{
    rectText.setX( 0 );
    rectText.setY( 0 );
    rectText.setWidth( 50 );
    rectText.setHeight( 50 );

    nTextVAlign     = Qt::AlignVCenter;
    nTextHAlign     = Qt::AlignLeft;
    bWordWrap       = true;

    // default to a faint yellow
    brush.setColor( QColor( 255, 246, 143 ) );
}

DCallOut::~DCallOut()
{
}

void DCallOut::setPos( const QPointF &point )
{
    DLine::setPos( point );
    doCalculateShape();
    if ( isSelected() ) doHandlesSync();
}

void DCallOut::setPosRaw( const QPointF &point )
{   
    DLine::setPosRaw( point );
    doCalculateShape();
    if ( isSelected() ) doHandlesSync();
}

void DCallOut::setProxy( DGraphicsProxyItem *p )
{
    DLine::setProxy( p );
    doCalculateShape();
}

QPixmap DCallOut::getIcon()
{
    return QPixmap( ":D/CallOut" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We bypass DLine because we want a mix of rectangle and line properties. 
 *  
 * We bypass DObject and use AWObject because we implement pen/brush/font widgets differently. 
 * 
 * \author pharvey (5/10/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DCallOut::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    p->insertTab( 1, new DSourcesWidget( this, p ), tr("Sources") );
    p->insertTab( 1, new DCallOutWidget( this, p ), tr("CallOut") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DCallOut::getPropWidget( QWidget *pWidgetParent )
{
    // skip the drawing stuff that DObject puts in...
    // AWPropWidget *pPropWidget = AWObject::getPropWidget( pWidgetParent );

    AWPropWidget *pPropWidget = DObject::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Text"), new DCallOutTextWidget( this, pPropWidget ) );
    pPropWidget->addWidget( tr("Connector"), new DCallOutConnectorWidget( this, pPropWidget ) );

    return pPropWidget;

}

int DCallOut::getTextFlags() 
{
    if ( bWordWrap ) return (nTextHAlign | nTextVAlign | Qt::TextWordWrap);
    return (nTextHAlign | nTextVAlign);
}

QDomElement DCallOut::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff, children, and any references...
    QDomElement domElemThis = DLine::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    domElemThis.setAttribute( "TextWidth", rectText.width() );
    domElemThis.setAttribute( "TextHeight", rectText.height() );
    domElemThis.setAttribute( "TextVAlign", nTextVAlign );
    domElemThis.setAttribute( "TextHAlign", nTextHAlign );
    domElemThis.setAttribute( "WordWrap", bWordWrap );

    return domElemThis;
}

bool DCallOut::doLoad( QDomElement *pdomElemObject )
{
    if ( !DLine::doLoad( pdomElemObject ) ) return false;

    // Text box - we only need w/h because pos will be set to line-begin in doCalculateShape()
    qreal nWidth    = pdomElemObject->attribute( "TextWidth", QString::number( rectText.width() ) ).toDouble();
    qreal nHeight   = pdomElemObject->attribute( "TextHeight", QString::number( rectText.height() ) ).toDouble();
    rectText.setX( 0 );
    rectText.setY( 0 );
    rectText.setWidth( nWidth );
    rectText.setHeight( nHeight );

    // Text flags
    nTextVAlign     = (Qt::AlignmentFlag)pdomElemObject->attribute( "TextVAlign", QString::number( Qt::AlignVCenter ) ).toInt();
    nTextHAlign     = (Qt::AlignmentFlag)pdomElemObject->attribute( "TextHAlign", QString::number( Qt::AlignLeft ) ).toInt();
    bWordWrap       = pdomElemObject->attribute( "WordWrap", QString::number( bWordWrap ) ).toInt();

    return true;
}

void DCallOut::doPostLoad()
{
    DLine::doPostLoad();
    doCalculateShape();
}

QRectF DCallOut::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath DCallOut::shape() const
{
    QPainterPath path;
    path.addPolygon( polygonCallOut );
    return path;
}

void DCallOut::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    pPainter->setPen( pen );
    pPainter->setBrush( brush );

    pPainter->drawPolygon( polygonCallOut );

    pPainter->setFont( font.font );
    pPainter->setPen( font.color );
    QRectF rect = getProxy()->mapFromScene( rectText ).boundingRect();
    switch ( nTextDisplay )
    {
        case TextDisplayName:
            pPainter->drawText( rect, getTextFlags(), getName() );
            break;
        case TextDisplayCode:
            pPainter->drawText( rect, getTextFlags(), getCode() );
            break;
        case TextDisplayComment:
            pPainter->drawText( rect, getTextFlags(), getComment() );
            break;
        case TextDisplayNone:
            break;
    }

    if ( getStyle( CBD::EOLEnd ) != W::WLineEndNone )
    {
        QLineF line( polygonCallOut.first(), polygon.first() );
        doDrawEnd( pPainter, line, lineIntersectedEnd );
    }
}

void DCallOut::slotTextVAlign( Qt::AlignmentFlag n )
{
    if ( nTextVAlign == n ) return;
    nTextVAlign = n;
    getProxy()->update();
    setModified();
}

void DCallOut::slotTextHAlign( Qt::AlignmentFlag n )
{
    if ( nTextHAlign == n ) return;
    nTextHAlign = n;
    getProxy()->update();
    setModified();
}

void DCallOut::slotWordWrap( Qt::CheckState n )
{
    if ( bWordWrap == n ) return;
    bWordWrap = n;
    getProxy()->update();
    setModified();
}

void DCallOut::slotStyleBegin( W::WLineEndStyles n ) 
{
    Q_UNUSED(n); 
}

void DCallOut::slotTruncateBegin( Qt::CheckState n ) 
{ 
    Q_UNUSED(n); 
}

void DCallOut::slotTruncateEnd( Qt::CheckState n ) 
{ 
    bTruncateEnd = ( n==0 ? false : true ); 
    doCalculateShape();
    getProxy()->update(); 
    setModified();
}

void DCallOut::doMoveSource( DSource *pSource, const QPointF &pointScenePos )
{
    DLine::doMoveSource( pSource, pointScenePos );
    doCalculateShape();
}

/*!
 * \brief Create sources. 
 *  
 * We need this because we do not want to create floating text - as is done in DLine. 
 * 
 * \author pharvey (5/3/20)
 */
void DCallOut::doSourcesCreate()
{
    Q_ASSERT( vectorSources.count() == 0 );

    vectorSources.resize( 2 );
    vectorSources[0] = new DSource( this, CBD::EOLBegin );
    vectorSources[1] = new DSource( this, CBD::EOLEnd );
}

/*!
 * \brief Move a handle.
 *  
 * This replaces the version in DLine as our vectorHandles are completely different. 
 *  
 * \author pharvey (5/2/20)
 */
void DCallOut::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    getProxy()->doPrepareGeometryChange();
    int n = vectorHandles.indexOf( pHandle );
    if ( n < 0 ) return;

    // adopt the move with one exception - we do not allow the rect to be inverted
    if ( n == CBD::RectangleTopLeft )
    {
        QPointF point = pointScenePos;
        if ( point.x() > rectText.right() ) point.setX( rectText.right() );
        if ( point.y() > rectText.bottom() ) point.setY( rectText.bottom() );
        rectText.setTopLeft( point );
    }
    else if ( n == CBD::RectangleTopRight )
    {
        QPointF point = pointScenePos;
        if ( point.x() < rectText.left() ) point.setX( rectText.left() );
        if ( point.y() > rectText.bottom() ) point.setY( rectText.bottom() );
        rectText.setTopRight( point );
    }
    else if ( n == CBD::RectangleBottomRight )
    {
        QPointF point = pointScenePos;
        if ( point.x() < rectText.left() ) point.setX( rectText.left() );
        if ( point.y() < rectText.top() ) point.setY( rectText.top() );
        rectText.setBottomRight( point );
    }
    else if ( n == CBD::RectangleBottomLeft )
    {
        QPointF point = pointScenePos;
        if ( point.x() > rectText.right() ) point.setX( rectText.right() );
        if ( point.y() < rectText.top() ) point.setY( rectText.top() );
        rectText.setBottomLeft( point );
    }
    else
        return;

    doCalculateShape();
    if ( isSelected() ) doHandlesSync();

    return; 
}

/*!
 * \brief Create handles.
 *  
 * This replaces the version in DLine as our vectorHandles are completely different. 
 *  
 * \author pharvey (5/2/20)
 */
void DCallOut::doHandlesCreate()
{
    Q_ASSERT( vectorHandles.count() == 0 );

    vectorHandles.resize( 4 );

    DHandle *pHandle;

    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, rectText.topLeft() );
    vectorHandles[CBD::RectangleTopLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, rectText.topRight() );
    vectorHandles[CBD::RectangleTopRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, rectText.bottomRight() );
    vectorHandles[CBD::RectangleBottomRight] = pHandle;
    getProxy()->scene()->addItem( pHandle );

    pHandle = new DHandle( this, DHandle::FunctionResizeWidthHeight, DHandle::ConnectTypeNone, rectText.bottomLeft() );
    vectorHandles[CBD::RectangleBottomLeft] = pHandle;
    getProxy()->scene()->addItem( pHandle );
}

/*!
 * \brief Ensure handles are where they are supposed to be (we may have been moved or similar).
 *  
 * This replaces the version in DLine as our vectorHandles are completely different. 
 *  
 * \author pharvey (5/2/20)
 */
void DCallOut::doHandlesSync()
{
    Q_ASSERT( vectorHandles.count() > 0 );

    DHandle *pHandle;

    pHandle = vectorHandles[CBD::RectangleTopLeft];
    pHandle->setPos( rectText.topLeft() );

    pHandle = vectorHandles[CBD::RectangleTopRight];
    pHandle->setPos( rectText.topRight() );

    pHandle = vectorHandles[CBD::RectangleBottomRight];
    pHandle->setPos( rectText.bottomRight() );

    pHandle = vectorHandles[CBD::RectangleBottomLeft];
    pHandle->setPos( rectText.bottomLeft() );
}

void DCallOut::doCalculateShape()
{
    qreal nLineBaseWidth = 5;

    // lets do our work in scene coordinates
    // we are based off of the underlying DLine::polygon so...
    QLineF lineScene( getProxy()->mapToScene( polygon.first() ), getProxy()->mapToScene( polygon.last() ) );

    // truncate end of line as needed (no need to consider truncating begin)
    if ( isTruncate( CBD::EOLEnd ) && getSource( CBD::EOLEnd )->isGlued() )
    {   
        QPointF     pointIntersectEnd;
        DRectangle *pRectangle  = getSource( CBD::EOLEnd )->getRectangle();
        QRectF      rect        = pRectangle->getProxy()->mapRectToScene( pRectangle->boundingRect() );

        pointIntersectEnd = getIntersect( lineScene, rect, lineIntersectedEnd );
        if ( !pointIntersectEnd.isNull() ) lineScene.setP2( pointIntersectEnd ); 
    }
    else
        lineIntersectedEnd = QLineF();

    // use a fixed rect for text (for now) - center it over the begin point
    rectText.moveCenter( lineScene.p1() );

    // catch case where end point is in text rect
    if ( rectText.contains( lineScene.p2() ) )
    {
        QPolygonF polygonScene;
        polygonScene.append( rectText.topLeft() );
        polygonScene.append( rectText.topRight() );
        polygonScene.append( rectText.bottomRight() );
        polygonScene.append( rectText.bottomLeft() );
        // convert back to item coordinates as we assign it
        polygonCallOut = getProxy()->mapFromScene( polygonScene );
        return; 
    }

    // get intersection point to the text rect
    QLineF  lineIntersected;
    QPointF pointIntersected = getIntersect( lineScene, rectText, lineIntersected );

    // start - then work clockwise back to start
    QPolygonF polygonScene;
    polygonScene.append( lineScene.p2() );

    //
    Qt::Orientation nOrientation = Qt::Horizontal;
    if ( lineIntersected.p1().x() == lineIntersected.p2().x() ) nOrientation = Qt::Vertical;

    CBD::DirectionTypes nSide;
    if ( nOrientation == Qt::Horizontal )
    {
        if ( lineIntersected.p1().y() == rectText.top() ) nSide = CBD::DirectionN;
        else nSide = CBD::DirectionS; 
    }
    else
    { 
        if ( lineIntersected.p1().x() == rectText.left() ) nSide = CBD::DirectionW;
        else nSide = CBD::DirectionE; 
    }

    //
    QPointF pointCurrent;
    if ( nSide == CBD::DirectionN )
    {
        polygonScene.append( QPointF( pointIntersected.x() + nLineBaseWidth, pointIntersected.y() ) );
        polygonScene.append( rectText.topRight() );
        polygonScene.append( rectText.bottomRight() );
        polygonScene.append( rectText.bottomLeft() );
        polygonScene.append( rectText.topLeft() );
        polygonScene.append( QPointF( pointIntersected.x() - nLineBaseWidth, pointIntersected.y() ) );
    }
    else if ( nSide == CBD::DirectionS )
    {
        polygonScene.append( QPointF( pointIntersected.x() - nLineBaseWidth, pointIntersected.y() ) );
        polygonScene.append( rectText.bottomLeft() );
        polygonScene.append( rectText.topLeft() );
        polygonScene.append( rectText.topRight() );
        polygonScene.append( rectText.bottomRight() );
        polygonScene.append( QPointF( pointIntersected.x() + nLineBaseWidth, pointIntersected.y() ) );
    }
    else if ( nSide == CBD::DirectionE )
    {
        polygonScene.append( QPointF( pointIntersected.x(), pointIntersected.y() + nLineBaseWidth ) );
        polygonScene.append( rectText.bottomRight() );
        polygonScene.append( rectText.bottomLeft() );
        polygonScene.append( rectText.topLeft() );
        polygonScene.append( rectText.topRight() );
        polygonScene.append( QPointF( pointIntersected.x(), pointIntersected.y() - nLineBaseWidth ) );
    }
    else if ( nSide == CBD::DirectionW )
    {
        polygonScene.append( QPointF( pointIntersected.x(), pointIntersected.y() - nLineBaseWidth ) );
        polygonScene.append( rectText.topLeft() );
        polygonScene.append( rectText.topRight() );
        polygonScene.append( rectText.bottomRight() );
        polygonScene.append( rectText.bottomLeft() );
        polygonScene.append( QPointF( pointIntersected.x(), pointIntersected.y() + nLineBaseWidth ) );
    }

    // and back to where we started
    polygonScene.append( lineScene.p2() );

    // convert back to item coordinates as we assign it
    polygonCallOut = getProxy()->mapFromScene( polygonScene );
}

