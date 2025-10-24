#include "LibInfo.h"
#include "DHandle.h"
#include "DGraphicsProxyItem.h"

#define HANDLE_SOURCE_SIZE 8
#define HANDLE_SINK_SIZE 10
#define HANDLE_SIZE 8
#define HANDLE_INDICATOR_SIZE 32

DHandle::DHandle( DObject *pObject, Functions nFunction, ConnectTypes nConnectType, const QPointF &pointPos  )
    : QGraphicsObject()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    // DHandle is a sibling (not a child) of its DObject 
    // Ensure the handle is on top of everything.
    setZValue( ZORDER_HANDLE );

    nIndex = -1;

    this->pObject = pObject;
    this->nFunction = nFunction;
    this->nConnectType = nConnectType;

    bConnected = false;

    // set our width/height
    qreal nX = 4;
    qreal nY = 4;

    switch ( nConnectType )
    {
        case ConnectTypeSource:
            nX = HANDLE_SOURCE_SIZE / 2;
            nY = HANDLE_SOURCE_SIZE / 2;
            break;
        case ConnectTypeSink:
            nX = HANDLE_SINK_SIZE / 2;
            nY = HANDLE_SINK_SIZE / 2;
            break;
        case ConnectTypeNone:
        default:
            nX = HANDLE_SIZE / 2;
            nY = HANDLE_SIZE / 2;
            break;
    }

    if ( nFunction == FunctionRotateIndicator )
    { 
        nX = HANDLE_INDICATOR_SIZE / 2;
        nY = HANDLE_INDICATOR_SIZE / 2;
    }

    if ( nFunction == FunctionShearHIndicator ) setTransform( QTransform().rotate( 45 ) );
    if ( nFunction == FunctionShearVIndicator ) setTransform( QTransform().rotate( 45 ) );

    // set bounding rect (origin is in middle of rect)
    rect.setTopLeft( QPointF( -nX, -nY ) );
    rect.setBottomRight( QPointF( nX, nY ) );

    setPos( pointPos );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

DHandle::~DHandle()
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] DESTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void DHandle::setPixmap( const QPixmap &pixmap ) 
{ 
    this->pixmap = pixmap; 
    if ( pixmap.isNull() )
    {
        return; 
    }
    qreal nX = pixmap.size().width() / 2;
    qreal nY = pixmap.size().height() / 2;

    rect.setTopLeft( QPointF( -nX, -nY ) );
    rect.setBottomRight( QPointF( nX, nY ) );
}

QRectF DHandle::boundingRect() const
{
    return rect;
}

void DHandle::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *nOption, QWidget *pWidget )
{
    Q_UNUSED( nOption );
    Q_UNUSED( pWidget );

    if ( !pixmap.isNull() )
    { 
        pPainter->drawPixmap( rect.topLeft(), pixmap );
        return;
    }

    switch ( nFunction )
    {
        case FunctionConnect:
        {
            switch ( nConnectType )
            {
                case ConnectTypeSource: // socket
                {
                    pPainter->setPen( QPen( Qt::black ) );
                    pPainter->setBrush( QBrush( Qt::green ) );
                    pPainter->drawEllipse( rect );
                    if ( !bConnected )
                    { 
                        pPainter->setPen( QPen( Qt::white ) );
                        pPainter->setBrush( QBrush( Qt::white ) );
                        QRectF r = rect;
                        r.setWidth( HANDLE_SOURCE_SIZE / 2 ); 
                        r.setHeight( HANDLE_SOURCE_SIZE / 2 ); 
                        r.moveCenter( QPointF( 0, 0 ) );
                        pPainter->drawEllipse( r );
                    }
                }
                break;

                case ConnectTypeSink: // pin
                {
                    QRectF r = rect;
                    r.setWidth( HANDLE_SINK_SIZE / 2 ); 
                    r.setHeight( HANDLE_SINK_SIZE / 2 ); 
                    r.moveCenter( QPointF( 0, 0 ) );
                    pPainter->setPen( QPen( Qt::black ) );
                    pPainter->setBrush( QBrush( Qt::green ) );
                    pPainter->drawEllipse( r );
                }
                break;
                case ConnectTypeNone:
                    // error: failure to specify a proper connect type
                break;
            }
        }
        break;

        case FunctionNewLineSegment:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
            pPainter->drawLine( 0, rect.top() + 2, 0, rect.bottom() - 2 );
            pPainter->drawLine( rect.left() + 2, 0, rect.right() - 2, 0 );
        }
        break;

        case FunctionRemoveLineSegment:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
            pPainter->drawLine( rect.left() + 2, 0, rect.right() - 2, 0 );
        }
        break;

        case FunctionResizeWidth:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionResizeHeight:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionResizeWidthHeight:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionResizeLayoutWidthHeight:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::magenta ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionSelectTopLeft:
        {
            pPainter->setPen( Qt::black );
            pPainter->setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect.topLeft().x() + 2;
            qreal nY = rect.topLeft().y() + 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX + nS, nY );
            polygon << QPointF( nX + nS, nY + nW );
            polygon << QPointF( nX + nW, nY + nW ); // arm-pit
            polygon << QPointF( nX + nW, nY + nS );
            polygon << QPointF( nX, nY + nS );

            pPainter->drawPolygon( polygon );
        }
        break;

        case FunctionSelectTopRight:
        {
            pPainter->setPen( Qt::black );
            pPainter->setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect.topRight().x() - 2;
            qreal nY = rect.topRight().y() + 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX, nY + nS );
            polygon << QPointF( nX - nW, nY + nS );
            polygon << QPointF( nX - nW, nY + nW );
            polygon << QPointF( nX - nS, nY + nW ); // arm-pit
            polygon << QPointF( nX - nS, nY );

            pPainter->drawPolygon( polygon );
        }
        break;

        case FunctionSelectBottomRight:
        {
            pPainter->setPen( Qt::black );
            pPainter->setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect.bottomRight().x() - 2;
            qreal nY = rect.bottomRight().y() - 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX - nS, nY );
            polygon << QPointF( nX - nS, nY - nW );
            polygon << QPointF( nX - nW, nY - nW ); // arm-pit
            polygon << QPointF( nX - nW, nY - nS ); 
            polygon << QPointF( nX, nY - nS );

            pPainter->drawPolygon( polygon );
        }
        break;

        case FunctionSelectBottomLeft:
        {
            pPainter->setPen( Qt::black );
            pPainter->setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect.bottomLeft().x() + 2;
            qreal nY = rect.bottomLeft().y() - 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX, nY - nS );
            polygon << QPointF( nX + nW, nY - nS );
            polygon << QPointF( nX + nW, nY - nW ); // arm-pit
            polygon << QPointF( nX + nS, nY - nW );
            polygon << QPointF( nX + nS, nY );

            pPainter->drawPolygon( polygon );
        }
        break;

        case FunctionFloatingTextBegin:
        case FunctionFloatingTextEnd:
        case FunctionFloatingTextCenter:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::yellow ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionBaseTransform:
            {
                pPainter->setPen( QPen( Qt::black ) );
                pPainter->setBrush( QBrush( Qt::red ) );
                pPainter->drawEllipse( rect );
            }
        break;

        case FunctionRotateIndicator:
            {
                pPainter->drawImage( rect, QImage( ":W/rotate" ) );
            }
        break;

        case FunctionShearHIndicator:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
            pPainter->drawLine( rect.topLeft(), rect.bottomRight() );
        }
        break;

        case FunctionShearVIndicator:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
            pPainter->drawLine( rect.bottomLeft(), rect.topRight() );
        }
        break;

        case FunctionSlide:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionSlideBoth:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionMenu:
        {
            // this is back up - we should have a pixmap for this one
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionManipulateOther:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::green ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionAnchor:
        {
            pPainter->drawImage( rect, QImage( ":D/Anchor" ) );
        }
        break;

        case FunctionMove:
        {
            pPainter->setPen( QPen( Qt::magenta ) );
            pPainter->setBrush( QBrush( Qt::black ) );
            pPainter->drawRect( rect );
        }
        break;

        case FunctionInsertLayout:
        {
            pPainter->setPen( QPen( Qt::black ) );
            pPainter->setBrush( QBrush( Qt::magenta ) );
            pPainter->drawEllipse( rect );
            // pPainter->drawLine( rect.left(), rect.height() / 2, rect.right(), rect.height() / 2 );
            // pPainter->drawLine( rect.width() / 2, rect.top(), rect.width() / 2, rect.bottom() );
            pPainter->drawLine( 0, rect.top() + 2, 0, rect.bottom() - 2 );
            pPainter->drawLine( rect.left() + 2, 0, rect.right() - 2, 0 );
        }
        break;
    }
}


