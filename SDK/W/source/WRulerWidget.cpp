/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "WRulerWidget.h"

WRulerCornerWidget::WRulerCornerWidget( QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

QSize WRulerCornerWidget::sizeHint() const
{
    return QSize( 20, 20 );
}

/*!
  \class WRulerWidget WRulerWidget.h
  \brief Simple ruler widget
  \ingroup basic

*/

/*!
  Constructs a WRulerWidget.
  \param nOrientation Ruler direction
  \param min minimum resolution between marks
  \param group number of marks in group
*/
WRulerWidget::WRulerWidget( QWidget *pwidgetParent, Qt::Orientation n, int nMajour )
    : QWidget( pwidgetParent )
{
    this->nMajour       = nMajour;
    this->nMinor        = nMajour / 2;

    nOrientation    = n;
    nPosOnDocument  = 0;
    nOffsetScaled   = 0;
    nLength         = 0;
    nScale          = 1;
    nGuide          = -1;

    rectPos.setWidth( 8 );
    rectPos.setHeight( 8 );
    rectGuide.setWidth( 8 );
    rectGuide.setHeight( 8 );

    if ( nOrientation == Qt::Horizontal )
    {
        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
        setMinimumHeight( 20 );
        setMaximumHeight( 20 );
        setMinimumWidth( 20 );
    }
    else
    {
        setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding );
        setMinimumWidth( 20 );
        setMaximumWidth( 20 );
        setMinimumHeight( 20 );
    }
}

WRulerWidget::~WRulerWidget()
{
}

/*!
    \brief sets minimum distance between marks
 */
void WRulerWidget::setMinor( qreal n )
{
    if ( (n != nMinor) & (n > 5) )
    {
        nMinor = n;
        repaint();
    }
}

void WRulerWidget::setMajour( int n )
{
    if ( n == nMajour) return;

    nMajour = n;
    repaint();
}

/*!
 * \brief Create a new guide marker. 
 *  
 * This will get appended to vectorGuides. The resulting index position is, presumably, 
 * being noted in the calling code. 
 *  
 * This call is most often made in response to the signalGuideNewRequest signal. 
 *   
 * \author pharvey (2/24/20)
 * 
 * \param nPointOnScene 
 * \return index of guide 
 */
int WRulerWidget::doGuideNew( qreal nPointOnScene )
{
    vectorGuides.append( nPointOnScene );
    repaint();

    return vectorGuides.count() - 1;
}

/*!
 * \brief Delete the guide at nIndex. 
 *  
 * This will change the index for all subsequent guides. This should not be a problem if the caller 
 * is also maintaining a vector and has done the same.
 * 
 * This call is most often made in response to the signalGuideDeleteRequest signal. 
 *   
 * \author pharvey (2/24/20)
 * 
 * \param nIndex 
 */
void WRulerWidget::doGuideDelete( int nIndex )
{
    Q_ASSERT( nIndex >= 0 );
    Q_ASSERT( nIndex < vectorGuides.count() );

    vectorGuides.remove( nIndex );
    repaint();
}

/*!
 * \brief Move the guide marker to nPointOnScene. 
 *  
 * This call is most often made in response to the signalGuideMoveRequest signal. 
 *   
 * \author pharvey (2/24/20)
 * 
 * \param nIndex 
 * \param nPointOnScene 
 */
void WRulerWidget::doGuideMove( int nIndex, qreal nPointOnScene )
{
    Q_ASSERT( nIndex >= 0 );
    Q_ASSERT( nIndex < vectorGuides.count() );

    vectorGuides[nIndex] = nPointOnScene;
    repaint();
}

void WRulerWidget::slotUpdate( qreal nOffset, qreal nScale, qreal nLength )
{
    this->nOffsetScaled = nOffset;
    this->nScale        = nScale;
    this->nLength       = nLength;
    repaint();
}

/*!
 * \brief Set the offset between the top/left of the scene and the top/left of the view (viewport). 
 *  
 * This arrives scaled for us. This is because we lack all of the information needed to calc ourself. 
 *  
 * This will be a negative value when the top/left of the scene is scrolled out of view. 
 *  
 * \author pharvey (2/22/20)
 * 
 * \param n 
 */
/*
void WRulerWidget::slotOffset( qreal n )                                         
{                                                                                
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
   if ( n == nOffsetScaled ) return;                                             
    nOffsetScaled = n;                                                           
    repaint();                                                                   
}                                                                                
                                                                                 
*/
/*!
 * \brief Set the length of the ruler. 
 *  
 * This is the length of the drawn part of the ruler - not the width/height of the widget. 
 * This will correspond with the width/height of the scene. 
 * This can be longer, shorter, or equal to the length of the widget. 
 * This is not scaled. 
 *  
 * \author pharvey (2/22/20)
 * 
 * \param n 
 */
/*
void WRulerWidget::slotLength( qreal n )
{                                       
    if ( n == nLength ) return;         
    nLength = n;                        
    repaint();                          
}                                       
*/

/*!
 * \brief Set length based upon a rect.
 *  
 * We do not really need the whole rect but this can be connected to some slots that 
 * would not otherwise be connectable. ie scene rect size change 
 *  
 * Here we just use the width() or the height() depending upon our orientation. 
 *  
 * \author pharvey (2/24/20)
 * 
 * \param r 
 */
/*
void WRulerWidget::slotLength( const QRectF &r )                                 
{                                                                                
qDebug() << "[PAH]" << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << " ";
   if ( nOrientation == Qt::Horizontal ) nLength = r.width();                    
    else nLength = r.height();                                                   
    repaint();                                                                   
}                                                                                
*/

/*!
 * \brief Set scale/zoom.
 *  
 * For example; 1.5 means 1.5x scale/zoom. 
 *  
 * \author pharvey (2/24/20)
 * 
 * \param n 
 */
/*
void WRulerWidget::slotScale( qreal n )
{                                      
    if ( n == nScale ) return;         
    nScale = n;                        
    repaint();                         
}                                      
*/

/*!
 * \brief Set current position. 
 *  
 * This is the position in the scene when dealing with QGraphicsView/QGraphicsScene. 
 * It is not scaled - even if the scene has a scale transform. 
 * 
 * \author pharvey (2/21/20)
 * 
 * \param n 
 */
void WRulerWidget::slotPos( qreal n )
{                                    
    if ( n < 0 ) return;             
    if ( n > nLength ) return;       
                                     
    nPosOnDocument = n;              
    repaint();                       
}                                    

/*!
 * \brief Returns the index to a guide at given coordinate. 
 * 
 * \author pharvey (2/24/20)
 * 
 * \param n
 * 
 * \return int index or -1
 */
int WRulerWidget::getGuide( qreal nOnWidget )
{
    if ( nOrientation == Qt::Horizontal )
        rectGuide.moveCenter( QPointF( nOnWidget, height() / 2 ) );
    else
        rectGuide.moveCenter( QPointF( width() / 2, nOnWidget ) );

    for ( int n = 0; n < vectorGuides.count(); n++ )
    {
        QPointF point;

        if ( nOrientation == Qt::Horizontal ) 
        {
            point.setX( mapDocumentToWidget( vectorGuides[n] ) );
            point.setY( rectGuide.center().y() );
        }
        else
        {
            point.setX( rectGuide.center().x() );
            point.setY( mapDocumentToWidget( vectorGuides[n] ) );
        }
        
        if ( rectGuide.contains( point ) ) return n;
    }

    return -1;
}

qreal WRulerWidget::mapWidgetToDocument( qreal n )
{
    return ( n - nOffsetScaled ) / nScale ;
}

qreal WRulerWidget::mapDocumentToWidget( qreal n )
{
    return n * nScale + nOffsetScaled;
}

qreal WRulerWidget::mapDocumentToLength( qreal n )
{
    return n * nScale + nOffsetScaled;
}

/*!
 * \brief Create/delete guide. 
 *  
 * This does not actually create/delete anything - it will just send a request signal. 
 *  
 * \author pharvey (2/22/20)
 * 
 * \param pEvent 
 */
void WRulerWidget::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    Qt::Orientation nLineOrientation = ( nOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal );

    // exists?
    int nIndex; 
    if ( nOrientation == Qt::Horizontal ) nIndex = getGuide( pEvent->pos().x() );
    else nIndex = getGuide( pEvent->pos().y() );

    if ( nIndex >= 0 )
    {
        // delete request
        emit signalGuideDeleteRequest( nLineOrientation, nIndex );
        return;
    }

    //new request
    qreal nCoordOnDocument;
    if ( nOrientation == Qt::Horizontal ) nCoordOnDocument = mapWidgetToDocument( pEvent->pos().x() );
    else nCoordOnDocument = mapWidgetToDocument( pEvent->pos().y() );
    emit signalGuideNewRequest( nLineOrientation, nCoordOnDocument );
}

/*!
 * \brief Send a move request signal (if a guide is being moved).
 * 
 * \author pharvey (2/24/20)
 * 
 * \param pEvent 
 */
void WRulerWidget::mouseMoveEvent( QMouseEvent *pEvent )
{
    // are we moving a guide?
    if ( nGuide < 0 ) return;

    Qt::Orientation nLineOrientation = (nOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);
    qreal nCoordOnDocument;

    if ( nOrientation == Qt::Horizontal ) nCoordOnDocument = mapWidgetToDocument( pEvent->pos().x() );
    else nCoordOnDocument = mapWidgetToDocument( pEvent->pos().y() );

    emit signalGuideMoveRequest( nLineOrientation, nGuide, nCoordOnDocument );
}

/*!
 * \brief Start moving a guide (or not).
 * 
 * \author pharvey (2/24/20)
 * 
 * \param pEvent 
 */
void WRulerWidget::mousePressEvent( QMouseEvent *pEvent )
{
    if ( nOrientation == Qt::Horizontal ) nGuide = getGuide( pEvent->pos().x() );
    else nGuide = getGuide( pEvent->pos().y() );
}

/*!
 * \brief Stop moving a guide (if we are moving a guide).
 *  
 * Could do a final move here but does not seem to be needed. 
 *  
 * \author pharvey (2/24/20)
 * 
 * \param pEvent 
 */
void WRulerWidget::mouseReleaseEvent( QMouseEvent *pEvent )
{
    Q_UNUSED(pEvent);

    if ( nGuide < 0 ) return;
    nGuide = -1;
    repaint();
}

/*!
 * \brief Paint the ruler. 
 *  
 * Paint the tick marks. 
 * Paint the position indicator. 
 * Paint any snap indicators. 
 *  
 * The ruler shadows the scene. This means that it scrolls with the scene and its length reflects the scene.
 *  
 * \note nOffsetScaled is given to the ruler as scaled. 
 *  
 * \author pharvey (2/21/20)
 */
void WRulerWidget::paintEvent( QPaintEvent * )
{
    // draw ruler
    qreal nLengthScaled = nLength * nScale;
    qreal nMinorScaled  = nMinor * nScale;
         
    QPainter painter( this );
    QRectF rectRuler;
    QFont font;
    font.setPointSize( 6 );
    painter.setFont( font );

    if ( nOrientation == Qt::Horizontal )
    {
        rectRuler = QRectF( nOffsetScaled, 0, nLengthScaled, height() - 1 );
        painter.drawRect( rectRuler );
        int nCount = 0;
        int nCountMajour = 1;
        for ( qreal nX = nOffsetScaled; nX < nLengthScaled + nOffsetScaled; nX += nMinorScaled )             
        {                                                                                   
            if ( nCount == nMajour ) 
            {
                painter.drawLine( nX, height(), nX, height() - 6 );
                painter.drawText( QPoint( nX - 6, height() - 7 ), QString::number( nCountMajour ) );
                nCountMajour++;
                nCount = 0;
            }
            else painter.drawLine( nX, height(), nX, height() - 3 );                        
            nCount++;
        }                                                                                   
    }
    else
    {
        rectRuler = QRectF( 0, nOffsetScaled, width() - 1, nLengthScaled );
        painter.drawRect( rectRuler );
        int nCount = 0;
        int nCountMajour = 1;
        for ( qreal nY = nOffsetScaled; nY < nLengthScaled + nOffsetScaled; nY += nMinorScaled )
        {
            if ( nCount == nMajour )
            {
                painter.drawLine( width() - 6, nY, width(), nY );
                painter.drawText( QPoint( 1, nY ), QString::number( nCountMajour ) );
                nCountMajour++;
                nCount = 0;
            }
            else 
                painter.drawLine( width() - 3, nY, width(), nY );
            nCount++;
        }
    }

    // guides
    qreal nOnDocument; // coord of guide
    foreach( nOnDocument, vectorGuides )
    {
        paintGuide( &painter, nOnDocument );
    }

    // pos
    paintPos( &painter, nPosOnDocument );
}

/*!
 * \brief Draw a current position marker on the ruler that cooresponds to nOnDocument.
 * 
 * \author pharvey (2/24/20)
 * 
 * \param pPainter 
 * \param nOnDocument 
 */
void WRulerWidget::paintPos( QPainter *pPainter, qreal nOnDocument )
{
    pPainter->setBrush( Qt::SolidPattern );
    pPainter->setBrush( Qt::red );
    pPainter->setPen( Qt::black );

    if ( nOrientation == Qt::Horizontal )
    {
        rectPos.moveCenter( QPointF( mapDocumentToLength( nOnDocument ), height() / 2 ) );
        QPolygonF polygon;                                          
        polygon << rectPos.topLeft();                               
        polygon << rectPos.topRight();                              
        polygon << QPointF( rectPos.left() + rectPos.width() / 2, rectPos.bottom() );
        pPainter->drawPolygon( polygon );                           
    }
    else
    {
        rectPos.moveCenter( QPointF( width() / 2, mapDocumentToLength( nOnDocument ) ) );
        QPolygonF polygon;                                  
        polygon << rectPos.bottomLeft();                    
        polygon << rectPos.topLeft();                       
        polygon << QPointF( rectPos.right(), rectPos.top() + rectPos.height() / 2 );
        pPainter->drawPolygon( polygon );                   
    }
}

/*!
 * \brief Draw a guide marker on the ruler that cooresponds to nOnDocument.
 * 
 * \author pharvey (2/24/20)
 * 
 * \param pPainter 
 * \param nOnDocument 
 */
void WRulerWidget::paintGuide( QPainter *pPainter, qreal nOnDocument )
{
    pPainter->setBrush( Qt::SolidPattern );
    pPainter->setBrush( Qt::green );
    pPainter->setPen( Qt::black );

    if ( nOrientation == Qt::Horizontal )
    {
        rectGuide.moveCenter( QPointF( mapDocumentToLength( nOnDocument ), height() / 2 ) );
        QPolygonF polygon;                                          
        polygon << rectGuide.topLeft();                               
        polygon << rectGuide.topRight();                              
        polygon << QPointF( rectGuide.left() + rectGuide.width() / 2, rectGuide.bottom() );
        pPainter->drawPolygon( polygon );                           
    }
    else
    {
        rectGuide.moveCenter( QPointF( width() / 2, mapDocumentToLength( nOnDocument ) ) );
        QPolygonF polygon;                                  
        polygon << rectGuide.bottomLeft();                    
        polygon << rectGuide.topLeft();                       
        polygon << QPointF( rectGuide.right(), rectGuide.top() + rectGuide.height() / 2 );
        pPainter->drawPolygon( polygon );                   
    }

//    pPainter->drawRect( rectGuide );
}


