#include "LibInfo.h"
#include "DVector.h"

#include "DGraphicsProxyItem.h"
#include "DVectorWidget.h"

DVector::DVector( ADObject *pObjectParent, const QString &stringName )
    : DRectangle( pObjectParent, stringName )
{
    pRenderer = new QSvgRenderer( this );
}

DVector::~DVector()
{
    delete pRenderer;
}

QPixmap DVector::getIcon()
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
AWDataWidget *DVector::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DRectangle::getObjectWidget( pWidgetParent );

    p->insertTab( 1, new DVectorWidget( this, p ), tr("Image") );

    p->setCurrentIndex( 0 );

    return p;
}

AWPropWidget *DVector::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    pPropWidget->addWidget( tr("Image"), new DVectorWidget( this, pPropWidget ) );

    return pPropWidget;

}

void DVector::paintAt( QPainter *pPainter, const QPointF &pointPos )
{
    QRectF r;
    r.setTopLeft( pointPos );
    r.setSize( rect.size() );

    if ( ByteArray.isNull() ) return DRectangle::paintAt( pPainter, pointPos );

    pRenderer->render( pPainter, r );

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

QDomElement DVector::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff and children...
    QDomElement domElemImage = DRectangle::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    ulong nBytesCompressed = 0;
    QString stringEncodedImage = CBDPersistUtility::getEncodedBinary( ByteArray, &nBytesCompressed );
    domElemImage.setAttribute( "ImageSize", QString::number( nBytesCompressed ) );
    domElemImage.setAttribute( "Image", stringEncodedImage );

    return domElemImage;
}

bool DVector::doLoad( QDomElement *pdomElem )
{
    // load common stuff and children...
    DRectangle::doLoad( pdomElem );

    // load additional attributes...
    QString stringAttribute;

    stringAttribute = pdomElem->attribute( "ImageSize" );
    if ( stringAttribute.isNull() )
        return false;
    ulong nBytesCompressed = stringAttribute.toULong();

    stringAttribute = pdomElem->attribute( "Image" );
    if ( stringAttribute.isNull() )
        return false;

    ByteArray = CBDPersistUtility::getDecodedBinary( stringAttribute, nBytesCompressed );
    if ( !pRenderer->load( ByteArray ) )
    { 
        doMessageBox( "ERROR", tr("Load"), tr("Encountered an error while loading vector.") );
    }

    return true;
}

void DVector::slotVector( const QByteArray &ba ) 
{
    ByteArray = ba; 

    if ( !pRenderer->load( ByteArray ) )
    { 
        doMessageBox( "ERROR", tr("Load"), tr("Encountered an error while loading vector.") );
    }

    if ( getProxy() ) getProxy()->update();
    setModified();
}


