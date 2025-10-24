#include "RouteScene.h"

RouteScene::RouteScene( QObject *pObject )
    : QGraphicsScene( 0, 0, 1000, 1000, pObject )
{
    pRoute = nullptr;

    setBackgroundBrush( Qt::lightGray );

    sizeCell.setWidth( 64 );
    sizeCell.setHeight( 64 );
    sizeGrid.setWidth( round( width() / sizeCell.width() ) );
    sizeGrid.setHeight( round( height() / sizeCell.height() ) );

    for ( int nRow = 0; nRow < sizeGrid.height(); nRow++ )
    {
        grid.append( RSourceRow( sizeGrid.width() ) );
    }

    nMode = ModeClear;
}

RouteScene::~RouteScene()
{
}

void RouteScene::setRoute( RRoute *p ) 
{ 
    pRoute = p; 
    if ( pRoute ) connect( pRoute, SIGNAL(signalChanged()), SLOT(slotUpdate()) );
}

QWidget *RouteScene::getControls( QWidget *pParent )
{
    QWidget *pWidget = new QWidget( pParent );
    QVBoxLayout *pLayout = new QVBoxLayout( pWidget );
    pLayout->addWidget( new QLabel( "Cell", pWidget ) );
    QComboBox *pModes = new QComboBox( pWidget );
    pModes->addItem( "Clear" );
    pModes->addItem( "Line" );
    pModes->addItem( "Shape" );
    pModes->addItem( "Begin" );
    pModes->addItem( "End" );
    pModes->addItem( "Info" );
    connect( pModes, SIGNAL(textActivated(const QString &)), SLOT(slotMode(const QString &)) );
    pLayout->addWidget( pModes );

    return pWidget;
}

RSourceCell RouteScene::getCell( const QPoint &pointCell )
{
    Q_ASSERT( isValid( pointCell ) );
    return grid[pointCell.y()][pointCell.x()];
}

QPoint RouteScene::getBegin() 
{ 
    return pointBegin; 
}

QPoint RouteScene::getEnd() 
{ 
    return pointEnd; 
}

bool RouteScene::isValid( const QPoint &pointCell )
{
    return ( pointCell.x() >=0 && pointCell.x() < sizeGrid.width() &&
             pointCell.y() >=0 && pointCell.y() < sizeGrid.height());
}

QPoint RouteScene::mapSceneToCell( const QPointF &pointScene )
{
    return QPoint( pointScene.x() / sizeCell.width(), pointScene.y() / sizeCell.height() );
}

/*!
 * \brief Map *top-left* of cell to scene.
 *  
 * This is different then RSource::mapCellToScene. 
 *  
 * \author pharvey (5/14/20)
 * 
 * \param pointCell 
 * 
 * \return QPointF 
 */
QPointF RouteScene::mapCellToScene( const QPoint &pointCell )
{
    // top left corner of cell
    return QPointF( pointCell.x() * sizeCell.width(), pointCell.y() * sizeCell.height() );
}

void RouteScene::slotMode( const QString &s )
{
    if ( s == "Clear" ) setMode( ModeClear );
    else if ( s == "Line" ) setMode( ModeLine );
    else if ( s == "Shape" ) setMode( ModeShape );
    else if ( s == "Begin" ) setMode( ModeBegin );
    else if ( s == "End" ) setMode( ModeEnd );
    else if ( s == "Info" ) setMode( ModeInfo );
}

void RouteScene::slotUpdate()
{
    update();
}

void RouteScene::mousePressEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    // which cell?
    QPoint pointCell = mapSceneToCell( pMouseEvent->scenePos() );
    switch ( nMode )
    {
        case ModeClear:
            grid[pointCell.y()][pointCell.x()].nContent = RSourceCell::CellContentClear;
            break;
        case ModeLine:
            grid[pointCell.y()][pointCell.x()].nContent = RSourceCell::CellContentLine;
            break;
        case ModeShape:
            grid[pointCell.y()][pointCell.x()].nContent = RSourceCell::CellContentShape;
            break;
        case ModeBegin:
            pointBegin = pointCell;
            break;
        case ModeEnd:
            pointEnd = pointCell;
            break;
        case ModeInfo:
            if ( pRoute ) pRoute->doInfo( pointCell );
            return;
    }
    // invalidate( pMouseEvent->pos().x(), pMouseEvent->pos().y(), sizeCell.width(), sizeCell.height() );
    update();

    QGraphicsScene::mousePressEvent( pMouseEvent );
}

void RouteScene::mouseMoveEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    QPoint pointCell = mapSceneToCell( pMouseEvent->scenePos() );
    switch ( nMode )
    {
        case ModeClear:
            break;
        case ModeLine:
            break;
        case ModeShape:
            break;
        case ModeBegin:
            pointBegin = pointCell;
            if ( pRoute ) pRoute->slotRoute();
            else update();
            break;
        case ModeEnd:
            pointEnd = pointCell;
            if ( pRoute ) pRoute->slotRoute(); 
            else update();
            break;
        case ModeInfo:
            return;
    }
    QGraphicsScene::mouseMoveEvent( pMouseEvent );
}

void RouteScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *pMouseEvent )
{
    QGraphicsScene::mouseReleaseEvent( pMouseEvent );
}

void RouteScene::drawBackground( QPainter *pPainter, const QRectF &rect )
{
    Q_UNUSED(rect);

    for ( int nCellX = 0; nCellX < sizeGrid.width(); nCellX++ )
    {
        for ( int nCellY = 0; nCellY < sizeGrid.height(); nCellY++ )
        {
            // we paint our cell based upon what we know (ModeTypes, Begin/End)
            QPoint pointCell( nCellX, nCellY );
            QPointF pointScene = mapCellToScene( pointCell );
            QRectF r( pointScene.x(), pointScene.y(), sizeCell.width(), sizeCell.height() );
            RSourceCell sc = grid[nCellY][nCellX];

            // mode
            if ( sc.nContent == RSourceCell::CellContentShape )
            {
                pPainter->setBrush( QBrush( Qt::black ) );
                pPainter->setPen( Qt::black );
                pPainter->drawRect( r );
            }
            else if ( sc.nContent == RSourceCell::CellContentLine )
            {
                pPainter->setBrush( QBrush( Qt::darkGray ) );
                pPainter->setPen( Qt::black );
                pPainter->drawRect( r );
            }
            else
            {
                pPainter->setBrush( QBrush( Qt::white ) );
                pPainter->setPen( Qt::black );
                pPainter->drawRect( r );
            }

            // route will embelish the cell based upon what it knows (but not the begin/end)
            if ( pRoute )
            {
                pRoute->doPaint( pPainter, QPoint( nCellX, nCellY ) );
            }

            // begin/end
            if ( pointCell == pointBegin )
            {
                QRectF rect( 0, 0, sizeCell.width() / 2, sizeCell.height() / 2 );
                rect.moveCenter( r.center() );
                pPainter->setPen( Qt::black );
                pPainter->setBrush( Qt::green );
                pPainter->drawRect( rect );
            }
            else if ( pointCell == pointEnd )
            { 
                QRectF rect( 0, 0, sizeCell.width() / 2, sizeCell.height() / 2 );
                rect.moveCenter( r.center() );
                pPainter->setPen( Qt::black );
                pPainter->setBrush( Qt::red );
                pPainter->drawRect( rect );
            }
        }
    }
}

void RouteScene::contextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
    Q_UNUSED( pEvent );
}

void RouteScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    QGraphicsScene::mouseDoubleClickEvent( pEvent );
}


