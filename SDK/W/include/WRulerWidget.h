/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#ifndef H_WRulerWidget
#define H_WRulerWidget

#include "W.h"

class WRulerCornerWidget: public QWidget
{
    Q_OBJECT
public:
    WRulerCornerWidget( QWidget *pWidgetParent ); 

    virtual QSize sizeHint() const;
};

class WRulerWidget : public QWidget
{
    Q_OBJECT
public:
        // nMinor 24, nMajour 2 corresponds nicely with a 48x48 grid
    WRulerWidget( QWidget *pwidgetParent, Qt::Orientation dir = Qt::Horizontal, int nMajour = 32 );
    virtual ~WRulerWidget();

    // SETTERS
    void setMinor( qreal n );
    void setMajour( int n );

    // GETTERS
    qreal getPos()    { return nPosOnDocument;      }
    qreal getMinor()  { return nMinor;    }
    int   getMajour() { return nMajour;   }
    qreal getOffset() { return nOffsetScaled;   }
    qreal getLength() { return nLength;   }
    qreal getScale()  { return nScale;    }

    // These are mostly for responding to a request but can also be called
    // for other reasons. ie restoring guides during a load
    int doGuideNew( qreal nPointOnScene );
    void doGuideDelete( int nIndex );
    void doGuideMove( int nIndex, qreal nPointOnScene );

signals:
    // We let the 'caller' decide how to handle these. 
    // The 'caller' can use doGuideNew() etc as warranted. 
    void signalGuideNewRequest( Qt::Orientation n, qreal nOnDocument );
    void signalGuideDeleteRequest( Qt::Orientation n, int nIndex );
    void signalGuideMoveRequest( Qt::Orientation n, int nIndex, qreal nOnDocument );

public slots:
    void slotUpdate( qreal nOffset, qreal nScale, qreal nLength );
    void slotPos( qreal n );          
/*
    void slotOffset( qreal n );       
    void slotLength( qreal n );       
    void slotLength( const QRectF & );
    void slotScale( qreal n );        
*/

protected:
    // These are for markers. The idea is that moveCenter can be used to place them
    // to the marker position in question. Then they can be used;
    //      - to check if the mouse has been clicked in that location
    //      - for painting marker
    QRectF rectPos;
    QRectF rectGuide;

    // These are for putting the ticks on the ruler.
    qreal   nMinor;     // minor tick happens every nMinor pixels
    int     nMajour;    // majour tick happens every nMajour minor ticks

    // Offset
    // - delta between top/Left corner of document and top/left corner of viewport
    // - accounts for scrolling when document is larger than viewport (offset is negative)
    // - accounts for extra space in viewport when document smaller than viewport (offset is positive)
    // - used to determine location of rulers zero on the widget
    // - implicitly scaled (as document width/height will reflect any scaling)
    qreal nOffsetScaled;    

    // Length
    // - width or height of the document in the viewport
    // - ie from pGraphicsView->scene()->width()
    // - length of the drawn ruler 
    // - length may be longer or shorter than the widget - depending upon scaling and scrolling
    // - implicitly scaled (as document width/height will reflect any scaling)
    qreal nLength;
    
    // Scale
    // - set via QGraphicsView, QTransform
    // - IS the scale
    qreal nScale;    

    // We handle both cases, horizontal and vertical, in this class.
    Qt::Orientation nOrientation;

    // Pos
    // - current (mouse) coordinate on document
    // - NOT scaled
    qreal nPosOnDocument;       

    // Guides
    // - In this case - guides are just markers on the ruler. 
    // - These markers are typically created/deleted by double clicking on the ruler but can also be created/deleted on-demand.
    // - The index is important as it is used to keep these guides in sync with the app's implementation of the guides.
    // - qreal is the x (or y) coordinate. 
    // - qreal is in document coordinates (not transformed/scaled)
    // - nGuide is an index to the guide being moved (or -1)
    // - getGuide returned an index to the guide under the provided coordinate (or -1)
    QVector<qreal> vectorGuides;
    int nGuide;
    int getGuide( qreal nOnWidget );

    // Map Coordinates
    // - here we can map a coordinate every which way
    qreal mapWidgetToDocument( qreal n );
    qreal mapDocumentToLength( qreal n );
    qreal mapDocumentToWidget( qreal n );

    // Events
    void mouseDoubleClickEvent( QMouseEvent *pEvent );
    void mouseMoveEvent( QMouseEvent *pEvent );
    void mousePressEvent( QMouseEvent *pEvent );
    void mouseReleaseEvent( QMouseEvent *pEvent );
    void paintEvent( QPaintEvent * );

    // Paint Markers
    void paintPos( QPainter *pPainter, qreal nOnDocument );
    void paintGuide( QPainter *pPainter, qreal nOnDocument );
};

#endif 

