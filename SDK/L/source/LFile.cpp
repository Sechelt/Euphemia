/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LFile.h"

#include <SSettings.h>

#include "LGraphicsLayer.h"
#include "LLayer.h"
#include "LEditorWidget.h"

LFile::LFile( ADObject *pParent, const QString &stringName )
   : AWFile( pParent, stringName )
{
    pSettingGeneral     = g_SSettings->getSetting( "LSGeneral" ); Q_ASSERT( pSettingGeneral );
    pSettingBackground  = g_SSettings->getSetting( "LSBackground" ); Q_ASSERT( pSettingBackground );
    pSettingPage        = g_SSettings->getSetting( "LSPage" ); Q_ASSERT( pSettingPage );
    pSettingGrid        = g_SSettings->getSetting( "LSGrid" ); Q_ASSERT( pSettingGrid );
    pSettingPointer     = g_SSettings->getSetting( "LSPointer" ); Q_ASSERT( pSettingPointer );

    slotRefreshGeneral();
    slotRefreshBackground();
    slotRefreshPage();
    slotRefreshGrid();
    slotRefreshPointer();

    connect( pSettingGeneral, SIGNAL(signalChanged()), SLOT(slotRefreshGeneral()) );
    connect( pSettingBackground, SIGNAL(signalChanged()), SLOT(slotRefreshBackground()) );
    connect( pSettingPage, SIGNAL(signalChanged()), SLOT(slotRefreshPage()) );
    connect( pSettingGrid, SIGNAL(signalChanged()), SLOT(slotRefreshGrid()) );
    connect( pSettingPointer, SIGNAL(signalChanged()), SLOT(slotRefreshPointer()) );
}

LFile::~LFile()
{
    // this will happen anyway if we are the parent but...
    if ( pGraphics ) doGraphicsDelete();
}

// actually this MUST be done before adding a layer
void LFile::doGraphicsCreate( QObject *pParent )
{
    Q_ASSERT( !pGraphics );

    pGraphics = new LGraphicsScene( this, rect, pParent );

    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LLayer *pLayer, l )
    {
        pLayer->doGraphicsCreate( pGraphics );
    }
}

void LFile::doGraphicsDelete()
{
    Q_ASSERT( pGraphics );

    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );
    foreach( LLayer *pLayer, l )
    {
        pLayer->doGraphicsDelete();
    }
    delete pGraphics;
    pGraphics = nullptr;
}

void LFile::doMousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doMousePressEvent( pEvent );
}

void LFile::doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
    // update ruler
    if ( hasRuler() && getEditorWidget() )
    { 
        LEditorWidget *p = (LEditorWidget*)getEditorWidget();
        p->getRuler( Qt::Horizontal )->slotPos( pEvent->scenePos().x() );
        p->getRuler( Qt::Vertical )->slotPos( pEvent->scenePos().y() );
    }

    // update cross hairs
    if ( hasCrossHairs() && getEditorWidget() )
    {
        QPointF p1;
        QPointF p2;
        qreal   nWidth    = getGraphics()->width();
        qreal   nHeight   = getGraphics()->height();

        // horizontal line
        if ( pEvent->scenePos().y() >= 0 && pEvent->scenePos().y() < nHeight )
        {
            QPointF point = pCrossH->mapFromScene( pEvent->scenePos() );
            QLineF line( pCrossH->mapFromScene( QPointF( 0, pEvent->scenePos().y() ) ), pCrossH->mapFromScene( QPointF( nWidth, pEvent->scenePos().y() ) ) );
            pCrossH->setLine( point, line ); 
        }

        // vertical line
        if ( pEvent->scenePos().x() >= 0 && pEvent->scenePos().x() < nWidth )
        {
            QPointF point = pCrossH->mapFromScene( pEvent->scenePos() );
            QLineF line( pCrossV->mapFromScene( QPointF( pEvent->scenePos().x(), 0 ) ), pCrossV->mapFromScene( QPointF( pEvent->scenePos().x(), nHeight ) ) );
            pCrossV->setLine( point, line ); 
        }
    }

    // 
    if ( !pLayer ) return;
    pLayer->doMouseMoveEvent( pEvent );
}

void LFile::doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doMouseReleaseEvent( pEvent );
}

void LFile::doDragEnterEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doDragEnterEvent( pEvent );
}

void LFile::doDragMoveEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doDragMoveEvent( pEvent );
}

void LFile::doDropEvent( QGraphicsSceneDragDropEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doDropEvent( pEvent );
}

void LFile::doContextMenuEvent( QGraphicsSceneContextMenuEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doContextMenuEvent( pEvent );
}

void LFile::doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    if ( !pLayer ) return;
    pLayer->doMouseDoubleClickEvent( pEvent );
}

void LFile::doDrawBackground( QPainter *pPainter, const QRectF & )
{
    // \ToDo add support for other LSPage settings

    // Turn any scale off (to avoid fat lines)...
    // get current transform so we can muck with the scale
    QTransform t = pPainter->transform();
    // X/Y scale
    qreal m11 = t.m11(), m22 = t.m22();
    // save painter state so we can restore later
    pPainter->save();
    // undo scale for our draw to prevent grid lines from getting fat
    pPainter->setTransform( QTransform( 1, t.m12(), t.m13(), t.m21(), 1, t.m23(), t.m31(), t.m32(), t.m33() ) );
    // fill entire scene
    pPainter->setBrush( brushTransparency ); 
    pPainter->fillRect( 0, 0, rect.width() * m11, rect.height() * m22, brushTransparency ); 
    // draw pages 
    if ( bDrawPage ) 
    {
        pPainter->setPen( Qt::black );
        pPainter->setBrush( QBrush( Qt::NoBrush ) );
        for ( int nPageX = 0; nPageX < sizePages.width(); nPageX++ )
        {
            for ( int nPageY = 0; nPageY < sizePages.height(); nPageY++ )
            {
                // draw page
                qreal nX = nPageX * sizePage.width();
                qreal nY = nPageY * sizePage.height();
                pPainter->drawRect( nX * m11, nY * m22, sizePage.width() * m11, sizePage.height() * m22 );
                // draw margins
                if ( bDrawMargins )
                {
                    nX += sizeMargins.width();
                    nY += sizeMargins.height();
                    pPainter->drawRect( nX * m11, nY * m22, (sizePage.width() - sizeMargins.width() * 2) * m11, (sizePage.height() - sizeMargins.height() * 2) * m22 );
                }
            }
        }
    }

    // Turn any scale back on...
    pPainter->restore(); 
}

void LFile::doDrawForeground( QPainter *pPainter, const QRectF & )
{
    // Draw Grid
    // Something to note here is that we always want to draw without
    // a scale to avoid drawing fat lines. We want our lines to be 1 pixel
    // even when the scene is scaled.
    // To do this we temp disable any scale in painter and apply any scale,
    // manually, to our coordinates only.

    // draw grid?
    if ( !bGrid ) return;
    if ( nGridType == L::GridTypeNone ) return;
    // get current transform so we can muck with the scale
    QTransform t = pPainter->transform();
    // X/Y scale
    qreal m11 = t.m11(), m22 = t.m22();
    // save painter state so we can restore later
    pPainter->save();
    // undo scale for our draw to prevent grid lines from getting fat
    pPainter->setTransform( QTransform( 1, t.m12(), t.m13(), t.m21(), 1, t.m23(), t.m31(), t.m32(), t.m33() ) );
    // get cell size
    qreal nCellWidth;
    qreal nCellHeight;
    if ( nGridType == L::GridTypePixel )
    {
        // in this case the cell is always 1 because we are representing a single pixel
        nCellWidth = 1; // m11;
        nCellHeight = 1; // m22;
        // does not make sense to show this unless we are scaled (zoomed in)
        if ( m11 < 8 || m22 < 8 ) { pPainter->restore(); return; }                       // hard threshold
        if ( m11 < nGridSizeMin || m22 < nGridSizeMin ) { pPainter->restore(); return; } // user defined threshold can require more scaling
    }
    else
    {
        // in this case the cell size is user specified and is scaled with scene
        nCellWidth  = sizeGridCell.width() * m11;
        nCellHeight = sizeGridCell.height() * m22;
        if ( nCellWidth < 8 || nCellHeight < 8 ) { pPainter->restore(); return; }                       // hard threshold
        if ( nCellWidth < nGridSizeMin || nCellHeight < nGridSizeMin ) { pPainter->restore(); return; }  // user defined threshold can require more scaling
    }

    int nWidth = pGraphics->sceneRect().width();
    int nHeight = pGraphics->sceneRect().height();

    // Draw twice to ensure line will be visible regrdaless of color behind it; 
    // 1 - darkGray base
    // 2 - white dotted
    QPen pen( Qt::darkGray );
    pPainter->setPen( pen );

    for ( int nX = 0; nX <= nWidth; nX += nCellWidth )
    {
        pPainter->drawLine( QPointF( nX*m11, 0 ), QPointF( nX*m11, nHeight*m22 ) );
    }
    for ( int nY = 0; nY <= nHeight; nY += nCellHeight )
    {
        pPainter->drawLine( QPointF( 0, nY*m22 ), QPointF( nWidth*m11, nY*m22 ) );
    }

    pen.setColor( Qt::white );
    pen.setStyle( Qt::DotLine );
    pPainter->setPen( pen );

    for ( int nX = 0; nX <= nWidth; nX += nCellWidth )
    {
        pPainter->drawLine( QPointF( nX*m11, 0 ), QPointF( nX*m11, nHeight*m22 ) );
    }
    for ( int nY = 0; nY <= nHeight; nY += nCellHeight )
    {
        pPainter->drawLine( QPointF( 0, nY*m22 ), QPointF( nWidth*m11, nY*m22 ) );
    }

    // restore transform
    pPainter->restore(); 
}

bool LFile::setLayerCurrentIndex( int nIndex )
{
    if ( nCurrent == nIndex ) return true; // needed to avoid looping                                                                                                              
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    if ( nIndex < 0 || nIndex >= l.count() ) return false; // out of range?                                                                             
                                                                                                                                         
    if ( pLayer )                                                                                                             
    {                                                                                                                                    
        if ( pLayer->isDrawing() ) pLayer->doCancel();                                                                                                   
        if ( pLayer->hasSelection() ) pLayer->doSelectNone();                                                                                            
    }                                                                                                                                    
                                                                                                                                         
    nCurrent    = nIndex;                                                                                                                
    pLayer      = l.at( nCurrent );                                                                                                      
               
    emit signalChangedCurrent();                                                                                                                          

    return true;
}

int LFile::getLayerCurrentIndex()
{
    return nCurrent;
}

LLayer *LFile::getLayerCurrent()
{
    return pLayer;
}

int LFile::getLayerCount()         
{ 
    return findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly ).count();
}

QMenu *LFile::getLayerContextMenu( QWidget *pParent )
{
    if ( pLayer ) return pLayer->getContextMenu( pParent );
    return nullptr;
}

/*!
 * \brief Create a new layer. 
 *  
 * This should be used instead of calling getObject( class-name ); 
 * 
 * \author pharvey (4/6/23)
 * 
 * \param stringClass Must be a name of a LLayer based class.
 * 
 * \return bool 
 */
bool LFile::doLayerNew( const QString &stringClass )
{
    if ( !stringFileName.isEmpty() ) 
    {
        QFileInfo f( stringFileName );
        if ( f.suffix() != "blue" ) 
        {
            QMessageBox::StandardButton nButton;                                                                                                                                                                                              
            nButton = QMessageBox::question( getEditorWidgetParent(), 
                                             getDialogCaption(), 
                                             tr("File format does not support layers. If you proceed the file name will be cleared.\nDo you want to proceed?"),  
                                             QMessageBox::Ok | QMessageBox::Cancel );                                                                            
            if ( nButton == QMessageBox::Cancel ) return false;  
            setFileName( QString() );
        }
    }

    // cancel any tool that may be at play
    if ( pLayer && pLayer->isDrawing() ) pLayer->doCancel();  
                                                         
    // get layer object (it is appended to children list)
    ADObject *p = getObject( stringClass );
    if ( !p ) return false;
    Q_ASSERT( p->inherits( "LLayer" ) );

    if ( pGraphics ) 
    {
        ((LLayer*)p)->doGraphicsCreate( pGraphics );
        doSyncGraphicsOrder();
    }

    // make it the current layer                         
    setLayerCurrentIndex( getLayerCount() - 1 );              
 
    emit signalChangedCount();                           

    return true;
}

QString LFile::getLayerClass( int nIndex )
{
    LLayer *pLayer = getLayer( nIndex );
    if ( !pLayer ) return QString();

    return pLayer->metaObject()->className();
}

bool LFile::doLayerDelete( int nIndex )
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   

    // less than 0 (default) means current                                                                                                               
    if ( nIndex < 0 ) nIndex = nCurrent;                                                                                                                 
    // out of range?                                                                                                                                     
    if ( nIndex >= l.count() ) return false;                                                                                                  
    // disconnect from current                                                                                                                           
    if ( pLayer )                                                                                                                             
    {                                                                                                                                                    
        if ( pLayer->isDrawing() ) pLayer->doCancel();                                                                                                                   
    }                                                                                                                                                    
    // delete layer                                                                                                                                      
    slotDelete( pLayer );
    pLayer = nullptr;
    nCurrent = -1;
    l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    if ( l.count() )
    {
        // first
        nCurrent = 0;
        pLayer = l.at( nCurrent );
    }

    if ( pGraphics ) doSyncGraphicsOrder();

    // let those that care - know we changed stuff                                                                                                       
    emit signalChangedCurrent();                                                                                                                          
    emit signalChangedCount();
    setModified();

    return true;
}

bool LFile::doLayerToPrev()
{
    if ( nCurrent < 1 ) return false; // at back - no where to go

    if ( pLayer ) 
    {
        pLayer->doToPrev( QString(), QString(), QString( "LLayer" ) );           
    }
    if ( pGraphics ) 
    {
        doSyncGraphicsOrder();
    }
    nCurrent--;

    emit signalChangedCurrent(); // we actually just changed nCurrent - pLayer is same
    emit signalChangedOrder();
    return true;
}

bool LFile::doLayerToNext()
{
    if ( nCurrent < 0 ) return false; // no current
    if ( nCurrent == findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly ).count() - 1 ) return false; // at front - no where to go                                                  

    if ( pLayer )
    {
        pLayer->doToNext( QString(), QString(), QString( "LLayer" ) );
    }
    if ( pGraphics ) doSyncGraphicsOrder();
    nCurrent++; 

    emit signalChangedCurrent(); // we actually just changed nCurrent - pLayer is same
    emit signalChangedOrder();
    return true;
}

bool LFile::hasLayer( LLayer *p )          
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    return l.contains( p );
}

bool LFile::hasLayerCurrent()          
{
    // sanity check
    Q_ASSERT( nCurrent >= 0 && pLayer ); 
    Q_ASSERT( nCurrent < 0 && !pLayer ); 
    //
    return pLayer;
}

bool LFile::isLayerVisible( int n )
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   

    if ( n < 0 ) n = nCurrent;
    if ( n < 0 ) return false;
    if ( n >= l.count() ) return false;
                           
    return l.at( n )->isVisible();
}

void LFile::doFindLoaded( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria )
{
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
    Q_UNUSED(Criteria);
}

void LFile::doValidateLoaded( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems )
{
    Q_UNUSED(nStrictness);
    Q_UNUSED(nValueType);
    Q_UNUSED(listObjectPathItems);
}

void LFile::setImage( const QImage &image )
{
    if ( pLayer ) 
    {
        pLayer->setImage( image );
        doResize( image.size() );
    }
}

void LFile::doCrop()
{
    if ( !pLayer ) return;

    pLayer->doCrop();
    doResize( pLayer->getImageRef()->width(), pLayer->getImageRef()->height() );
}

void LFile::doResize( qreal nX, qreal nY )
{
    // sanity check
    if ( nX < 1 ) return;
    if ( nY < 1 ) return;

    rect.setWidth( nX );
    rect.setHeight( nY );

    // layers will catch sceneRectChanged(const QRectF &rect) from scene as needed

    // scene
    if ( pGraphics ) pGraphics->setSceneRect( 0, 0, nX, nY );
}

void LFile::doResize( const QSizeF &size )
{
    doResize( size.width(), size.height() );
}

void LFile::doScale( qreal nX, qreal nY, Qt::AspectRatioMode n )
{
    // pass to current layer so that image is scaled...
    if ( pLayer ) pLayer->doScale( nX, nY, n );

    // sanity check
    if ( nX < 1 ) return;
    if ( nY < 1 ) return;

    rect.setWidth( nX );
    rect.setHeight( nY );

    // layers will catch sceneRectChanged(const QRectF &rect) from scene as needed

    // scene
    if ( pGraphics ) pGraphics->setSceneRect( 0, 0, nX, nY );
}

void LFile::doClear()
{
    //
    if ( hasCrossHairs() ) doDeleteCrossHairs();
    // get rid of children
    AWFile::doClear();
    //
    rect        = QRectF( 0.0, 0.0, 1000.0, 1000.0 );   /*!< the size we want for the scene         */
    nCurrent    = -1;                                   /*!< current layer -1 means none            */
    pLayer      = nullptr;                              /*!< current layer nullptr means none       */
    pGraphics   = nullptr;                              /*!< current scene nullptr means none       */
    pCrossH     = nullptr;
    pCrossV     = nullptr;
}

bool LFile::slotOpen()                                                                                                                                                                                                                   
{
    // get us to a clear state
    // we ASSUME we are clear
    // if ( slotClose() != AWObject::SaveSuccess ) return false;                                                                                                                                                                             

    // get file name                                                                                                                                                                                                                      
    QString stringFileName = QFileDialog::getOpenFileName( 0, QString(), QString(), getFileSpec() );                                                                                                                                      
    if ( stringFileName.isEmpty() )                                                                                                                                                                                                       
        return false;                                                                                                                                                                                                                     
                                                                                                                                                                                                                                          
    // read it in                                                                                                                                                                                                                               
    if ( !doRead( stringFileName ) ) return false;                                                                                                                                                                                        
    setFileName( stringFileName );                                                                                                                                                                                                    
    setModified( false );                                                                                                                                                                                                                 

    return true;
}

void LFile::slotPos( const QPointF &pointPixel )
{
   emit signalPos( QPoint( pointPixel.x() / sizeGridCell.width(), pointPixel.y() / sizeGridCell.height() ) );
   emit signalPos( pointPixel );
}

void LFile::slotZoomChanged( int nZoom )
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    foreach( LLayer *p, l )
    {
        p->doZoomChanged( nZoom );
    }
}

void LFile::slotScrollChanged( int nX, int nY )
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    foreach( LLayer *p, l )
    {
        p->doScrollChanged( nX, nY );
    }
}

void LFile::slotGuideNewRequest( Qt::Orientation nLineOrientation, qreal nOnScene )
{
    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    LEditorWidget *p = (LEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // restrict to width and height
    if ( nOnScene < 0 ) return;

    // add a guide line
    LGraphicsLine *pItem;
    if ( nLineOrientation == Qt::Vertical )
    {
        if ( nOnScene > getGraphics()->width() ) return;
        pRuler->doGuideNew( nOnScene );
        pItem = new LGraphicsLine( QLineF( nOnScene, 0, nOnScene, getGraphics()->height() ) );
        vectorGuidesX.append( pItem );
    }
    else
    {
        if ( nOnScene > getGraphics()->height() ) return;
        pRuler->doGuideNew( nOnScene );
        pItem = new LGraphicsLine( QLineF( 0, nOnScene, getGraphics()->width(), nOnScene ) );
        vectorGuidesY.append( pItem );
    }
    
    QPen pen( Qt::SolidLine );
    pen.setColor( Qt::lightGray );

    pItem->setPen( pen );
    getGraphics()->addItem( pItem );
}

void LFile::slotGuideDeleteRequest( Qt::Orientation nLineOrientation, int nIndex )
{
    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    LEditorWidget *p = (LEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // accept all requests

    // get ruler to delete a guide marker
    pRuler->doGuideDelete( nIndex );

    // delete guide line
    if ( nLineOrientation == Qt::Vertical )
    {
        delete vectorGuidesX[nIndex];
        vectorGuidesX.remove( nIndex );
    }
    else
    {
        delete vectorGuidesY[nIndex];
        vectorGuidesY.remove( nIndex );
    }

    // guide could be only thing holding a page - in such a case - we want to get rid of the page(s)
    // when the guide is deleted
    // doEnsurePages();
}

void LFile::slotGuideMoveRequest( Qt::Orientation nLineOrientation, int nIndex, qreal nOnScene )
{
    Q_ASSERT( nIndex >= 0 );

    Qt::Orientation nRulerOrientation = (nLineOrientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal);

    // this call is only valid if it comes from a WRulerWidget
    if ( !pEditorWidget ) return;
    LEditorWidget *p = (LEditorWidget*)pEditorWidget;
    WRulerWidget *pRuler = p->getRuler( nRulerOrientation );
    if ( !pRuler ) return;

    // restrict to width and height
    if ( nOnScene < 0 ) return;

    // move guide line
    if ( nLineOrientation == Qt::Vertical )
    {
        if ( nOnScene > getGraphics()->width() ) return;
        Q_ASSERT( nIndex < vectorGuidesX.count() );
        pRuler->doGuideMove( nIndex, nOnScene );
        vectorGuidesX[nIndex]->setLine( QLineF( nOnScene, 0, nOnScene, getGraphics()->height() ) );
    }
    else
    {
        if ( nOnScene > getGraphics()->height() ) return;
        Q_ASSERT( nIndex < vectorGuidesY.count() );
        pRuler->doGuideMove( nIndex, nOnScene );
        vectorGuidesY[nIndex]->setLine( QLineF( 0, nOnScene, getGraphics()->width(), nOnScene ) );
    }
}

/*
QDomElement LFile::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )   
{                                                                                    
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );           
    doSaveADDocument( pdomDoc, &domElemThis );                                       

    domElemThis.setAttribute( "PageBreaks", bPageBreaks );
    domElemThis.setAttribute( "Grid", bGrid );
    domElemThis.setAttribute( "CellWidth", sizeCell.width() );
    domElemThis.setAttribute( "CellHeight", sizeCell.height() );
    domElemThis.setAttribute( "Ruler", bRuler );
    domElemThis.setAttribute( "CrossHairs", bCrossHairs );
    domElemThis.setAttribute( "Strictness", ADValidation::getStrictness( nStrictness ) );
    domElemThis.setAttribute( "PageSizePrinter", bPagePrinter );
    domElemThis.setAttribute( "PageSizeWidth", sizePage.width() );
    domElemThis.setAttribute( "PageSizeHeight", sizePage.height() );
    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmapBackground.toImage(), &nBytesCompressed );
    domElemThis.setAttribute( "BitmapSize", QString::number( nBytesCompressed ) );
    domElemThis.setAttribute( "Bitmap", stringEncodedImage );
    domElemThis.setAttribute( "BitmapPlacement", int(nBitmapBackgroundPlacement) );
    domElemThis.setAttribute( "BitmapUse", bPixmapBackgroundUse );

    domElemThis.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc ) );
    domElemThis.appendChild( CBDPersistNative::doSaveBrush( brush, pdomDoc ) );

    // save guides
    {
        // X
        QString s;
        DGraphicsItemLine *pItem;
        foreach( pItem, vectorGuidesX )
        {
            if ( !s.isEmpty() ) s += ":";
            s += QString::number( pItem->getLine().x1() );
        }
        if ( !s.isEmpty() ) domElemThis.setAttribute( "GuidesX", s );

        // Y
        s = QString();
        foreach( pItem, vectorGuidesY )
        {
            if ( !s.isEmpty() ) s += ":";
            s += QString::number( pItem->getLine().y1() );
        }
        if ( !s.isEmpty() ) domElemThis.setAttribute( "GuidesY", s );
    }

    domElemThis.appendChild( doSaveValidation( pdomDoc ) );

    return domElemThis;                                                              
}                                                                                    
                                                                                     
bool LFile::doLoad( QDomElement *pdomElemThis )                                   
{                                                                                    
    AWObject::doLoad( pdomElemThis );                                                
   doLoadADDocument( pdomElemThis );                                                
                                                                                     
   bPageBreaks = pdomElemThis->attribute( "PageBreaks", "1" ).toInt();
   sizeCell.setWidth( pdomElemThis->attribute( "CellWidth", "48" ).toInt() );                                                                                     
   sizeCell.setHeight( pdomElemThis->attribute( "CellHeight", "48" ).toInt() );                                                                                     
   bGrid = pdomElemThis->attribute( "Grid", "1" ).toInt();
   bRuler = pdomElemThis->attribute( "Ruler", "1" ).toInt();
   slotCrossHairs( pdomElemThis->attribute( "CrossHairs", "1" ).toInt() );
   nStrictness = ADValidation::getStrictness( pdomElemThis->attribute( "Strictness", "" ) );
   bPagePrinter = pdomElemThis->attribute( "PageSizePrinter", "1" ).toInt();                                                                                     
   sizePage.setWidth( pdomElemThis->attribute( "PageSizeWidth", "500" ).toInt() );                                                                                     
   sizePage.setHeight( pdomElemThis->attribute( "PageSizeHeight", "500" ).toInt() );                                                                                     
   bPixmapBackgroundUse = pdomElemThis->attribute( "BitmapUse", "0" ).toInt(); 
   nBitmapBackgroundPlacement = (W::WImagePlacements)pdomElemThis->attribute( "BitmapPlacement", QString::number( W::WImagePlacementScaled ) ).toInt();
   ulong nBytesCompressed = pdomElemThis->attribute( "BitmapSize", "0" ).toULong();
   if ( nBytesCompressed )
   { 
       QString stringAttribute = pdomElemThis->attribute( "Bitmap" );
       if ( stringAttribute.isNull() ) pixmapBackground = QPixmap();
       else pixmapBackground = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
   }

   // we may have changed page size so...
   setSceneRect();

   // load guides
   {
       QString s1, s2;
       QStringList l;
       // X
       s1 = pdomElemThis->attribute( "GuidesX" );
       if ( !s1.isEmpty() )
       {
           l = s1.split( ':' );
           foreach( s2, l )
           {
               doLoadGuide( Qt::Vertical, s2.toDouble() ); 
           }
       }
       // Y
       s1 = pdomElemThis->attribute( "GuidesY" );
       if ( !s1.isEmpty() )
       {
           l = s1.split( ':' );
           foreach( s2, l )
           {
               doLoadGuide( Qt::Horizontal, s2.toDouble() ); 
           }
       }
   }

   // load additional nodes...
   QDomElement         domElem;
   QDomNode            domNode;
   domNode = pdomElemThis->firstChild();
   while( !domNode.isNull() ) 
   {
       domElem = domNode.toElement();
       if( domElem.isNull() ) 
       {
           domNode = domNode.nextSibling();
           continue;
       }

       if ( domElem.tagName() == "Validation" ) doLoadValidation( &domElem );
       else if ( domElem.tagName() == "Pen" ) pen = CBDPersistNative::doLoadPen( &domElem );
       else if ( domElem.tagName() == "Brush" ) brush = CBDPersistNative::doLoadBrush( &domElem );

       domNode = domNode.nextSibling();
   }

   return true;                                                                     
}                                                                                    
                                                                                     
*/

void LFile::doPostLoad()
{
    // do the usual
    ADObject::doPostLoad();

    if ( pGraphics )
    {
        QList<LLayer *> l = findChildren<LLayer *>( QString(), Qt::FindDirectChildrenOnly );
        foreach( LLayer *p, l )
        {
            ((LLayer*)p)->doGraphicsCreate( pGraphics );
        }
    }

    if ( pGraphics ) doSyncGraphicsOrder();
    setLayerCurrentIndex( getLayerCount() - 1 );              
    emit signalChangedCount();                           
}

bool LFile::hasRuler()
{
    LEditorWidget *p = (LEditorWidget*)getEditorWidget();
    if ( !p ) return false;
    return p->hasRuler();
}

void LFile::slotRefreshGeneral()
{
    bDrawPage           = pSettingGeneral->property( "bPage" ).toBool();
    bGrid               = pSettingGeneral->property( "bGrid" ).toBool();

    if ( pGraphics ) pGraphics->update();
}

void LFile::slotRefreshBackground()
{
    brushTransparency   = pSettingBackground->property( "brushTransparency" ).value<QBrush>();
    if ( pGraphics ) pGraphics->update();
}

void LFile::slotRefreshPage()
{
    sizePage    = pSettingPage->property( "sizeDefault" ).toSizeF();
    sizePages   = pSettingPage->property( "sizePages" ).toSize();
    sizeMargins = pSettingPage->property( "sizeMargins" ).toSize();
    bDrawMargins= pSettingPage->property( "bDrawMargins" ).toBool();
    rect.setWidth( sizePage.width() * sizePages.width() ); 
    rect.setHeight( sizePage.height() * sizePages.height() ); 
    if ( pGraphics ) pGraphics->setSceneRect( rect );
}

void LFile::slotRefreshGrid()
{
    nGridType    = (L::GridTypes)pSettingGrid->property( "nType" ).toInt();
    sizeGridCell = pSettingGrid->property( "sizeCell" ).toSize();
    nGridSizeMin = pSettingGrid->property( "nSizeMin" ).toInt();
    if ( bGrid && pGraphics ) pGraphics->update();
}

void LFile::slotRefreshPointer()
{
    if ( pGraphics ) 
    {
        bool b;
        b = pSettingPointer->property( "bCrossHairs" ).toBool();
        if ( b != bCrossHairs )
        {
            bCrossHairs = b;
            if ( bCrossHairs && !hasCrossHairs() ) doCreateCrossHairs();
            if ( !bCrossHairs && hasCrossHairs() ) doDeleteCrossHairs();
        }
        pGraphics->update();
    }
    else
    {
        bCrossHairs = pSettingPointer->property( "bCrossHairs" ).toBool();
    }
}

LLayer *LFile::getLayer( int nIndex )
{
    // less than 0 (default) means current 
    if ( nIndex < 0 ) nIndex = nCurrent;
    // index too low
    if ( nIndex < 0 ) return nullptr;
    // return current
    if ( nIndex == nCurrent && pLayer ) return pLayer;
    // search children which are LLayer
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );
    // index too high
    if ( nIndex >= l.count() ) return nullptr;
    // found it
    return l.at( nIndex );
}

ADObject *LFile::getObject( const QString &stringClass, ADObject *pObjectParent )
{
    Q_UNUSED( stringClass );
    Q_UNUSED( pObjectParent );
    /* DERIVED CLASSES WILL WANT TO CREATE LAYERS HERE THAT REFLECT mapMeta */
    return nullptr;
}

void LFile::doCreateCrossHairs()
{
    Q_ASSERT( !pCrossH );

    pCrossH = new LCrossHairLine( Qt::Horizontal ); 
    pCrossV = new LCrossHairLine( Qt::Vertical );
    pGraphics->addItem( pCrossH );
    pGraphics->addItem( pCrossV );

    if ( pGraphics->views().count() < 1 ) return;
    QGraphicsView *pView = pGraphics->views().at( 0 );
    pView->setCursor( Qt::CrossCursor );
}

void LFile::doDeleteCrossHairs()
{
    Q_ASSERT( pCrossH );

    delete pCrossH; pCrossH = nullptr;
    delete pCrossV; pCrossV = nullptr;

    if ( pGraphics->views().count() < 1 ) return;
    QGraphicsView *pView = pGraphics->views().at( 0 );
    pView->unsetCursor();
}

void LFile::doSyncGraphicsOrder()
{
    QList<LLayer*> l = findChildren<LLayer*>( QString(), Qt::FindDirectChildrenOnly );                                                   
    int n = 0;
    foreach( LLayer *p, l )
    {
        if ( p->getGraphics() ) p->getGraphics()->setZValue( n );
        n++;
    }
}


