/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WPen.h"

/****************************************************************
 * WPenButton
 ****************************************************************/
WPenButton::WPenButton( const QPen &pen1, QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "WPenButton" );
    setToolTip( tr("configure pen") );
    pen = pen1;
    setIcon( getIcon() );
    connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
}

void WPenButton::slotRefresh( const QPen &pen1 )
{
    if ( pen1 == pen ) return;
    pen = pen1;
    setIcon( getIcon() );
}

void WPenButton::slotClicked()
{
    QPen pen1 = WPenDialog::doEdit( pen, this );
    if ( pen1 == pen ) return;
    pen = pen1;
    setIcon( getIcon() );
    emit signalChanged( pen );
}

QPixmap WPenButton::getIcon()
{
    QPixmap pixmap( ":W/Pen16" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), pen.color() );
    return pixmap;
}

/****************************************************************
 * WPenToolBar
 ****************************************************************/
WPenToolBar::WPenToolBar( const QPen &pen1, QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "WPenToolBar" );
    pen = pen1;
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pColor = new WColorButton( pen.color(), this, WColorButton::Pen );
    pLayout->addWidget( pColor );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );

    pStyle = new WLineStyleComboBox( pen.style(), this );
    pStyle->setToolTip( tr("pen style") );
    pLayout->addWidget( pStyle );
    connect( pStyle, SIGNAL(signalChanged(Qt::PenStyle)), SLOT(slotStyle(Qt::PenStyle)) );

    pWidth = new QSpinBox( this );
    pWidth->setToolTip( tr("pen width") );
    pWidth->setMinimum( 0 );
    pWidth->setMaximum( 100 );
    pWidth->setValue( pen.width() );
    pLayout->addWidget( pWidth );
    connect( pWidth, SIGNAL(valueChanged(int)), SLOT(slotWidth(int)) );

    pMore = new QToolButton( this );
    pMore->setToolTip( tr("more") );
    pMore->setText( ":" );
    pLayout->addWidget( pMore );
    connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

    pLayout->addStretch( 11 );
}

void WPenToolBar::slotRefresh( const QPen &pen1 )
{
    if ( pen1 == pen ) return;
    pen = pen1;
    pColor->setValue( pen.color() );                
    pStyle->setValue( pen.style() );                
    pWidth->setValue( pen.width() );
}

void WPenToolBar::slotColor( const QColor &color )
{
    pen.setColor( color );
    emit signalChanged( pen );
}

void WPenToolBar::slotStyle( Qt::PenStyle n )
{
    pen.setStyle( n );
    emit signalChanged( pen );
}

void WPenToolBar::slotWidth( int n )
{
    pen.setWidth( n );
    emit signalChanged( pen );
}

void WPenToolBar::slotMore()
{
    QPen pen1 = WPenDialog::doEdit( pen, this );
    if ( pen1 == pen ) return;
    pen = pen1;
    emit signalChanged( pen );
}

/****************************************************************
 * WPenPanel
 ****************************************************************/
WPenPanel::WPenPanel( const QPen &pen1, QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Pen") );
    pen = pen1;
    // style
    pStyle = new WLineStyleComboBox( pen.style(), this );
    connect( pStyle, SIGNAL(signalChanged(Qt::PenStyle)), SLOT(slotStyle(Qt::PenStyle)) );
    // width
    pWidth = new QSpinBox( this );
    pWidth->setMinimum( 0 );
    pWidth->setMaximum( 100 );
    pWidth->setValue( pen.width() );
    connect( pWidth, SIGNAL(valueChanged(int)), SLOT(slotWidth(int)) );
    // color
    pColor = new WColorButton( pen.color(), this, WColorButton::Pen );
    connect( pColor, SIGNAL(signalChanged(const QColor &)), SLOT(slotColor(const QColor &)) );
    // cap
    pCapStyle = new QComboBox( this );
    pCapStyle->addItem( "SquareCap", (int)Qt::SquareCap );
    pCapStyle->addItem( "FlatCap", (int)Qt::FlatCap );
    pCapStyle->addItem( "RoundCap", (int)Qt::RoundCap );
    WSetComboBox( pCapStyle, pen.capStyle() );
    connect( pCapStyle, SIGNAL(activated(int)), SLOT(slotCapStyle(int)) );
    // join
    pJoinStyle = new QComboBox( this );
    pJoinStyle->addItem( "BevelJoin", (int)Qt::BevelJoin );
    pJoinStyle->addItem( "MiterJoin", (int)Qt::MiterJoin );
    pJoinStyle->addItem( "RoundJoin", (int)Qt::RoundJoin );
    WSetComboBox( pJoinStyle, pen.joinStyle() );
    connect( pJoinStyle, SIGNAL(activated(int)), SLOT(slotJoinStyle(int)) );
    // miter
    pMiterLimit = new QSpinBox( this );
    pMiterLimit->setMinimum( 0 );
    pMiterLimit->setMaximum( 10 );
    pMiterLimit->setValue( pen.miterLimit() );
    connect( pMiterLimit, SIGNAL(valueChanged(int)), SLOT(slotMiterLimit(int)) );
    // stroke: color
    pColorBrush = new WColorButton( pen.brush().color(), this, WColorButton::Brush );
    pColorBrush->setToolTip( tr("stroke fill color") );
    connect( pColorBrush, SIGNAL(signalChanged(const QColor &)), SLOT(slotColorBrush(const QColor &)) );
    // stroke: style
    pStyleBrush = new WBrushStyleComboBox( this, pen.brush().style() );
    pStyleBrush->setToolTip( tr("stroke fill style (NoBrush for no stroke fill)") );
    connect( pStyleBrush, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyleBrush(Qt::BrushStyle)) );
    // stroke: texture
    pImageBrush = new WImageButton( this, pen.brush().textureImage() );
    connect( pImageBrush, SIGNAL(signalClick()), SLOT(slotImageBrush()) );

    // FORM
    QFormLayout *pLayoutForm = new QFormLayout( this );
    pLayoutForm->addRow( tr("Style:"), pStyle );
    pLayoutForm->addRow( tr("Width:"), pWidth );
    pLayoutForm->addRow( tr("Color:"), pColor );
    pLayoutForm->addRow( tr("Cap Style:"), pCapStyle );
    pLayoutForm->addRow( tr("Join Style:"), pJoinStyle );
    pLayoutForm->addRow( tr("Miter Limit:"), pMiterLimit );
    pLayoutForm->addRow( tr("Stroke Color:"), pColorBrush );
    pLayoutForm->addRow( tr("Stroke Style:"), pStyleBrush );
    pLayoutForm->addRow( tr("Stroke Texture:"), pImageBrush );
}

void WPenPanel::slotRefresh( const QPen &pen1 )
{
    if ( pen1 == pen ) return;
    pen = pen1;
    pStyle->setValue( pen.style() );
    pWidth->setValue( pen.width() );
    pColor->setValue( pen.color() );
    WSetComboBox( pCapStyle, pen.capStyle() );
    WSetComboBox( pJoinStyle, pen.joinStyle() );
    pMiterLimit->setValue( pen.miterLimit() );
    pColorBrush->setValue( pen.brush().color() );
    pStyleBrush->setValue( pen.brush().style() );
    pImageBrush->setImage( pen.brush().textureImage() );
}

void WPenPanel::slotCapStyle( int nIndex )
{
    if ( pCapStyle->itemData( nIndex ).isNull() ) return;
    pen.setCapStyle( (Qt::PenCapStyle)(pCapStyle->itemData( nIndex ).toInt()) );
    emit signalChanged( pen );
}

void WPenPanel::slotColor( const QColor &color )
{
    pen.setColor( color );
    emit signalChanged( pen );
}

void WPenPanel::slotJoinStyle( int nIndex )
{
    if ( pJoinStyle->itemData( nIndex ).isNull() ) return;
    pen.setJoinStyle( (Qt::PenJoinStyle)(pJoinStyle->itemData( nIndex ).toInt()) );
    emit signalChanged( pen );
}

void WPenPanel::slotMiterLimit( int n )
{
    pen.setMiterLimit( n );
    emit signalChanged( pen );
}

void WPenPanel::slotStyle( Qt::PenStyle n )
{
    pen.setStyle( n );
    emit signalChanged( pen );
}

void WPenPanel::slotWidth( int n )
{
    pen.setWidth( n );
    emit signalChanged( pen );
}

void WPenPanel::slotColorBrush( const QColor &color )
{
    QBrush brush = pen.brush();
    brush.setColor( color );
    pen.setBrush( brush );
    emit signalChanged( pen );
}

void WPenPanel::slotStyleBrush( Qt::BrushStyle n )
{
    QBrush brush = pen.brush();
    brush.setStyle( n );
    pen.setBrush( brush );
    emit signalChanged( pen );
}

void WPenPanel::slotImageBrush()
{
    QImage image;
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "*.png" );
    if ( !stringFileName.isEmpty() ) image.load( stringFileName );

    QBrush brush = pen.brush();
    brush.setTextureImage( image );
    pen.setBrush( brush );
    emit signalChanged( pen );
}

/****************************************************************
 * WPenSwatch
 ****************************************************************/
WPenSwatch::WPenSwatch( const QPen &pen1, QWidget *pParent )
    : QWidget( pParent )
{
    pen = pen1;
}

void WPenSwatch::slotRefresh( const QPen &pen1 )
{
    if ( pen1 == pen ) return;
    pen = pen1;
    update();
}

void WPenSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setPen( pen );
    painter.fillRect( 0, 0, width(), height(), pen.color().lighter() );
    painter.drawLine( 0, height()/2, width(), height()/2 );
}

QSize WPenSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}

/****************************************************************
 * WPenDialog
 ****************************************************************/
WPenDialog::WPenDialog( const QPen &pen, QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Text") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pPanel = new WPenPanel( pen, this );
    pLayout->addWidget( pPanel );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this );
    pLayout->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    connect( pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
}

QPen WPenDialog::doEdit( const QPen &pen, QWidget *pParent )
{
    WPenDialog dialog( pen, pParent );
    if ( dialog.exec() == QDialog::Accepted )
        return dialog.getPen();
    return pen;
}


