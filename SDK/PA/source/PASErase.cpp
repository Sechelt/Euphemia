/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASErase.h"

#include "SSettings.h"

/****************************************************************
 * PASErase
 ****************************************************************/
PASErase::PASErase()
{
}

void PASErase::setShape( Shapes n )
{
    if ( n == nShape ) return;
    nShape = n;
    emit signalChanged();
}

void PASErase::setSize( const QSize &t )
{
    if ( t == size ) return;
    size = t;
    emit signalChanged();
}

void PASErase::setImage( const QImage &t )
{
    if ( t == image ) return;
    image = t;
    emit signalChanged();
}

void PASErase::setSelected( Shapes nShape, const QSize &size, const QImage &image )
{
    this->nShape    = nShape;
    this->size      = size;
    this->image     = image;
    emit signalSync();
}

QWidget *PASErase::getToolBar( QWidget *pParent )
{
    return new PASEraseToolBar( pParent );
}

QWidget *PASErase::getPanel( QWidget *pParent )
{
    return new PASErasePanel( pParent );
}

void PASErase::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASErase" );

    domElem.setAttribute( "Shape", nShape );
    domElem.setAttribute( "Width", size.width() );
    domElem.setAttribute( "Height", size.height() );
    if ( !image.isNull() ) domElem.appendChild( CBDPersistNative::doSaveImage( image, pdomDoc, "Image" ) );

    pdomElem->appendChild( domElem );
}

void PASErase::doLoad( QDomElement *pdomElem )
{
    nShape = PASErase::Shapes(pdomElem->attribute( "Shape", QString::number( int(nShape) ) ).toInt());
    size.setWidth( pdomElem->attribute( "Width", QString::number( size.width() ) ).toInt() );
    size.setHeight( pdomElem->attribute( "Height", QString::number( size.height() ) ).toInt() );

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

        if ( domElem.tagName() == "Image" ) image = CBDPersistNative::doLoadImage( pdomElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * PASEraseToolBar
 ****************************************************************/
PASEraseToolBar::PASEraseToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASErase*)g_SSettings->getSetting( "PASErase" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [PASErase].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pShape = new QComboBox( this );
    pShape->addItem( "Pen", PASErase::ShapePen );
    pShape->addItem( "Ellipse", PASErase::ShapeEllipse );
    pShape->addItem( "Rectangle", PASErase::ShapeRectangle );
    pShape->addItem( "Cross", PASErase::ShapeCross );
    pShape->addItem( "ImageScaled", PASErase::ShapeImageScaled );
    pShape->addItem( "Image", PASErase::ShapeImage );
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pShape->setToolTip( tr("shape") );
    pLayout->addWidget( pShape );
    connect( pShape, SIGNAL(currentIndexChanged(int)), SLOT(slotShape(int)) );
    //
    pWidth = new QSpinBox( this );
    pWidth->setMinimum( 1 );
    pWidth->setMaximum( 50 );
    pWidth->setValue( pSetting->getSize().width() );
    pWidth->setToolTip( tr("width") );
    pLayout->addWidget( pWidth );
    connect( pWidth, SIGNAL(valueChanged(int)), SLOT(slotWidth(int)) );
    //
    pHeight = new QSpinBox( this );
    pHeight->setMinimum( 1 );
    pHeight->setMaximum( 50 );
    pHeight->setValue( pSetting->getSize().height() );
    pHeight->setToolTip( tr("height") );
    pLayout->addWidget( pHeight );
    connect( pHeight, SIGNAL(valueChanged(int)), SLOT(slotHeight(int)) );

    pImage = new WImageButton( this, pSetting->getImage() );
    pImage->setToolTip( tr("image (double click to change)") );
    pLayout->addWidget( pImage, 10 );
    connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

    pLayout->addStretch( 16 );

    pWidth->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASErase::ShapePen );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASEraseToolBar::slotRefresh()
{
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pWidth->setValue( pSetting->getSize().width() );
    pHeight->setValue( pSetting->getSize().height() );
    pImage->setImage( pSetting->getImage() );

    pWidth->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASErase::ShapePen );
}

void PASEraseToolBar::slotShape( int n )
{
    pSetting->setShape( (PASErase::Shapes)pShape->itemData( n ).toInt() );
}

void PASEraseToolBar::slotWidth( int n )
{
    QSize size = pSetting->getSize();
    size.setWidth( n );
    pSetting->setSize( size );
}

void PASEraseToolBar::slotHeight( int n )
{
    QSize size = pSetting->getSize();
    size.setHeight( n );
    pSetting->setSize( size );
}

void PASEraseToolBar::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    //
    pSetting->setImage( image );
}

/****************************************************************
 * PASErasePanel
 ****************************************************************/
PASErasePanel::PASErasePanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASErase*)g_SSettings->getSetting( "PASErase" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [PASErase].";
        return;
    }

    // shape
    pShape = new QComboBox( this );
    pShape->addItem( "Pen", PASErase::ShapePen );
    pShape->addItem( "Ellipse", PASErase::ShapeEllipse );
    pShape->addItem( "Rectangle", PASErase::ShapeRectangle );
    pShape->addItem( "Cross", PASErase::ShapeCross );
    pShape->addItem( "ImageScaled", PASErase::ShapeImageScaled );
    pShape->addItem( "Image", PASErase::ShapeImage );
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pShape->setToolTip( tr("shape") );
    connect( pShape, SIGNAL(currentIndexChanged(int)), SLOT(slotShape(int)) );
    // width
    pWidth = new QSpinBox( this );
    pWidth->setMinimum( 1 );
    pWidth->setMaximum( 50 );
    pWidth->setValue( pSetting->getSize().width() );
    pWidth->setToolTip( tr("width") );
    connect( pWidth, SIGNAL(valueChanged(int)), SLOT(slotWidth(int)) );
    // height
    pHeight = new QSpinBox( this );
    pHeight->setMinimum( 1 );
    pHeight->setMaximum( 50 );
    pHeight->setValue( pSetting->getSize().height() );
    pHeight->setToolTip( tr("height") );
    connect( pHeight, SIGNAL(valueChanged(int)), SLOT(slotHeight(int)) );
    // image
    pImage = new WImageButton( this, pSetting->getImage() );
    pImage->setToolTip( tr("image (double click to change)") );
    connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

    // layout
    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Shape"), pShape );
    pLayout->addRow( tr("Width"), pWidth );
    pLayout->addRow( tr("Height"), pHeight );
    pLayout->addRow( tr("Image"), pImage );

    pWidth->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASErase::ShapePen );

    // refresh
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASErasePanel::slotRefresh()
{
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pWidth->setValue( pSetting->getSize().width() );
    pHeight->setValue( pSetting->getSize().height() );
    pImage->setImage( pSetting->getImage() );

    pWidth->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASErase::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASErase::ShapePen );
}

void PASErasePanel::slotShape( int n )
{
    pSetting->setShape( (PASErase::Shapes)pShape->itemData( n ).toInt() );
}

void PASErasePanel::slotWidth( int n )
{
    QSize size = pSetting->getSize();
    size.setWidth( n );
    pSetting->setSize( size );
}

void PASErasePanel::slotHeight( int n )
{
    QSize size = pSetting->getSize();
    size.setHeight( n );
    pSetting->setSize( size );
}

void PASErasePanel::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    //
    pSetting->setImage( image );
}

