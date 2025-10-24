#include "LibInfo.h"
#include "DStencilItem.h"

#include <WSvgButton.h>
#include <WBitmapButton.h>

DStencilItem::DStencilItem( AWObject *pParent, const QString &stringName )
    : AWObject( pParent, stringName )
{
}

DStencilItem::~DStencilItem()
{
}

void DStencilItem::setIcon( const QPixmap &p )
{
    pixmapIcon = p;
    setModified();
}

QPixmap DStencilItem::getIcon()
{
    if ( isNull() ) return QPixmap( ":D/Image" );
    return pixmapIcon;
}

/*!
 * \brief Creates a tab widget with a number of tabs for editing/viewing object properties.
 *  
 * We introduce a way to select the image; 
 *  
 * \author pharvey (5/8/20)
 * 
 * \param pWidgetParent 
 * 
 * \return AWDataWidget* 
 */
AWDataWidget *DStencilItem::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = AWObject::getObjectWidget( pWidgetParent );

    return p;
}

QMenu *DStencilItem::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = new QMenu( pParent );

    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Object..."), this, SLOT(slotObjectDialog()) );
    pMenu->addSeparator();
    pMenu->addAction( QPixmap( ":AW/Delete" ), tr("Delete ") + metaObject()->className(), this, SLOT(slotDelete()) );

    return pMenu; // caller invokes and then deletes
}

QDomElement DStencilItem::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff and children...
    QDomElement domElemImage = AWObject::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    if ( !pixmapIcon.isNull() )
    {
        ulong nBytesCompressed = 0;
        QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmapIcon.toImage(), &nBytesCompressed );
        domElemImage.setAttribute( "IconSize", QString::number( nBytesCompressed ) );
        domElemImage.setAttribute( "Icon", stringEncodedImage );
    }

    return domElemImage;
}

bool DStencilItem::doLoad( QDomElement *pdomElem )
{
    ulong nBytesCompressed = pdomElem->attribute( "IconSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "Icon" );
        if ( !stringAttribute.isEmpty() ) 
        {
            pixmapIcon = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
        }
    }

    return AWObject::doLoad( pdomElem );
}

void DStencilItem::slotIcon( const QPixmap &pixmap )
{
    setIcon( pixmap );
}

// *******************************

DStencilItemSvg::DStencilItemSvg( AWObject *pParent, const QString &stringName )
    : DStencilItem( pParent, stringName )
{
}

DStencilItemSvg::~DStencilItemSvg()
{
}

AWDataWidget *DStencilItemSvg::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DStencilItem::getObjectWidget( pWidgetParent );

    // image
    {
        WSvgButton *pSvgButton = new WSvgButton( ByteArray, p );
        connect( pSvgButton, SIGNAL(signalChanged(const QByteArray &)), this, SLOT(slotVector(const QByteArray &)) );

        p->insertTab( 1, pSvgButton, tr("Image") );
    }

    p->setCurrentIndex( 0 );

    return p;
}

bool DStencilItemSvg::isNull()
{
    return ( ByteArray.isEmpty() || pixmapIcon.isNull() );
}

QDomElement DStencilItemSvg::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff and children...
    QDomElement domElemImage = DStencilItem::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    if ( !ByteArray.isEmpty() )
    {
        ulong nBytesCompressed = 0;
        QString stringEncodedImage = CBDPersistUtility::getEncodedBinary( ByteArray, &nBytesCompressed );
        domElemImage.setAttribute( "SvgSize", QString::number( nBytesCompressed ) );
        domElemImage.setAttribute( "Svg", stringEncodedImage );
    }

    return domElemImage;
}

bool DStencilItemSvg::doLoad( QDomElement *pdomElem )
{
    ulong nBytesCompressed = pdomElem->attribute( "SvgSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "Svg" );
        if ( !stringAttribute.isEmpty() ) 
        {
            ByteArray = CBDPersistUtility::getDecodedBinary( stringAttribute, nBytesCompressed );
            if ( pixmapIcon.isNull() ) doUpdateIcon();
        }
    }

    return DStencilItem::doLoad( pdomElem );
}

void DStencilItemSvg::setVector( const QByteArray &ba )
{
    // adopt the data
    ByteArray = ba;
    doUpdateIcon();
    setModified();
}

void DStencilItemSvg::slotVector( const QByteArray &ba )
{
    setVector( ba );
}

void DStencilItemSvg::doUpdateIcon()
{
    QPixmap pixmap;

    //
    if ( !pixmap.loadFromData( ByteArray ) )
    { 
        doMessageBox( "ERROR", tr("Load"), tr("Encountered an error while getting a pixmap version of the vector.") );
        return;
    }

    setIcon( pixmap.scaled( 48, 48, Qt::KeepAspectRatio ) );
}

// *******************************

DStencilItemBitmap::DStencilItemBitmap( AWObject *pParent, const QString &stringName )
    : DStencilItem( pParent, stringName )
{
}

DStencilItemBitmap::~DStencilItemBitmap()
{
}

AWDataWidget *DStencilItemBitmap::getObjectWidget( QWidget *pWidgetParent )
{
    AWDataWidget *p = DStencilItem::getObjectWidget( pWidgetParent );

    // image
    {
        WBitmapButton *pBitmapButton = new WBitmapButton( pixmap, p );
        connect( pBitmapButton, SIGNAL(signalChanged(const QPixmap &)), this, SLOT(slotPixmap(const QPixmap &)) );

        p->insertTab( 1, pBitmapButton, tr("Image") );
    }

    p->setCurrentIndex( 0 );

    return p;
}

bool DStencilItemBitmap::isNull()
{
    return pixmap.isNull();
}

QDomElement DStencilItemBitmap::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common stuff and children...
    QDomElement domElemImage = DStencilItem::doSave( pdomDoc, pdomElemParent );

    // save additional attributes...
    if ( !isNull() )
    {
        ulong nBytesCompressed = 0;
        QString stringEncodedImage = CBDPersistUtility::getEncodedImage( pixmap.toImage(), &nBytesCompressed );
        domElemImage.setAttribute( "BitmapSize", QString::number( nBytesCompressed ) );
        domElemImage.setAttribute( "Bitmap", stringEncodedImage );
    }

    return domElemImage;
}

bool DStencilItemBitmap::doLoad( QDomElement *pdomElem )
{
    ulong nBytesCompressed = pdomElem->attribute( "BitmapSize", "0" ).toULong();
    if ( nBytesCompressed )
    { 
        QString stringAttribute = pdomElem->attribute( "Bitmap" );
        if ( !stringAttribute.isEmpty() ) 
        {
            pixmap = QPixmap::fromImage( CBDPersistUtility::getDecodedImage( stringAttribute, nBytesCompressed ) );
            if ( pixmapIcon.isNull() ) doUpdateIcon();
        }
    }

    return DStencilItem::doLoad( pdomElem );
}

void DStencilItemBitmap::setPixmap( const QPixmap &pixmap )
{
    // adopt the data
    this->pixmap = pixmap;
    doUpdateIcon();
    setModified();
}

void DStencilItemBitmap::slotPixmap( const QPixmap &pixmap )
{
    setPixmap( pixmap );
}

void DStencilItemBitmap::doUpdateIcon()
{
    setIcon( pixmap.scaled( 48, 48, Qt::KeepAspectRatio ) );
}



