#include "LibInfo.h"
#include "DBitmap.h"

#include "DBitmapWidget.h"

#include "DGraphicsProxyItem.h"

DBitmap::DBitmap( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
//    border.setStyle( Qt::NoPen );
    pixmap = QPixmap( ":D/Image" );
    resizedPixmap = pixmap.scaled( rect.width(), rect.height() );
    // pixmap.setOptimization(QPixmap::BestOptim);
    // resizedPixmap.setOptimization(QPixmap::BestOptim);
    nPlacement = W::WImagePlacementScaled;
}

DBitmap::~DBitmap()
{
}

QPixmap DBitmap::getIcon()
{
    return QPixmap( ":D/Image" );
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties. 
 *  
 * Allow a vector image (svg/svgz file) to be loaded. 
 *  
 * \author pharvey (5/9/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DBitmap::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DRectangle::getObjectWidget( pWidgetParent );

    p->insertTab( 1, new DBitmapWidget( this, pWidgetParent ), tr( "Bitmap" ) );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DBitmap::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    // image
    {
        DBitmapWidget *p = new DBitmapWidget( this, pPropWidget );

        pPropWidget->addWidget( tr("Image"), p );
    }

    // caller will add more (if needed) and/or off to the app
    // caller must delete
    return pPropWidget;

}

QPixmap DBitmap::getPixmap() 
{ 
    return pixmap; 
}

W::WImagePlacements DBitmap::getPlacement()
{
    return nPlacement;
}

void DBitmap::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    if ( pixmap.isNull() ) return DRectangle::paintAt( pPainter, pointPos );

    QRectF rect;
    rect.setTopLeft( pointPos );
    rect.setSize( this->rect.size() );

    switch ( nPlacement )
    {
    case W::WImagePlacementNormal:
        pPainter->drawPixmap( rect.x(), rect.y(), pixmap, 0, 0, rect.width(), rect.height() );
        break;
    case W::WImagePlacementScaled:
        pPainter->drawPixmap( rect.x(), rect.y(), resizedPixmap, 0, 0, rect.width(), rect.height() );
        break;
    case W::WImagePlacementTiled:
        pPainter->drawTiledPixmap( rect, pixmap );
        break;
    }

    // corners
/*
    QPen    penCorners( Qt::black );

    pPainter->setPen( penCorners );
    pPainter->drawLine( rect.topLeft(), QPoint( rect.topLeft().x(), rect.topLeft().y() + 3 ) );
    pPainter->drawLine( rect.topLeft(), QPoint( rect.topLeft().x() + 3, rect.topLeft().y() ) );

    pPainter->drawLine( rect.topRight(), QPoint( rect.topRight().x(), rect.topRight().y() + 3 ) );
    pPainter->drawLine( rect.topRight(), QPoint( rect.topRight().x() - 3, rect.topRight().y() ) );

    pPainter->drawLine( rect.bottomLeft(), QPoint( rect.bottomLeft().x(), rect.bottomLeft().y() - 3 ) );
    pPainter->drawLine( rect.bottomLeft(), QPoint( rect.bottomLeft().x() + 3, rect.bottomLeft().y() ) );

    pPainter->drawLine( rect.bottomRight(), QPoint( rect.bottomRight().x(), rect.bottomRight().y() - 3 ) );
    pPainter->drawLine( rect.bottomRight(), QPoint( rect.bottomRight().x() - 3, rect.bottomRight().y() ) );
*/ 
}

QDomElement DBitmap::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff and children...
    QDomElement domElemImage = DRectangle::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmap.toImage(), &nBytesCompressed );
    domElemImage.setAttribute( "ImageSize", QString::number( nBytesCompressed ) );
    domElemImage.setAttribute( "Image", stringEncodedImage );
    domElemImage.setAttribute( "ImagePlacement", getPlacement() );

    return domElemImage;
}

bool DBitmap::doLoad( QDomElement *pdomElem )
{
    // load common stuff and children...
    DRectangle::doLoad( pdomElem );

    // load additional attributes...
    QString stringAttribute;

    stringAttribute = pdomElem->attribute( "ImagePlacement" );
    if ( !stringAttribute.isNull() )
        slotPlacement( (W::WImagePlacements)stringAttribute.toInt() );

    stringAttribute = pdomElem->attribute( "ImageSize" );
    if ( stringAttribute.isNull() )
        return false;
    ulong nBytesCompressed = stringAttribute.toULong();

    stringAttribute = pdomElem->attribute( "Image" );
    if ( stringAttribute.isNull() )
        return false;

    pixmap = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
    resizedPixmap = pixmap.scaled( rect.width(), rect.height() );

    return true;
}

void DBitmap::slotPixmap( const QPixmap &pixmap ) 
{ 
    this->pixmap = pixmap; 
    resizedPixmap = pixmap.scaled( rect.width(), rect.height() );
    if ( getProxy() ) getProxy()->update();
    setModified();
}

void DBitmap::slotPlacement( int n )
{
    nPlacement = (W::WImagePlacements)n;
    if ( getProxy() ) getProxy()->update();
    setModified();
}

/*!
 * \brief Move a handle. 
 *  
 * Have DRectangle do its thing and then update our scaled pixmap 
 * to reflect any change in rectangle geometry. 
 * 
 * \author pharvey (11/24/19)
 * 
 * \param pHandle 
 * \param pointScenePos 
 */
void DBitmap::doHandleMove( DHandle *pHandle, const QPointF &pointScenePos )
{
    DRectangle::doHandleMove( pHandle, pointScenePos );
    resizedPixmap = pixmap.scaled( rect.width(), rect.height() );
}


