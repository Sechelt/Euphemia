/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LHandle.h"

#include "LObject.h"

#define HANDLE_SOURCE_SIZE 8
#define HANDLE_SINK_SIZE 10
#define HANDLE_SIZE 8
#define HANDLE_INDICATOR_SIZE 32

LHandle::LHandle( QGraphicsObject *pObject, QGraphicsView *pView, Type nType, const QPointF &pointCenter, ConnectTypes nConnectType )
    : QWidget( pView->viewport() )
{
    pGraphicsObject     = pObject;
    this->pView         = pView;
    this->nType         = nType;
    this->nConnectType  = nConnectType;

    resize( HANDLE_SIZE, HANDLE_SIZE );
    setCenter( pointCenter ); 

    doInit();
}

LHandle::LHandle( LObject *pObject, QGraphicsView *pView, Type nType, const QPointF &pointCenter, ConnectTypes nConnectType )
    : QWidget( pView->viewport() )
{
    pLayerObject        = pObject;
    this->pView         = pView;
    this->nType         = nType;
    this->nConnectType  = nConnectType;

    resize( HANDLE_SIZE, HANDLE_SIZE );
    setCenter( pointCenter ); 

    doInit();
}

void LHandle::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED( pEvent );

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );

    if ( !pixmap.isNull() )
    { 
        painter.drawPixmap( rect().topLeft(), pixmap );
        return;
    }

    switch ( nType )
    {
        case TypeMovePoint:
        case TypeGradientIntermediate:
        case TypeGradientRadius:
        case TypeGradientFocal:
        case TypeGradientAngle:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawEllipse( rect() );
            }
            break;
        case TypePointFactory:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( Qt::white );
                painter.drawRect( rect() );
                painter.setBrush( Qt::green );
                painter.drawEllipse( rect().x() + 2, rect().y() + 2, rect().width() - 4, rect().height() - 4 );
            }
            break;
        case TypeDrag:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeLeft:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeRight:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeTop:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeBottom:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeTopRight:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeTopLeft:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeBottomRight:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeSizeBottomLeft:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                painter.drawRect( rect() );
            }
            break;
        case TypeFillSeed:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::green ) );
                QRect r( 0, 0, 4, 4 );
                r.moveCenter( rect().center() );
                painter.drawRect( r );

                painter.setPen( QPen( Qt::black ) );
                painter.drawLine( rect().topLeft(), rect().bottomRight() );
                painter.drawLine( rect().bottomLeft(), rect().topRight() );
            }
            break;
        case TypeGradientStart:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::darkGreen ) );
                painter.drawEllipse( rect() );
            }
            break;
        case TypeGradientStop:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::darkRed ) );
                painter.drawEllipse( rect() );
            }
            break;
        case FunctionConnect:
        {
            switch ( nConnectType )
            {
                case ConnectTypeSource: // socket
                {
                    painter.setPen( QPen( Qt::black ) );
                    painter.setBrush( QBrush( Qt::green ) );
                    painter.drawEllipse( rect() );
                    if ( !bConnected )
                    { 
                        painter.setPen( QPen( Qt::white ) );
                        painter.setBrush( QBrush( Qt::white ) );
                        QRectF r = rect();
                        r.setWidth( HANDLE_SOURCE_SIZE / 2 ); 
                        r.setHeight( HANDLE_SOURCE_SIZE / 2 ); 
                        r.moveCenter( QPointF( 0, 0 ) );
                        painter.drawEllipse( r );
                    }
                }
                break;

                case ConnectTypeSink: // pin
                {
                    QRectF r = rect();
                    r.setWidth( HANDLE_SINK_SIZE / 2 ); 
                    r.setHeight( HANDLE_SINK_SIZE / 2 ); 
                    r.moveCenter( QPointF( 0, 0 ) );
                    painter.setPen( QPen( Qt::black ) );
                    painter.setBrush( QBrush( Qt::green ) );
                    painter.drawEllipse( r );
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
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
            painter.drawLine( 0, rect().top() + 2, 0, rect().bottom() - 2 );
            painter.drawLine( rect().left() + 2, 0, rect().right() - 2, 0 );
        }
        break;

        case FunctionRemoveLineSegment:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
            painter.drawLine( rect().left() + 2, 0, rect().right() - 2, 0 );
        }
        break;

        case FunctionResizeWidth:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionResizeHeight:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionResizeWidthHeight:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionResizeLayoutWidthHeight:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::magenta ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionSelectTopLeft:
        {
            painter.setPen( Qt::black );
            painter.setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect().topLeft().x() + 2;
            qreal nY = rect().topLeft().y() + 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX + nS, nY );
            polygon << QPointF( nX + nS, nY + nW );
            polygon << QPointF( nX + nW, nY + nW ); // arm-pit
            polygon << QPointF( nX + nW, nY + nS );
            polygon << QPointF( nX, nY + nS );

            painter.drawPolygon( polygon );
        }
        break;

        case FunctionSelectTopRight:
        {
            painter.setPen( Qt::black );
            painter.setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect().topRight().x() - 2;
            qreal nY = rect().topRight().y() + 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX, nY + nS );
            polygon << QPointF( nX - nW, nY + nS );
            polygon << QPointF( nX - nW, nY + nW );
            polygon << QPointF( nX - nS, nY + nW ); // arm-pit
            polygon << QPointF( nX - nS, nY );

            painter.drawPolygon( polygon );
        }
        break;

        case FunctionSelectBottomRight:
        {
            painter.setPen( Qt::black );
            painter.setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect().bottomRight().x() - 2;
            qreal nY = rect().bottomRight().y() - 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX - nS, nY );
            polygon << QPointF( nX - nS, nY - nW );
            polygon << QPointF( nX - nW, nY - nW ); // arm-pit
            polygon << QPointF( nX - nW, nY - nS ); 
            polygon << QPointF( nX, nY - nS );

            painter.drawPolygon( polygon );
        }
        break;

        case FunctionSelectBottomLeft:
        {
            painter.setPen( Qt::black );
            painter.setBrush( QBrush( Qt::yellow ) );
            QPolygonF polygon;
            qreal nX = rect().bottomLeft().x() + 2;
            qreal nY = rect().bottomLeft().y() - 2;
            qreal nW = 4;
            qreal nS = HANDLE_SIZE - 2;
            polygon << QPointF( nX, nY ) << QPointF( nX, nY - nS );
            polygon << QPointF( nX + nW, nY - nS );
            polygon << QPointF( nX + nW, nY - nW ); // arm-pit
            polygon << QPointF( nX + nS, nY - nW );
            polygon << QPointF( nX + nS, nY );

            painter.drawPolygon( polygon );
        }
        break;

        case FunctionFloatingTextBegin:
        case FunctionFloatingTextEnd:
        case FunctionFloatingTextCenter:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::yellow ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionBaseTransform:
            {
                painter.setPen( QPen( Qt::black ) );
                painter.setBrush( QBrush( Qt::red ) );
                painter.drawEllipse( rect() );
            }
        break;

        case FunctionRotateIndicator:
            {
                painter.drawImage( rect(), QImage( ":W/rotate" ) );
            }
        break;

        case FunctionShearHIndicator:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
            painter.drawLine( rect().topLeft(), rect().bottomRight() );
        }
        break;

        case FunctionShearVIndicator:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
            painter.drawLine( rect().bottomLeft(), rect().topRight() );
        }
        break;

        case FunctionSlide:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionSlideBoth:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionMenu:
        {
            // this is back up - we should have a pixmap for this one
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionManipulateOther:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::green ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionAnchor:
        {
            painter.drawImage( rect(), QImage( ":DR/Anchor" ) );
        }
        break;

        case FunctionMove:
        {
            painter.setPen( QPen( Qt::magenta ) );
            painter.setBrush( QBrush( Qt::black ) );
            painter.drawRect( rect() );
        }
        break;

        case FunctionInsertLayout:
        {
            painter.setPen( QPen( Qt::black ) );
            painter.setBrush( QBrush( Qt::magenta ) );
            painter.drawEllipse( rect() );
            painter.drawLine( 0, rect().top() + 2, 0, rect().bottom() - 2 );
            painter.drawLine( rect().left() + 2, 0, rect().right() - 2, 0 );
        }
        break;
    }
}

void LHandle::setCenter( const QPointF &pointCenter )
{
    QPointF pointTopLeft( pointCenter.x() - rect().width() / 2, pointCenter.y() - rect().height() / 2 );
    move( pointTopLeft.toPoint() );
}

void LHandle::doMoveBy( qreal nDeltaX, qreal nDeltaY )
{
    doMoveBy( QPointF( nDeltaX, nDeltaY ) );
}

void LHandle::doMoveBy( const QPointF &pointDelta )
{
    move( pos() + pointDelta .toPoint());
}

void LHandle::doInit()
{
    // set cursor
    switch ( nType )
    {
        case TypeMovePoint:
        case TypeGradientIntermediate:
        case TypeGradientRadius:
        case TypeGradientFocal:
        case TypeGradientAngle:
        case TypeGradientStart:
        case TypeGradientStop:
            setCursor( QCursor( Qt::PointingHandCursor ) );
            break;
        case TypePointFactory:
            setCursor( QCursor( Qt::CrossCursor ) );
            break;
        case TypeDrag:
            setCursor( QCursor( Qt::OpenHandCursor ) );
            break;
        case TypeSizeLeft:
        case TypeSizeRight:
            setCursor( QCursor( Qt::SizeHorCursor ) );
            break;
        case TypeSizeTop:
        case TypeSizeBottom:
            setCursor( QCursor( Qt::SizeVerCursor ) );
            break;
        case TypeSizeTopRight:
        case TypeSizeBottomLeft:
            setCursor( QCursor( Qt::SizeBDiagCursor ) );
            break;
        case TypeSizeTopLeft:
        case TypeSizeBottomRight:
            setCursor( QCursor( Qt::SizeFDiagCursor ) );
            break;
        case TypeFillSeed:
            break;
        case FunctionConnect: 
        case FunctionNewLineSegment:
        case FunctionRemoveLineSegment:
        case FunctionResizeWidth:
        case FunctionResizeHeight:
        case FunctionResizeWidthHeight:
        case FunctionResizeLayoutWidthHeight:
        case FunctionSelectTopLeft:
        case FunctionSelectTopRight:
        case FunctionSelectBottomRight:
        case FunctionSelectBottomLeft:
        case FunctionFloatingTextBegin:
        case FunctionFloatingTextEnd:
        case FunctionFloatingTextCenter:
        case FunctionBaseTransform:
        case FunctionRotateIndicator:
        case FunctionShearHIndicator:
        case FunctionShearVIndicator:
        case FunctionSlide:
        case FunctionSlideBoth:
        case FunctionMenu:
        case FunctionManipulateOther:
            setCursor( QCursor( Qt::PointingHandCursor ) );
            break;
        case FunctionAnchor:
        case FunctionMove:
            setCursor( QCursor( Qt::OpenHandCursor ) );
            break;
        case FunctionInsertLayout:
            break;
    }
}


