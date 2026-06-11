/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WBrush.h"

/****************************************************************
 * WBrushButton
 ****************************************************************/
WBrushButton::WBrushButton( const QBrush &brush1, QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "WBrushButton" );
    setToolTip( tr("configure brush") );
    brush = brush1;
    setIcon( getIcon() );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void WBrushButton::slotRefresh( const QBrush &brush1 )
{
    if ( brush1 == brush ) return;
    brush = brush1;
    setIcon( getIcon() );
}

void WBrushButton::slotClicked()
{
    QBrush brush1 = WBrushDialog::doEdit( brush, this );
    if ( brush1 == brush ) return;
    brush = brush1;
    setIcon( getIcon() );
    emit signalChanged( brush );
}

QPixmap WBrushButton::getIcon()
{
    QPixmap pixmap( ":W/Brush" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), brush.color() );
    return pixmap;
}

/****************************************************************
 * WBrushToolBar
 ****************************************************************/
WBrushToolBar::WBrushToolBar( const QBrush &brush1, QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "WBrushToolBar" );
    brush = brush1;
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pColor = new WColorButton( brush.color(), this, WColorButton::Fill );
    pLayout->addWidget( pColor );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );

    pStyle = new WBrushStyleComboBox( this, brush.style() );
    pStyle->setToolTip( tr("brush style") );
    pLayout->addWidget( pStyle );
    connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

    pImage = new WImageButton( this, brush.textureImage() );
    pImage->setToolTip( tr("double-click to set texture") );
    pLayout->addWidget( pImage, 10 );
    connect( pImage, SIGNAL(signalClick()), SLOT(slotImage()) );

    pMore = new QToolButton( this );
    pMore->setToolTip( tr("more") );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 10 );
}

void WBrushToolBar::slotRefresh( const QBrush &brush1 )
{
    if ( brush1 == brush ) return;
    brush = brush1;
    if ( pColor ) pColor->setValue( brush.color() );
    pStyle->setValue( brush.style() );
    if ( pImage ) pImage->setImage( brush.textureImage() );
}

void WBrushToolBar::slotColor( const QColor &color )
{
    brush.setColor( color );
    emit signalChanged( brush );
}

void WBrushToolBar::slotStyle( Qt::BrushStyle n )
{
    brush.setStyle( n );
    emit signalChanged( brush );
}

void WBrushToolBar::slotImage()
{
    QImage image;

    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "Image files (*.png *.xpm *.jpg)" );
    if ( !stringFileName.isEmpty() )
    {
        image.load( stringFileName );
    }

    // accept null or loaded image
    brush.setTextureImage( image );
    emit signalChanged( brush );
}

void WBrushToolBar::slotMore()
{
    QBrush brush1 = WBrushDialog::doEdit( brush, this );
    if ( brush1 == brush ) return;
    brush = brush1;
    emit signalChanged( brush );
}

/****************************************************************
 * WBrushPanel
 ****************************************************************/
WBrushPanel::WBrushPanel( const QBrush &brush1, QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Brush") );
    brush = brush1;
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
}

void WBrushPanel::slotRefresh( const QBrush &brush1 )
{
    if ( brush1 == brush ) return;
    brush = brush1;
    pColor->setValue( brush.color() );
    pStyleComboBox->setValue( brush.style() );
    pImage->setImage( brush.textureImage() );
}

void WBrushPanel::slotColor( QColor color )
{
    brush.setColor( color );
    emit signalChanged( brush );
}

void WBrushPanel::slotStyle( Qt::BrushStyle n )
{
    brush.setStyle( n );
    emit signalChanged( brush );
}

void WBrushPanel::slotImage()
{
    QImage image;
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "*.png" );
    if ( !stringFileName.isEmpty() ) image.load( stringFileName );
    brush.setTextureImage( image );
    emit signalChanged( brush );
}

/****************************************************************
 * WBrushSwatch
 ****************************************************************/
WBrushSwatch::WBrushSwatch( const QBrush &brush1, QWidget *pParent )
    : QWidget( pParent )
{
    brush = brush1;
}

void WBrushSwatch::slotRefresh( const QBrush &brush1 )
{
    if ( brush1 == brush ) return;
    brush = brush1;
    update();
}

void WBrushSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter(this);
    painter.fillRect( 0, 0, width(), height(), brush );
}

QSize WBrushSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}

/****************************************************************
 * WBrushDialog
 ****************************************************************/
WBrushDialog::WBrushDialog( const QBrush &brush, QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Bush") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pPanel = new WBrushPanel( brush, this );
    pLayout->addWidget( pPanel );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
}

QBrush WBrushDialog::doEdit( const QBrush &brush, QWidget *pParent )
{
    WBrushDialog dialog( brush, pParent );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getBrush();
    return brush;
}

