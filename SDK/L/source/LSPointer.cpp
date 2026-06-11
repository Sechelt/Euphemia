/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSPointer.h"

#include <SSettings.h>

/****************************************************************
 * LSPointer
 ****************************************************************/
LSPointer::LSPointer()
{
}

void LSPointer::setStandard( Qt::CursorShape t )
{
    if ( nStandard == t ) return;
    nStandard = t;
    emit signalChanged();
}

void LSPointer::setResource( const QString &t )
{
    if ( stringResource == t ) return;
    stringResource = t;
    emit signalChanged();
}

void LSPointer::setImage( const QImage &t )
{
    if ( image == t ) return;
    image = t;
    emit signalChanged();
}

void LSPointer::setHotSpot( const QPoint &t )
{
    if ( pointHotSpot == t ) return;
    pointHotSpot = t;
    emit signalChanged();
}

void LSPointer::setCrossHairs( bool t )
{
    if ( bCrossHairs == t ) return;
    bCrossHairs = t;
    emit signalChanged();
}

void LSPointer::setPen( const QPen &t )
{
    if ( pen == t ) return;
    pen = t;
    emit signalChanged();
}

void LSPointer::setSelected( Qt::CursorShape nStandard, const QString &stringResource, const QImage &image, const QPoint &pointHotSpot, bool bCrossHairs, const QPen &pen )
{
    this->nStandard         = nStandard;
    this->stringResource    = stringResource;
    this->image             = image;
    this->pointHotSpot      = pointHotSpot;
    this->bCrossHairs       = bCrossHairs;
    this->pen               = pen;
    emit signalSync();
}

QCursor LSPointer::getCursor()
{
    if ( nStandard == Qt::BitmapCursor )
    {
        if ( stringResource.isEmpty() )
        {
            if ( image.isNull() )
            {
                return QCursor( Qt::ArrowCursor );
            }
            else
            {
                return QCursor( QPixmap::fromImage( image ) );
            }
        }
        else
        {
            return QCursor( QPixmap( stringResource ) );
        }
    }
    else
        return QCursor( nStandard );
}

QSize LSPointer::getSizeCursor()
{
    if ( nStandard == Qt::BitmapCursor )
    {
        if ( stringResource.isEmpty() )
        {
            if ( image.isNull() )
            {
                return QSize( 0, 0 );
            }
            else
            {
                return image.size();
            }
        }
        else
        {
            return QPixmap( stringResource ).size();
        }
    }
    else
        return QSize( 0, 0 );
}

QWidget *LSPointer::getToolBar( QWidget *pParent )
{
    return new LSPointerWidget( pParent, Qt::Horizontal );
}

QWidget *LSPointer::getPanel( QWidget *pParent )
{
    return new LSPointerWidget( pParent, Qt::Vertical );
}

void LSPointer::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSPointer" );

    if ( !image.isNull() ) domElem.appendChild( CBDPersistNative::doSaveImage( image, pdomDoc, "Image" ) );
    domElem.setAttribute( "Standard", (int)nStandard );
    domElem.setAttribute( "Resource", stringResource );
    domElem.setAttribute( "HotSpot.x", pointHotSpot.x() );
    domElem.setAttribute( "HotSpot.y", pointHotSpot.y() );
    domElem.setAttribute( "CrossHairs", bCrossHairs );
    domElem.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc, "Pen" ) );

    pdomElem->appendChild( domElem );
}

void LSPointer::doLoad( QDomElement *pdomElem )
{
    nStandard = Qt::CursorShape( pdomElem->attribute( "Standard", QString::number( (int)nStandard ) ).toInt() );
    stringResource = pdomElem->attribute( "Resource", stringResource );
    pointHotSpot.setX( pdomElem->attribute( "HotSpot.x", QString::number( pointHotSpot.x() ) ).toInt() );
    pointHotSpot.setY( pdomElem->attribute( "HotSpot.y", QString::number( pointHotSpot.y() ) ).toInt() );
    bCrossHairs = pdomElem->attribute( "CrossHairs", QString::number( bCrossHairs ) ).toInt();

    // load child nodes (of interest)
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElem->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "Pen" ) pen = CBDPersistNative::doLoadPen( &domElem );
        else if ( domElem.tagName() == "Image" ) image = CBDPersistNative::doLoadImage( &domElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * LSPointerWidget
 ****************************************************************/
LSPointerWidget::LSPointerWidget( QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "LSPointerWidget" );

    pSetting = (LSPointer*)g_SSettings->getSetting( "LSPointer" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSPointer].";
        return;
    }

    pStandard = new QComboBox( this );
    pStandard->setInsertPolicy( QComboBox::InsertAtBottom );
    pStandard->addItem( "BitmapCursor", (int)Qt::BitmapCursor );
    pStandard->addItem( "ArrowCursor", (int)Qt::ArrowCursor );
    pStandard->addItem( "UpArrowCursor", (int)Qt::UpArrowCursor );
    pStandard->addItem( "CrossCursor", (int)Qt::CrossCursor );
    pStandard->addItem( "WaitCursor", (int)Qt::WaitCursor );
    pStandard->addItem( "IBeamCursor", (int)Qt::IBeamCursor );
    pStandard->addItem( "SizeVerCursor", (int)Qt::SizeVerCursor );
    pStandard->addItem( "SizeHorCursor", (int)Qt::SizeHorCursor );
    pStandard->addItem( "SizeBDiagCursor", (int)Qt::SizeBDiagCursor );
    pStandard->addItem( "SizeFDiagCursor", (int)Qt::SizeFDiagCursor );
    pStandard->addItem( "SizeAllCursor", (int)Qt::SizeAllCursor );
    pStandard->addItem( "BlankCursor", (int)Qt::BlankCursor );
    pStandard->addItem( "SplitVCursor", (int)Qt::SplitVCursor );
    pStandard->addItem( "SplitHCursor", (int)Qt::SplitHCursor );
    pStandard->addItem( "PointingHandCursor", (int)Qt::PointingHandCursor );
    pStandard->addItem( "ForbiddenCursor", (int)Qt::ForbiddenCursor );
    pStandard->addItem( "WhatsThisCursor", (int)Qt::WhatsThisCursor );
    pStandard->addItem( "BusyCursor", (int)Qt::BusyCursor );
    pStandard->addItem( "OpenHandCursor", (int)Qt::OpenHandCursor );
    pStandard->addItem( "ClosedHandCursor", (int)Qt::ClosedHandCursor );
    pStandard->addItem( "DragCopyCursor", (int)Qt::DragCopyCursor );
    pStandard->addItem( "DragMoveCursor", (int)Qt::DragMoveCursor );
    pStandard->addItem( "DragLinkCursor", (int)Qt::DragLinkCursor );
    pStandard->setCurrentIndex( pStandard->findData( (int)pSetting->getStandard() ) );
    connect( pStandard, SIGNAL(currentIndexChanged(int)), SLOT(slotStandard(int)) );

    pResource = new QComboBox( this );
    pResource->setInsertPolicy( QComboBox::InsertAtBottom );
    pResource->addItem( QIcon( ":L/SquareDot" ), "SquareDot", ":L/SquareDot" );
    pResource->addItem( QIcon( ":L/CircleDot" ), "CircleDot", ":L/CircleDot" );
    pResource->setCurrentIndex( pResource->findData( pSetting->getResource() ) );
    connect( pResource, SIGNAL(currentIndexChanged(int)), SLOT(slotResource(int)) );

    pImage = new WImageButton( this, pSetting->getImage() );
    pImage->setToolTip( tr("image file: <double-click> to select an image file") );
    pImage->setImage( pSetting->getImage() );
    connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

    pHotSpotX = new QSpinBox( this );
    pHotSpotX->setMinimum( 0 );
    pHotSpotX->setMaximum( 100 );
    pHotSpotX->setValue( pSetting->getHotSpot().x() );
    pHotSpotX->setToolTip( tr("HotSpot x-coordinate.") );
    connect( pHotSpotX, SIGNAL(valueChanged(int)), SLOT(slotHotSpotX(int)) );

    pHotSpotY = new QSpinBox( this );
    pHotSpotY->setMinimum( 0 );
    pHotSpotY->setMaximum( 100 );
    pHotSpotY->setValue( pSetting->getHotSpot().y() );
    pHotSpotY->setToolTip( tr("HotSpot y-coordinate.") );
    connect( pHotSpotY, SIGNAL(valueChanged(int)), SLOT(slotHotSpotY(int)) );

    pCrossHairs = new QCheckBox( this );
    pCrossHairs->setTristate( false );
    pCrossHairs->setToolTip( tr("Enable cross-hairs.") );    
    pCrossHairs->setChecked( pSetting->getCrossHairs() );
    connect( pCrossHairs, SIGNAL(toggled(bool)), SLOT(slotCrossHairs(bool)) );

    pPen = new WPenToolBar( pSetting->getPen(), this );
    pPen->setPen( pSetting->getPen() );    
    connect( pPen, SIGNAL(signalChanged(const QPen &)), SLOT(slotPen(const QPen &)) );

    if ( n == Qt::Horizontal )
    {
        pCrossHairs->setText( tr("Cross-Hairs") );
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pStandard );
        pLayout->addWidget( pResource );
        pLayout->addWidget( pImage );
        pLayout->addWidget( pHotSpotX );
        pLayout->addWidget( pHotSpotY );
        pLayout->addWidget( pCrossHairs );
        pLayout->addWidget( pPen );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Standard"), pStandard );
        pLayout->addRow( tr("Resource"), pResource );
        pLayout->addRow( tr("File"), pImage );
        pLayout->addRow( tr("HotSpot X"), pHotSpotX );
        pLayout->addRow( tr("HotSpot Y"), pHotSpotY );
        pLayout->addRow( tr("Cross-Hairs"), pCrossHairs );
        pLayout->addRow( tr("Pen"), pPen );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSPointerWidget::slotRefresh()
{
    pStandard->setCurrentIndex( pStandard->findData( (int)pSetting->getStandard() ) );
    pResource->setCurrentIndex( pResource->findData( pSetting->getResource() ) );
    pImage->setImage( pSetting->getImage() );
    pHotSpotX->setValue( pSetting->getHotSpot().x() );
    pHotSpotY->setValue( pSetting->getHotSpot().y() );
    pCrossHairs->setChecked( pSetting->getCrossHairs() );
    pPen->setPen( pSetting->getPen() );    
}

void LSPointerWidget::slotStandard( int n )
{
    pSetting->setStandard( Qt::CursorShape(pStandard->itemData( n ).toInt()) );
}

void LSPointerWidget::slotResource( int n )
{
    pSetting->setResource( pResource->itemData( n ).toString() );
}

void LSPointerWidget::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    pSetting->setImage( image );
}

void LSPointerWidget::slotHotSpotX( int n )
{
    pSetting->setHotSpot( QPoint( n, pHotSpotY->value() ) );
}

void LSPointerWidget::slotHotSpotY( int n )
{
    pSetting->setHotSpot( QPoint( pHotSpotX->value(), n ) );
}

void LSPointerWidget::slotCrossHairs( bool b )
{
    pSetting->setCrossHairs( b );
}

void LSPointerWidget::slotPen( const QPen &t )
{
    pSetting->setPen( t );
}


