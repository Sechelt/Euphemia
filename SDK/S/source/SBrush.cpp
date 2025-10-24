/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "SBrush.h"

/****************************************************************
 * SBrush
 ****************************************************************/
SBrush::SBrush()
{
}

void SBrush::setValue( const QBrush &t )
{
    if ( t == brush ) return;
    brush = t;
    emit signalChanged();
}

void SBrush::setSelected( const QBrush &t )
{
    if ( t == brush ) return;
    brush = t;
    emit signalSync();
}

QWidget *SBrush::getButton( QWidget *pParent )
{
    return new SBrushButton( pParent );
}

QWidget *SBrush::getToolBar( QWidget *pParent )
{
    return new SBrushToolBar( pParent );
}

QWidget *SBrush::getPanel( QWidget *pParent )
{
    return new SBrushPanel( pParent );
}

QWidget *SBrush::getSwatch( QWidget *pParent )
{
    return new SBrushSwatch( pParent );
}

void SBrush::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "SBrush" );

    domElemThis.appendChild( CBDPersistNative::doSaveBrush( brush, pdomDoc ) );

    pdomElemParent->appendChild( domElemThis );
}

void SBrush::doLoad( QDomElement *pdomElemThis )
{
    // load child nodes...
    brush = QBrush();

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

        if ( domElem.tagName() == "Brush" ) brush = CBDPersistNative::doLoadBrush( &domElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * SBrushButton
 ****************************************************************/
SBrushButton::SBrushButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "SBrushButton" );

    pBrush = (SBrush*)g_SSettings->getSetting( "SBrush" );
    if ( !pBrush )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SBrush].";
        Q_ASSERT( pBrush );
    }

    setIcon( getIcon() );

    connect( pBrush, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pBrush, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void SBrushButton::slotRefresh()
{
    setIcon( getIcon() );
}

void SBrushButton::slotClicked()
{
    SDialog dialog( this, "SBrush", "Brush" );
    dialog.exec();
}

QPixmap SBrushButton::getIcon()
{
    QPixmap pixmap( ":W/Brush" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), pBrush->getValue().color() );
    return pixmap;
}

/****************************************************************
 * SBrushToolBar
 ****************************************************************/
SBrushToolBar::SBrushToolBar( QWidget *pParent, bool bCompress )
    : QWidget( pParent )
{
    setObjectName( "SBrushToolBar" );

    pSettingsBrush = (SBrush*)g_SSettings->getSetting( "SBrush" );
    if ( pSettingsBrush )
    {
        brush    = pSettingsBrush->getValue();

        QHBoxLayout *pLayout = new QHBoxLayout( this );

        if ( !bCompress )
        {
            pColor = new WColorButton( brush.color(), this, WColorButton::Fill );
            pLayout->addWidget( pColor );
            connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );
        }

        pStyle = new WBrushStyleComboBox( this, brush.style() );
        pStyle->setToolTip( tr("brush style") );
        pLayout->addWidget( pStyle );
        connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

        if ( !bCompress )
        {
            pImage = new WImageButton( this, brush.textureImage() );
            pImage->setToolTip( tr("double-click to set texture") );
            pLayout->addWidget( pImage, 10 );
            connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );
        }

        pMore = new QToolButton( this );
        pMore->setToolTip( tr("more") );
        pMore->setText( ":" );
        pLayout->addWidget( pMore );
        connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

        pLayout->addStretch( 10 );

        connect( pSettingsBrush, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pSettingsBrush, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SBrush].";
    }
}

void SBrushToolBar::slotRefresh()
{
    brush = pSettingsBrush->getValue();
    if ( pColor ) pColor->setValue( brush.color() );
    pStyle->setValue( brush.style() );
    if ( pImage ) pImage->setImage( brush.textureImage() );
}

void SBrushToolBar::slotColor( const QColor &color )
{
    brush.setColor( color );
    pSettingsBrush->setValue( brush );
}

void SBrushToolBar::slotStyle( Qt::BrushStyle n )
{
    brush.setStyle( n );
    pSettingsBrush->setValue( brush );
}

void SBrushToolBar::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    brush.setTextureImage( image );
    pSettingsBrush->setValue( brush );
}

void SBrushToolBar::slotMore()
{
    SDialog dialog( this, "SBrush", tr("Brush") );
    dialog.exec();
}

/****************************************************************
 * SBrushPanel
 ****************************************************************/
SBrushPanel::SBrushPanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Brush") );

    pSettingsBrush = (SBrush*)g_SSettings->getSetting( "SBrush" );
    if ( pSettingsBrush )
    {
        brush    = pSettingsBrush->getValue();

        QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

        // form
        {
            QFormLayout *pLayoutForm = new QFormLayout();
            pLayoutForm->addRow( tr("Color"), pColor = new WColorButton( brush.color(), this, WColorButton::Brush ) );
            pLayoutForm->addRow( tr("Style"), pStyleComboBox = new WBrushStyleComboBox( this, brush.style() ) );
            pLayoutForm->addRow( tr("Texture"), pImage = new WImageButton( this, brush.textureImage() ) );

            connect( pColor, SIGNAL(signalChanged(QColor)), SLOT(slotColor(QColor)) );
            connect( pStyleComboBox, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );
            connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

            pLayoutTop->addLayout( pLayoutForm );
        }

        connect( pSettingsBrush, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pSettingsBrush, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SBrush].";
    }
}

void SBrushPanel::slotRefresh()
{
    brush = pSettingsBrush->getValue();
    pColor->setValue( brush.color() );
    pStyleComboBox->setValue( brush.style() );
    pImage->setImage( brush.textureImage() );
}

void SBrushPanel::slotColor( QColor color )
{
    brush.setColor( color );
    pSettingsBrush->setValue( brush );
}

void SBrushPanel::slotStyle( Qt::BrushStyle n )
{
    brush.setStyle( n );
    pSettingsBrush->setValue( brush );
}

void SBrushPanel::slotImage()
{
    QImage image;
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "*.png" );
    if ( !stringFileName.isEmpty() ) image.load( stringFileName );
    brush.setTextureImage( image );
    pSettingsBrush->setValue( brush );
}

/****************************************************************
 * SBrushSwatch
 ****************************************************************/
SBrushSwatch::SBrushSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pSettingsBrush = (SBrush*)g_SSettings->getSetting( "SBrush" );
    if ( pSettingsBrush )
    {
        brush    = pSettingsBrush->getValue();
        connect( pSettingsBrush, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pSettingsBrush, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SBrush].";
    }
}

void SBrushSwatch::slotRefresh()
{
    brush = pSettingsBrush->getValue();
    update();
}

void SBrushSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter(this);
    painter.fillRect( 0, 0, width(), height(), brush );
}

QSize SBrushSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}



