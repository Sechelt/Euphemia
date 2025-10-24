/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LSBackground.h"

/****************************************************************
 * LSBackground
 ****************************************************************/
LSBackground::LSBackground()
{
    // default to useing checkered pattern to indicate transparency
    brushTransparency.setTextureImage( QImage( ":W/Transparent" ) );
}

void LSBackground::setTransparency( const QBrush &t )
{
    if ( brushTransparency == t ) return;
    brushTransparency = t;
    emit signalChanged();
}

void LSBackground::setSelected( const QBrush &t )
{
    if ( brushTransparency == t ) return;
    brushTransparency = t;
    emit signalSync();
}

QWidget *LSBackground::getToolBar( QWidget *pParent )
{
    return new LSBackgroundWidget( pParent, Qt::Horizontal );
}

QWidget *LSBackground::getPanel( QWidget *pParent )
{
    return new LSBackgroundWidget( pParent, Qt::Vertical );
}

void LSBackground::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSBackground" );
    domElem.appendChild( CBDPersistNative::doSaveBrush( brushTransparency, pdomDoc, "BrushTransparency" ) );
    pdomElem->appendChild( domElem );
}

void LSBackground::doLoad( QDomElement *pdomElem )
{
    // clear to get rid of any texture
    brushTransparency = QBrush();

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

        if ( domElem.tagName() == "BrushTransparency" ) brushTransparency = CBDPersistNative::doLoadBrush( &domElem );

        domNode = domNode.nextSibling();
    }
}

LSBackgroundWidget::LSBackgroundWidget( QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "LSBackgroundWidget" );

    pSetting = (LSBackground*)g_SSettings->getSetting( "LSBackground" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSBackground].";
        return;
    }

    QBrush brush = pSetting->getTransparency();
    if ( brush.style() == Qt::NoBrush ) brush.setTextureImage( QImage(":W/Transparent") );

    pColor = new WColorButton( brush.color(), this, WColorButton::Fill );
    pColor->setToolTip( tr("brush color to represent transparency") );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );

    pStyle = new WBrushStyleComboBox( this, brush.style() );
    pStyle->setToolTip( tr("brush style to represent transparency") );
    connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

    pImage = new WImageButton( this, brush.textureImage() );
    pImage->setToolTip( tr("brush texture to represent transparency") );
    connect( pImage, SIGNAL(signalClick()), this, SLOT(slotImage()) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pColor );
        pLayout->addWidget( pStyle );
        pLayout->addWidget( pImage );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Transparency Color"), pColor );
        pLayout->addRow( tr("Transparency Style"), pStyle );
        pLayout->addRow( tr("Transparency Texture"), pImage );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSBackgroundWidget::slotRefresh()
{
    QBrush brush = pSetting->getTransparency();
    if ( brush.style() == Qt::NoBrush ) brush.setTextureImage( QImage(":W/Transparent") );

    pColor->setValue( brush.color() );
    pStyle->setValue( brush.style() );
    pImage->setImage( brush.textureImage() );
}

void LSBackgroundWidget::slotColor( const QColor &color )
{
    QBrush brush = pSetting->getTransparency();
    brush.setColor( color );
    pSetting->setTransparency( brush );
}

void LSBackgroundWidget::slotStyle( Qt::BrushStyle n )
{
    QBrush brush = pSetting->getTransparency();
    brush.setStyle( n );
    pSetting->setTransparency( brush );
}

void LSBackgroundWidget::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    QBrush brush = pSetting->getTransparency();
    brush.setTextureImage( image );
    pSetting->setTransparency( brush );
}


