/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PAPaint.h"

#include <CBDPersist.h>
#include <SSettings.h>
#include <LGraphicsLayer.h>
#include <LGraphicsScene.h>

PAPaint::PAPaint( LFile *pParent, const QString &stringName )
    : LLayer( pParent, stringName )
{
    Q_ASSERT( pParent );

    pSettingImage = g_SSettings->getSetting( "LSImage" ); Q_ASSERT( pSettingImage );
    slotRefreshImage();
    connect( pSettingImage, SIGNAL(signalChanged()), SLOT(slotRefreshImage()) );

    image.convertTo( QImage::Format_ARGB32 );
}

void PAPaint::setImage( const QImage &i )
{
    // sanity check
    if ( isDrawing() ) doCancel();
    image = i;
    // caller (presumably scene) needs to set an appropriate scene rect - we do not do it here
    setModified();
}

void PAPaint::setTool( int n, bool bCancelDrawing )
{
    if ( n == nTool ) return;
    if ( bCancelDrawing && isDrawing() ) doCancel();
    if ( pTool ) doToolDelete();
    nTool = (Tools)n;
    doToolNew();
    emit signalChangedTool();
}

QPixmap PAPaint::getIcon()
{
    return QPixmap( ":PA/Paint" );
}

QVector<LTool> PAPaint::getTools()
{
    QVector<LTool> vectorReturn;

    vectorReturn.append( LTool( ToolSelectRectangle, tr("select using a rectangle"), QPixmap( ":PA/SelectRectangle" ), tr("select using a rectangle") ) );
    vectorReturn.append( LTool( ToolSelectEllipse, tr("select using ellipse"), QPixmap( ":PA/SelectEllipse" ), tr("select using ellipse") ) );
    vectorReturn.append( LTool( ToolSelectPolygon, tr("select using a polygon"), QPixmap( ":PA/SelectPolygon" ), tr("select using a polygon") ) );
    vectorReturn.append( LTool( ToolDrawArc, tr("arc"), QPixmap( ":PA/Arc" ), tr("arc") ) );
    vectorReturn.append( LTool( ToolDrawChord, tr("chord"), QPixmap( ":PA/Chord" ), tr("chord") ) );
    vectorReturn.append( LTool( ToolDrawPie, tr("pie"), QPixmap( ":PA/Pie" ), tr("pie") ) );
    vectorReturn.append( LTool( ToolDrawFreeHand, tr("free hand"), QPixmap( ":PA/FreeHand" ), tr("free hand") ) );
    vectorReturn.append( LTool( ToolDrawSpray, tr("spray"), QPixmap( ":PA/Spray" ), tr("spray") ) );
    vectorReturn.append( LTool( ToolDrawErase, tr("erase"), QPixmap( ":PA/Erase" ), tr("erase") ) );
    vectorReturn.append( LTool( ToolDrawLine, tr("line"), QPixmap( ":PA/Line" ), tr("line") ) );
    vectorReturn.append( LTool( ToolDrawRectangle, tr("rectangle"), QPixmap( ":PA/Rectangle" ), tr("rectangle") ) );
    vectorReturn.append( LTool( ToolDrawEllipse, tr("ellipse"), QPixmap( ":PA/Ellipse" ), tr("ellipse") ) );
    vectorReturn.append( LTool( ToolDrawPolygon, tr("polygon"), QPixmap( ":PA/Polygon" ), tr("polygon") ) );
    vectorReturn.append( LTool( ToolDrawPolyline, tr("polyline"), QPixmap( ":PA/Polyline" ), tr("polyline") ) );
    vectorReturn.append( LTool( ToolDrawRectangleFilled, tr("rectangle filled"), QPixmap( ":PA/RectangleFilled" ), tr("rectangle filled") ) );
    vectorReturn.append( LTool( ToolDrawEllipseFilled, tr("ellipse filled"), QPixmap( ":PA/EllipseFilled" ), tr("ellipse filled") ) );
    vectorReturn.append( LTool( ToolDrawPolygonFilled, tr("polygon filled"), QPixmap( ":PA/PolygonFilled" ), tr("polygon filled") ) );
    vectorReturn.append( LTool( ToolDrawText, tr("text"), QPixmap( ":PA/Text" ), tr("text") ) );
    vectorReturn.append( LTool( ToolFillFlood, tr("flood fill"), QPixmap( ":PA/FillFlood" ), tr("flood fill") ) );
    vectorReturn.append( LTool( ToolFillGradient, tr("gradient fill"), QPixmap( ":PA/FillGradient" ), tr("gradient fill") ) );
    vectorReturn.append( LTool( ToolMagnifierSelection, tr("select area for pixels editor"), QPixmap( ":PA/Pixels" ), tr("select area for pixels editor") ) );

    return vectorReturn;
}

QImage PAPaint::getCopyImage()
{
    if ( !pTool ) return QImage();
    if ( !pTool->canCopy() ) return QImage();
    return pTool->getCopy();
}

QWidget *PAPaint::getToolBar( QWidget *pParent )
{
    switch ( nTool )
    {
        case ToolSelectRectangle:    
            return g_SSettings->getSetting( "LSRectangle" )->getToolBar( pParent );
        case ToolSelectEllipse:      
            break;
        case ToolSelectPolygon:      
            break;
        case ToolDrawArc:      
            break;
        case ToolDrawChord:      
            break;
        case ToolDrawPie:      
            break;
        case ToolDrawFreeHand:               
            return g_SSettings->getSetting( "PASFreeHand" )->getToolBar( pParent );
        case ToolDrawSpray:               
            return g_SSettings->getSetting( "PASSpray" )->getToolBar( pParent );
        case ToolDrawErase:               
            return g_SSettings->getSetting( "PASErase" )->getToolBar( pParent );
        case ToolDrawLine:
            return g_SSettings->getSetting( "LSLine" )->getToolBar( pParent );
        case ToolDrawRectangle:      
            return g_SSettings->getSetting( "LSRectangle" )->getToolBar( pParent );
        case ToolDrawEllipse:        
            return g_SSettings->getSetting( "LSEllipse" )->getToolBar( pParent );
        case ToolDrawPolygon:        
            break;
        case ToolDrawPolyline:        
            break;
        case ToolDrawRectangleFilled:
            return g_SSettings->getSetting( "LSRectangle" )->getToolBar( pParent );
        case ToolDrawEllipseFilled:  
            break;
        case ToolDrawPolygonFilled:  
            return g_SSettings->getSetting( "PASPolygonFilled" )->getToolBar( pParent );
        case ToolDrawText:
            return g_SSettings->getSetting( "LSText" )->getToolBar( pParent );
        case ToolFillFlood:
            break;
        case ToolFillGradient:       
            return g_SSettings->getSetting( "PASGradient" )->getToolBar( pParent );
        case ToolMagnifierSelection:
            break;
    }
    return nullptr;
}

QWidget *PAPaint::getPanel( QWidget *pParent )
{
    switch ( nTool )
    {
        case ToolSelectRectangle:    
            break;
        case ToolSelectEllipse:      
            break;
        case ToolSelectPolygon:      
            break;
        case ToolDrawArc:      
            break;
        case ToolDrawChord:      
            break;
        case ToolDrawPie:      
            break;
        case ToolDrawFreeHand:               
            return g_SSettings->getSetting( "PASFreeHand" )->getPanel( pParent );
        case ToolDrawSpray:               
            return g_SSettings->getSetting( "PASSpray" )->getPanel( pParent );
        case ToolDrawErase:               
            break;
        case ToolDrawLine:
            return g_SSettings->getSetting( "LSLine" )->getPanel( pParent );
        case ToolDrawRectangle:      
            return g_SSettings->getSetting( "LSRectangle" )->getPanel( pParent );
        case ToolDrawEllipse:        
            return g_SSettings->getSetting( "LSEllipse" )->getPanel( pParent );
        case ToolDrawPolygon:        
            break;
        case ToolDrawPolyline:        
            break;
        case ToolDrawRectangleFilled:
            return g_SSettings->getSetting( "LSRectangle" )->getPanel( pParent );
        case ToolDrawEllipseFilled:  
            return g_SSettings->getSetting( "LSEllipse" )->getPanel( pParent );
        case ToolDrawPolygonFilled:  
            return g_SSettings->getSetting( "PASPolygonFilled" )->getPanel( pParent );
        case ToolDrawText:
            return g_SSettings->getSetting( "LSText" )->getPanel( pParent );
        case ToolFillFlood:
            break;
        case ToolFillGradient:       
            return g_SSettings->getSetting( "PASGradient" )->getPanel( pParent );
        case ToolMagnifierSelection:
            break;
    }
    return nullptr;
}

void PAPaint::doMousePressEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    if ( !pTool ) doToolNew();

    // ignore if outside of scene (happens when outside of scene but within view)
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;
    // convert to our simplified mouse event
    PMouseEvent mouseEvent( pEvent );

    pTool->doPress( &mouseEvent );

    // paste is a special tool - it never hangs around in idle state
    if ( pTool->inherits("PAPasteRectangle") && pTool->getState() == PATool::StateIdle ) 
    {
        doToolDelete();    // get rid of paste tool
        doToolNew();       // and put current tool in its place
    }
}

void PAPaint::doMouseMoveEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    // ignore if outside of scene (happens when outside of scene but within view)
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;
    // convert to our simplified mouse event
    PMouseEvent mouseEvent( pEvent );

    if ( !pTool ) return; 

    pTool->doMove( &mouseEvent );
}

void PAPaint::doMouseReleaseEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    if ( !pTool ) return; 

    // release is outside of scene is not a problem (prevented/handled in the move)
    // if ( !scene()->sceneRect().contains( pEvent->scenePos() ) ) return;

    // convert to our simplified mouse event
    PMouseEvent mouseEvent( pEvent );

    // pass event to 'tool'
    pTool->doRelease( &mouseEvent );
}

void PAPaint::doDragEnterEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );

}

void PAPaint::doDragMoveEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );

}

void PAPaint::doDropEvent( QGraphicsSceneDragDropEvent * )
{
    Q_ASSERT( pGraphics );

}

void PAPaint::doContextMenuEvent( QGraphicsSceneContextMenuEvent * )
{
    Q_ASSERT( pGraphics );

}

void PAPaint::doMouseDoubleClickEvent( QGraphicsSceneMouseEvent *pEvent )
{
    Q_ASSERT( pGraphics );

    if ( !pTool ) return; 

    // ignore if outside of scene (happens when outside of scene but within view)
    if ( !pGraphics->scene()->sceneRect().contains( pEvent->scenePos() ) ) return;
    // convert to our simplified mouse event
    PMouseEvent mouseEvent( pEvent );

    pTool->doDoubleClick( &mouseEvent );
}

QDomElement PAPaint::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = LLayer::doSave( pdomDoc, pdomElemParent );

    // save image as attribute
    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( image, &nBytesCompressed );
    domElemThis.setAttribute( "BitmapSize", QString::number( nBytesCompressed ) );
    domElemThis.setAttribute( "Bitmap", stringEncodedImage );

    return domElemThis;
}

bool PAPaint::doLoad( QDomElement *pdomElemThis )
{
    LLayer::doLoad( pdomElemThis );                                                

    ulong nBytesCompressed = pdomElemThis->attribute( "BitmapSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElemThis->attribute( "Bitmap" );
        if ( !stringAttribute.isNull() ) 
        {
            image = CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed );
        }
    }
    return true;                                                                     
}

void PAPaint::doCut()
{
    if ( pTool && pTool->canCut() ) 
    {
        pTool->doCut();
        pGraphics->update();
        if ( isDrawing() ) doCancel();
    }
}

void PAPaint::doCopy()
{
    if ( pTool && pTool->canCopy() ) 
    {
        pTool->doCopy();
        if ( isDrawing() ) doCancel();
    }
}

void PAPaint::doPaste()
{
    if ( isDrawing() ) doCancel();
    if ( pTool ) doToolDelete();

    doToolNew( QGuiApplication::clipboard()->image() );
}

void PAPaint::doPaste( const QImage &i )
{
    if ( isDrawing() ) doCancel();
    if ( pTool ) doToolDelete();

    doToolNew( i );
}

void PAPaint::doUndo()
{
    Q_ASSERT( !stackUndo.isEmpty() );
    stackRedo.push( image );
    image = stackUndo.pop();
    pGraphics->update();
    setModified();
    emit signalChanged();
    emit signalPixelRefreshImage();
}

void PAPaint::doRedo()
{
    Q_ASSERT( !stackRedo.isEmpty() );
    stackUndo.push( image );
    image = stackRedo.pop();
    pGraphics->update();
    setModified();
    emit signalChanged();
    emit signalPixelRefreshImage();
}

void PAPaint::doSelectAll()
{
    setTool( ToolSelectRectangle );
    ((PASelectRectangle*)pTool)->doSelectAll();
}

void PAPaint::doSelectNone()
{
    if ( hasSelection() ) doCancel();
}

void PAPaint::doCrop()
{
    if ( !canCopy() ) return;

    QImage i = pTool->getCopy();
    doCancel();
    slotPreCommit();
    image = i;
    pGraphics->scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    slotCommitted();
}

void PAPaint::doScale( qreal nX, qreal nY, Qt::AspectRatioMode n )
{
    // scale shape?
    if ( pTool && pTool->doScale( nX, nY, n ) ) return;

    // scale layer
    if ( isDrawing() ) doCancel();
    slotPreCommit();
    image = image.scaled( (int)nX, (int)nY, n, Qt::SmoothTransformation );

    // assumes we are the only layer but...
    pGraphics->scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );

    slotCommitted();
}

void PAPaint::doFlipX()
{
    // try shape first
    if ( pTool && pTool->doFlipX() ) return;
    // cancel any drawing and flip entire canvas
    if ( isDrawing() ) doCancel();
    slotPreCommit();
    image = image.mirrored( true, false );
    slotCommitted();
}

void PAPaint::doFlipY()
{
    // try shape first
    if ( pTool && pTool->doFlipY() ) return;
    // cancel any drawing and flip entire canvas
    if ( isDrawing() ) doCancel();
    slotPreCommit();
    image = image.mirrored( false, true );
    slotCommitted();
}

void PAPaint::doRotate( int nDegrees )
{
    // try shape first
    if ( pTool && pTool->doRotate( nDegrees ) ) return;
    // cancel any drawing and rotate entire canvas
    if ( isDrawing() ) doCancel();
    slotPreCommit();
    image = image.transformed( QTransform().rotate( nDegrees ) );
    pGraphics->scene()->setSceneRect( QRectF( 0, 0, image.size().width(), image.size().height() ) );
    slotCommitted();
}

void PAPaint::doDelete()
{
    if ( !pTool || !pTool->canDelete() ) return; 
    pTool->doDelete();
    pGraphics->update();

    if ( isDrawing() ) doCancel();
}

void PAPaint::doCommit()
{
    Q_ASSERT( pTool );

    // doCommit() should trigger PAPaint::slotCommitted()
    if ( pTool->canCommit() ) pTool->doCommit(); 

    // paste is a special tool - it never hangs around in idle state
    if ( pTool->inherits("PAPasteRectangle") && pTool->getState() == PATool::StateIdle ) 
    {
        doToolDelete();    // get rid of paste tool
        doToolNew();       // and put current tool in its place
        emit signalChangedToolState();
    }

    Q_ASSERT( pTool->getState() == PATool::StateIdle );
}

void PAPaint::doCancel()
{
    Q_ASSERT( pTool );

    if ( pTool->canCancel() ) pTool->doCancel();

    // paste is a special tool - it never hangs around in idle state
    if ( pTool->inherits("PAPasteRectangle") && pTool->getState() == PATool::StateIdle ) 
    {
        doToolDelete();    // get rid of paste tool
        doToolNew();       // and put current tool in its place
        emit signalChangedToolState();
    }

    Q_ASSERT( pTool->getState() == PATool::StateIdle );
}

void PAPaint::doPushUndo()
{
    stackRedo.clear();
    stackUndo.push( image );
    if ( stackUndo.count() >= nMaxUndo ) stackUndo.removeFirst();
}

void PAPaint::doZoomChanged( int nZoom )
{
    if ( pTool ) pTool->doZoomChanged( nZoom );
}

void PAPaint::doScrollChanged( int nX, int nY )
{
    if ( pTool ) pTool->doScrollChanged( nX, nY );
}

bool PAPaint::isDrawing()
{
    return ( pTool && pTool->getState() != PATool::StateIdle );
}

bool PAPaint::hasSelection()
{
    return ( pTool && pTool->isSelector() );
}

bool PAPaint::canCut()
{
    if ( pTool ) return pTool->canCut();
    return false;
}

bool PAPaint::canCopy() 
{
    if ( pTool ) return pTool->canCopy();
    return false;
}

bool PAPaint::canPaste() 
{
    QClipboard *pClipboard = QGuiApplication::clipboard();
    const QMimeData *pMimeData = pClipboard->mimeData();

    if ( pMimeData->hasImage() ) return true;

    return false;
}

bool PAPaint::canUndo() 
{
    return !stackUndo.isEmpty();
}

bool PAPaint::canRedo() 
{
    return !stackRedo.isEmpty();
}

bool PAPaint::canSelectAll()
{
    return true;
}

bool PAPaint::canSelectNone()
{
    return hasSelection();
}

bool PAPaint::canDelete()
{
    return hasSelection();
}

bool PAPaint::canCommit() 
{
    if ( pTool ) return pTool->canCommit();
    return false;
}

bool PAPaint::canCancel() 
{
    if ( pTool ) return pTool->canCancel();
    return false;
}

bool PAPaint::canScale()
{
    return true;
}

bool PAPaint::canFlipX()
{
    return true;
}

bool PAPaint::canFlipY()
{
    return true;
}

bool PAPaint::canRotate()
{
    return true;
}

void PAPaint::slotSceneRectChanged( const QRectF &r )
{
    // cancel any drawing and resize image (we still take up entire scene)
    if ( isDrawing() ) doCancel();

    slotPreCommit();

    int nWidth  = image.width() > r.width() ? r.width() : image.width();
    int nHeight = image.height() > r.height() ? r.height() : image.height();

    image = image.copy( 0, 0, nWidth, nHeight ); 

    slotCommitted();
}

void PAPaint::slotRefreshImage()
{
    Qt::ImageConversionFlag nColorMode      = Qt::ImageConversionFlag(pSettingImage->property( "nColorMode" ).toInt());
    Qt::ImageConversionFlag nAlphaDither    = Qt::ImageConversionFlag(pSettingImage->property( "nAlphaDither" ).toInt());
    Qt::ImageConversionFlag nDither         = Qt::ImageConversionFlag(pSettingImage->property( "nDither" ).toInt());
    Qt::ImageConversionFlag nDitherMode     = Qt::ImageConversionFlag(pSettingImage->property( "nDitherMode" ).toInt());
    flagsImageConversion = nColorMode | nAlphaDither | nDither | nDitherMode;
}

void PAPaint::slotPreCommit()
{
    doPushUndo();
}

void PAPaint::slotCommitted()
{
    // we have been modified
    setModified();
    // setModified may - or may not - have done this so we do it here to ensure it happens
    emit signalChanged(); 
    pGraphics->update();
}
                                                                                     
void PAPaint::paint( QPainter *pPainter, const QStyleOptionGraphicsItem *pOptions, QWidget *pWidget )
{
    Q_UNUSED( pWidget );
    Q_UNUSED( pOptions );

/*
    if ( nGridType == L::GridTypePixel )                                                                                                         
    {                                                                                                                                            
        QRectF r = pOptions->exposedRect;                                                                                                        
        QBrush brushTransparency; // todo tap into general                                                                                       
        QPen pen( Qt::white );                                                                                                                   
        pPainter->setPen( pen );                                                                                                                 
                                                                                                                                                 
        for ( int nXCell = 0; nXCell < r.width(); nXCell++ )                                                                                     
        {                                                                                                                                        
            for ( int nYCell = 0; nYCell < r.height(); nYCell++ )                                                                                
            {                                                                                                                                    
                QColor color = image.pixelColor( nXCell, nYCell );                                                                               
                if ( color.alpha() > 0 && color != Qt::transparent )                                                                             
                    pPainter->setBrush( color );                                                                                                 
                else                                                                                                                             
                    pPainter->setBrush( brushTransparency );                                                                                     
                pPainter->drawRect( nXCell * sizeGridCell.width(), nYCell * sizeGridCell.height(), sizeGridCell.width(), sizeGridCell.height() );
                pPainter->setBrush( QBrush() );                                                                                                  
                                                                                                                                                 
            }                                                                                                                                    
        }                                                                                                                                        
    }                                                                                                                                            
    else                                                                                                                                         
*/
    {
        // has scene been resized?
        if ( boundingRect().size().toSize() != image.size() )
        {
            QImage i( boundingRect().size().toSize(), QImage::Format_ARGB32 );
            i.fill( Qt::transparent );
            QPainter p( &i );
            p.drawImage( 0, 0, image );
            image = i;
        }

        pPainter->drawImage( 0, 0, image, flagsImageConversion );
    }
}

void PAPaint::doToolDelete()
{
    Q_ASSERT( pTool );
    Q_ASSERT( pTool->getState() == PATool::StateIdle );

    delete pTool;
    pTool = nullptr;
    emit signalChanged();
}

void PAPaint::doToolNew()
{
    Q_ASSERT( !pTool );
    
    switch ( nTool )
    {
        case ToolSelectRectangle:    
            pTool = new PASelectRectangle( this );           
            break;
        case ToolSelectEllipse:      
            pTool = new PASelectEllipse( this );           
            break;
        case ToolSelectPolygon:      
            pTool = new PASelectPolygon( this );           
            break;
        case ToolDrawArc:      
            pTool = new PAArc( this );           
            break;
        case ToolDrawChord:      
            pTool = new PAChord( this );           
            break;
        case ToolDrawPie:      
            pTool = new PAPie( this );           
            break;
        case ToolDrawFreeHand:               
            pTool = new PAFreeHand( this );           
            break;
        case ToolDrawSpray:               
            pTool = new PASpray( this );           
            break;
        case ToolDrawErase:               
            pTool = new PAErase( this );           
            break;
        case ToolDrawLine:
            pTool = new PALine( this );
            break;
        case ToolDrawRectangle:      
            pTool = new PARectangle( this );           
            break;
        case ToolDrawEllipse:        
            pTool = new PAEllipse( this );           
            break;
        case ToolDrawPolygon:        
            pTool = new PAPolygon( this );           
            break;
        case ToolDrawPolyline:        
            pTool = new PAPolyline( this );           
            break;
        case ToolDrawRectangleFilled:
            pTool = new PARectangleFilled( this );           
            break;
        case ToolDrawEllipseFilled:  
            pTool = new PAEllipseFilled( this );           
            break;
        case ToolDrawPolygonFilled:  
            pTool = new PAPolygonFilled( this );           
            break;
        case ToolDrawText:  
            pTool = new PAText( this );           
            break;
        case ToolFillFlood:
            pTool = new PAFloodFill( this );           
            break;
        case ToolFillGradient:       
            pTool = new PAGradientFill( this );           
            break;
        case ToolMagnifierSelection:
            pTool = new PAPixelSelection( this );           
            connect( pTool, SIGNAL(signalMoved(const QPoint &)), SIGNAL(signalPixelSelectionMoved(const QPoint &)) );
            break;
    }

    Q_ASSERT( pTool );

    // shape will make itself a child of the canvas in its constructor - no need to add it to the scene again
    // scene()->addItem( pTool );
    connect( pTool, SIGNAL(signalChanged()), SIGNAL(signalChangedToolState()) );
    connect( pTool, SIGNAL(signalPreCommit()), SLOT(slotPreCommit()) );
    connect( pTool, SIGNAL(signalCommitted()), SLOT(slotCommitted()) );
    emit signalChanged();
}

void PAPaint::doToolNew( const QImage &i )
{
    Q_ASSERT( !pTool );
    
    pTool = new PAPasteRectangle( this, i );

    connect( pTool, SIGNAL(signalChanged()), SIGNAL(signalChangedToolState()) );
    connect( pTool, SIGNAL(signalPreCommit()), SLOT(slotPreCommit()) );
    connect( pTool, SIGNAL(signalCommitted()), SLOT(slotCommitted()) );
    emit signalChanged();
    emit signalChangedToolState();
}

void PAPaint::doClear()
{
    image.fill( colorBackground );
    setModified( false );
    if ( pGraphics ) doGraphicsDelete();
}
 

