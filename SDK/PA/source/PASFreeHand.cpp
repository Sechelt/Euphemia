/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASFreeHand.h"

#include "SSettings.h"

PASFreeHand::PASFreeHand()
{
}

void PASFreeHand::setShape( Shapes n )
{
    if ( n == nShape ) return;
    nShape = n;
    emit signalChanged();
}

void PASFreeHand::setSize( const QSize &t )
{
    if ( t == size ) return;
    size = t;
    emit signalChanged();
}

void PASFreeHand::setImage( const QImage &t )
{                                          
    if ( t == image ) return;              
    image = t;                             
    emit signalChanged();                  
}                                          
                                           
void PASFreeHand::setLineTo( bool t )
{                                          
    if ( t == bLineTo ) return;              
    bLineTo = t;                             
    emit signalChanged();                  
}                                          
                                           
void PASFreeHand::setSelected( Shapes nShape, const QSize &size, const QImage &image, bool bLineTo )
{
    this->nShape    = nShape;
    this->size      = size;
    this->image     = image;
    this->bLineTo   = bLineTo;
    emit signalSync();
}

QWidget *PASFreeHand::getToolBar( QWidget *pParent )
{
    return new PASFreeHandToolBar( pParent );
}

QWidget *PASFreeHand::getPanel( QWidget *pParent )
{
    return new PASFreeHandPanel( pParent );
}

void PASFreeHand::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASFreeHand" );

    domElem.setAttribute( "Shape", nShape );
    domElem.setAttribute( "Width", size.width() );
    domElem.setAttribute( "Height", size.height() );
    if ( !image.isNull() ) domElem.appendChild( CBDPersistNative::doSaveImage( image, pdomDoc, "Image" ) );

    pdomElem->appendChild( domElem );
}

void PASFreeHand::doLoad( QDomElement *pdomElem )
{
    nShape = PASFreeHand::Shapes(pdomElem->attribute( "Shape", QString::number( nShape ) ).toInt());
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
 * PASFreeHandToolBar
 ****************************************************************/
PASFreeHandToolBar::PASFreeHandToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASFreeHand*)g_SSettings->getSetting( "PASFreeHand" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [PASFreeHand].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pShape = new QComboBox( this );
    pShape->addItem( "Pen", PASFreeHand::ShapePen );
    pShape->addItem( "Ellipse", PASFreeHand::ShapeEllipse );
    pShape->addItem( "Rectangle", PASFreeHand::ShapeRectangle );
    pShape->addItem( "Cross", PASFreeHand::ShapeCross );
    pShape->addItem( "ImageScaled", PASFreeHand::ShapeImageScaled );
    pShape->addItem( "Image", PASFreeHand::ShapeImage );
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

    pLineTo = new QCheckBox( tr("LineTo"), this );
    pLineTo->setToolTip( tr("draw a line between points or just points") );
    pLineTo->setCheckState( (Qt::CheckState)pSetting->getLineTo() );
    pLayout->addWidget( pLineTo, 10 );
    connect( pLineTo, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotLineTo(Qt::CheckState)) );

    pLayout->addStretch( 16 );

    pWidth->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pLineTo->setVisible( pSetting->getShape() == PASFreeHand::ShapePen );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASFreeHandToolBar::slotRefresh()
{
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pWidth->setValue( pSetting->getSize().width() );
    pHeight->setValue( pSetting->getSize().height() );
    pImage->setImage( pSetting->getImage() );
    pLineTo->setChecked( pSetting->getLineTo() );

    pWidth->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pHeight->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pImage->setVisible( pSetting->getShape() != PASFreeHand::ShapePen );
    pLineTo->setVisible( pSetting->getShape() == PASFreeHand::ShapePen );
}

void PASFreeHandToolBar::slotShape( int n )
{
    pSetting->setShape( (PASFreeHand::Shapes)pShape->itemData( n ).toInt() );
}

void PASFreeHandToolBar::slotWidth( int n )
{
    QSize size = pSetting->getSize();
    size.setWidth( n );
    pSetting->setSize( size );
}

void PASFreeHandToolBar::slotHeight( int n )
{
    QSize size = pSetting->getSize();
    size.setHeight( n );
    pSetting->setSize( size );
}

void PASFreeHandToolBar::slotImage()
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

void PASFreeHandToolBar::slotLineTo( Qt::CheckState n )
{
    pSetting->setLineTo( n );
}

/****************************************************************
 * PASFreeHandPanel
 ****************************************************************/
PASFreeHandPanel::PASFreeHandPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASFreeHand*)g_SSettings->getSetting( "PASFreeHand" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [PASFreeHand].";
        return;
    }

    // shape
    pShape = new QComboBox( this );
    pShape->addItem( "Pen", PASFreeHand::ShapePen );
    pShape->addItem( "Ellipse", PASFreeHand::ShapeEllipse );
    pShape->addItem( "Rectangle", PASFreeHand::ShapeRectangle );
    pShape->addItem( "Cross", PASFreeHand::ShapeCross );
    pShape->addItem( "ImageScaled", PASFreeHand::ShapeImageScaled );
    pShape->addItem( "Image", PASFreeHand::ShapeImage );
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
    // line to
    pLineTo = new QCheckBox( this );
    pLineTo->setToolTip( tr("draw a line between points or just points") );
    pLineTo->setCheckState( (Qt::CheckState)pSetting->getLineTo() );
    connect( pLineTo, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotLineTo(Qt::CheckState)) );
    // layout
    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Shape"), pShape );
    pLayout->addRow( tr("Width"), pWidth );
    pLayout->addRow( tr("Height"), pHeight );
    pLayout->addRow( tr("Image"), pImage );
    pLayout->addRow( tr("LineTo"), pLineTo );

    pWidth->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pHeight->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pImage->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pLineTo->setEnabled( pSetting->getShape() == PASFreeHand::ShapePen );

    // refresh
    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASFreeHandPanel::slotRefresh()
{
    pShape->setCurrentIndex( pShape->findData( (int)pSetting->getShape() ) );
    pWidth->setValue( pSetting->getSize().width() );
    pHeight->setValue( pSetting->getSize().height() );
    pImage->setImage( pSetting->getImage() );
    pLineTo->setChecked( pSetting->getLineTo() );

    pWidth->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pHeight->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pImage->setEnabled( pSetting->getShape() != PASFreeHand::ShapePen );
    pLineTo->setEnabled( pSetting->getShape() == PASFreeHand::ShapePen );
}

void PASFreeHandPanel::slotShape( int n )
{
    pSetting->setShape( (PASFreeHand::Shapes)pShape->itemData( n ).toInt() );
}

void PASFreeHandPanel::slotWidth( int n )
{
    QSize size = pSetting->getSize();
    size.setWidth( n );
    pSetting->setSize( size );
}

void PASFreeHandPanel::slotHeight( int n )
{
    QSize size = pSetting->getSize();
    size.setHeight( n );
    pSetting->setSize( size );
}

void PASFreeHandPanel::slotImage()
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

void PASFreeHandPanel::slotLineTo( Qt::CheckState n )
{
    pSetting->setLineTo( (bool)n );
}


