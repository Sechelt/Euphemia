#include "LibInfo.h"
#include "DLayoutGrid.h"

#include "DGraphicsProxyItem.h"

DLayoutGrid::DLayoutGrid( ADObject *pObjectParent, const QString &stringName )
    : DLayout( pObjectParent, stringName )
{
}

DLayoutGrid::~DLayoutGrid()
{
}

QPixmap DLayoutGrid::getIcon()
{
    return QPixmap( ":D/GLayout" );
}

void DLayoutGrid::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    if ( hasChildren() && !isSelected() ) return;

    pPainter->setPen( pen );
    pPainter->setBrush( brush );
    pPainter->setFont( font.font );

    QRectF r;
    r.setTopLeft( pointPos );
    r.setSize( rect.size() );

    pPainter->drawRect( r );
}

bool DLayoutGrid::doEnter( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_UNUSED( pointScene );

    getProxy()->update();

    return true;
}

bool DLayoutGrid::doOver( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_UNUSED( pointScene );

    getProxy()->update();

    return true;
}

bool DLayoutGrid::doExit( DRectangleBase *p )
{
    Q_UNUSED( p );

    getProxy()->update();

    return true;
}

void DLayoutGrid::doDrag( DRectangleBase *p )
{
    doRemove( p );
}

bool DLayoutGrid::doDrop( DRectangleBase *p, const QPointF &pointScene )
{
    Q_UNUSED( p );
    Q_UNUSED( pointScene );

    // todo - layout adopts object

    pen.setWidth( 1 );
    getProxy()->update();

    return true;
}

void DLayoutGrid::doLayout()
{
    // do layout here

    getProxy()->update();
}

void DLayoutGrid::doInsert( DRectangleBase *p, int nX, int nY )
{
    Q_UNUSED( p );
    Q_UNUSED( nX );
    Q_UNUSED( nY );

    doLayout();
}

void DLayoutGrid::doRemove( DRectangleBase *p )
{
    Q_UNUSED( p );

    doLayout();
}

/*!
 * \brief Update our cache. 
 *  
 * Some things may be cached whenever we doLayout() but this cache is updated when; 
 *  
 * - object added 
 * - object removed 
 * - object changed its sizeHint, sizeMinumum, sizeMaximum 
 * 
 * \author pharvey (9/21/20)
 */
void DLayoutGrid::doUpdateSelf()
{
}




