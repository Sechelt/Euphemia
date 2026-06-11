/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#ifndef H_LHandle
#define H_LHandle

#include <QtWidgets>

class LObject;

/*!
 * \brief Handle, typically, used to allow User to manipulate things on the layer. 
 *  
 * The handles are a child of the current QGraphivsView::viewport. 
 * This is usually the first view. This is done to prevent the handles 
 * from being scaled. It also means that the handles do not show up in 
 * alternate views such as the panner. 
 *  
 * The handle is pretty dumb - it just knows how to paint itself. 
 * The LLayer (or its tools) manages; creating, positioning and 
 * deleting the handles.
 *  
 * The LLayer must also reposition any handles when the view is 
 * scrolled or scaled. This is done in; 
 *  
 *  \sa LLayer::doZoomChanged()
 *  \sa LLayer::doScrolled()
 *  
 * \author pharvey (2/10/23)
 */
class LHandle : public QWidget
{
    Q_OBJECT
public:
    enum ConnectTypes
    {
        ConnectTypeSource,
        ConnectTypeSink,
        ConnectTypeNone
    };

    enum Type
    {
        TypeDrag,
        TypeMovePoint,
        TypePointFactory,
        TypeSizeLeft,
        TypeSizeRight,
        TypeSizeTop,
        TypeSizeBottom,
        TypeSizeTopRight,
        TypeSizeTopLeft,
        TypeSizeBottomRight,
        TypeSizeBottomLeft,
        TypeFillSeed,
        TypeGradientStart,
        TypeGradientStop,
        TypeGradientIntermediate,
        TypeGradientRadius,
        TypeGradientFocal,
        TypeGradientAngle,
        FunctionConnect,            // begin/end of line 
        FunctionNewLineSegment,     // new point added if moved
        FunctionRemoveLineSegment,  // allows intermediate point to be moved. point removed if causes segments to line up  
        FunctionResizeWidth,
        FunctionResizeHeight,
        FunctionResizeWidthHeight,
        FunctionResizeLayoutWidthHeight,
        FunctionSelectTopLeft,
        FunctionSelectTopRight,
        FunctionSelectBottomRight,
        FunctionSelectBottomLeft,
        FunctionFloatingTextBegin,
        FunctionFloatingTextEnd,
        FunctionFloatingTextCenter,
        FunctionBaseTransform,
        FunctionRotateIndicator,
        FunctionShearHIndicator,
        FunctionShearVIndicator,
        FunctionSlide,
        FunctionSlideBoth,
        FunctionMenu,
        FunctionManipulateOther,
        FunctionAnchor,
        FunctionMove,
        FunctionInsertLayout
    };

    // mostly to set a background shape
    enum Shape
    {
        ShapeCircle,
        ShapeSquare
    };

    LHandle( QGraphicsObject *pObject, QGraphicsView *pView, Type nType, const QPointF &pointCenter, ConnectTypes nConnectType = ConnectTypeNone );
    LHandle( LObject *pObject, QGraphicsView *pView, Type nType, const QPointF &pointCenter, ConnectTypes nConnectType = ConnectTypeNone );

    void setType( Type n )                  { nType = n;            }
    void setCenter( const QPointF &point );
    void setDelta( const QPointF &point )   { pointDelta = point;   }
    void setConnected( bool b = true )      { bConnected = b;       }
    void setConnectType( ConnectTypes n );
    void setPixmap( const QPixmap &pixmap );

    Type                getType()           { return nType;         }
    QPointF             getCenter()         { return geometry().center();   }
    QPointF             getDelta()          { return pointDelta;    }
    bool                getConnected()      { return bConnected;    } 
    ConnectTypes        getConnectType()    { return nConnectType;  }
    QPixmap             getPixmap()         { return pixmap;            }
    QGraphicsObject *   getGraphicsObject() { return pGraphicsObject;   }
    LObject *           getLayerObject()    { return pLayerObject;      }

    void doMoveBy( qreal nX, qreal nY );
    void doMoveBy( const QPointF & );

protected:
    QGraphicsView *     pView;                      /*!< view - we are child of its viewport                                    */
    Type                nType = TypeMovePoint;
    QPointF             pointDelta;                 /*!< delta from a reference point (not used if isNull())                    */
    bool                bConnected;                 /*!< affects appearance                                                     */
    ConnectTypes        nConnectType;               /*!< affects appearance                                                     */
    QPixmap             pixmap;                     /*!< use pixmap instead of drawing it based upon type etc                   */
    QGraphicsObject *   pGraphicsObject = nullptr;  /*!< object we are working on (we should always have one)                   */
    LObject *           pLayerObject = nullptr;     /*!< object we are working on (we do not always have one of these)          */

    void paintEvent( QPaintEvent *pEvent );

    void doInit();
};

#endif


