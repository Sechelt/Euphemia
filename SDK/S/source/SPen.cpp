/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SPen.h"

/****************************************************************
 * SPen
 ****************************************************************/
SPen::SPen()
{
}

void SPen::setValue( const QPen &t )
{
    if ( t == pen ) return;
    pen = t;
    emit signalChanged();
}

void SPen::setSelected( const QPen &t )
{
    if ( t == pen ) return;
    pen = t;
    emit signalSync();
}

QWidget *SPen::getButton( QWidget *pParent )
{
    return new SPenButton( pParent );
}

QWidget *SPen::getToolBar( QWidget *pParent )
{
    return new SPenToolBar( pParent );
}

QWidget *SPen::getPanel( QWidget *pParent )
{
    return new SPenPanel( pParent );
}

QWidget *SPen::getSwatch( QWidget *pParent )
{
    return new SPenSwatch( pParent );
}

void SPen::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "SPen" );

    domElemThis.appendChild( CBDPersistNative::doSavePen( pen, pdomDoc ) );

    pdomElemParent->appendChild( domElemThis );
}

void SPen::doLoad( QDomElement *pdomElemThis )
{
    // load child nodes...
    pen  = QPen();

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

        if ( domElem.tagName() == "Pen" ) pen = CBDPersistNative::doLoadPen( &domElem );

        domNode = domNode.nextSibling();
    }
}

/****************************************************************
 * SPenButton
 ****************************************************************/
SPenButton::SPenButton( QWidget *pParent )
    : QToolButton( pParent )
{
    setObjectName( "SPenButton" );

    pPen = (SPen*)g_SSettings->getSetting( "SPen" );
    if ( pPen )
    {
        setIcon( getIcon() );

        connect( pPen, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pPen, SIGNAL(signalSync()), SLOT(slotRefresh()) );
        connect( this, SIGNAL(clicked()), SLOT(slotClicked()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SPen].";
    }
}

void SPenButton::slotRefresh()
{
    setIcon( getIcon() );
}

void SPenButton::slotClicked()
{
    SDialog dialog( this, "SPen", "Pen" );
    dialog.exec();
}

QPixmap SPenButton::getIcon()
{
    QPixmap pixmap( ":W/Pen16" );
    QPainter p( &pixmap );
    p.fillRect( QRect( 0, 13, 16, 5 ), pPen->getValue().color() );
    return pixmap;
}

/****************************************************************
 * SPenToolBar
 ****************************************************************/
SPenToolBar::SPenToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "SPenToolBar" );

    pPen = (SPen*)g_SSettings->getSetting( "SPen" );
    if ( pPen )
    {
        pen = pPen->getValue();

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

        connect( pPen, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pPen, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SPen].";
    }
}

void SPenToolBar::slotRefresh()
{
    pen = pPen->getValue();
    if ( pColor ) pColor->setValue( pen.color() );                
    pStyle->setValue( pen.style() );                
    pWidth->setValue( pen.width() );
}

void SPenToolBar::slotColor( const QColor &color )
{
    pen.setColor( color );
    pPen->setValue( pen );
}

void SPenToolBar::slotStyle( Qt::PenStyle n )
{
    pen.setStyle( n );
    pPen->setValue( pen );
}

void SPenToolBar::slotWidth( int n )
{
    pen.setWidth( n );
    pPen->setValue( pen );
}

void SPenToolBar::slotMore()
{
    SDialog dialog( this, "SPen", "Pen" );
    dialog.exec();
}

/****************************************************************
 * SPenPanel
 ****************************************************************/
SPenPanel::SPenPanel( QWidget *pParent )
    : QWidget( pParent )
{
    setWindowTitle( tr("Pen") );

    pPen = (SPen*)g_SSettings->getSetting( "SPen" );
    if ( pPen )
    {
        pen = pPen->getValue();

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

        connect( pPen, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pPen, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SPen].";
    }
}

void SPenPanel::slotRefresh()
{
    pen = pPen->getValue();
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

void SPenPanel::slotCapStyle( int nIndex )
{
    if ( pCapStyle->itemData( nIndex ).isNull() ) return;
    pen.setCapStyle( (Qt::PenCapStyle)(pCapStyle->itemData( nIndex ).toInt()) );
    pPen->setValue( pen );
}

void SPenPanel::slotColor( const QColor &color )
{
    pen.setColor( color );
    pPen->setValue( pen );
}

void SPenPanel::slotJoinStyle( int nIndex )
{
    if ( pJoinStyle->itemData( nIndex ).isNull() ) return;
    pen.setJoinStyle( (Qt::PenJoinStyle)(pJoinStyle->itemData( nIndex ).toInt()) );
    pPen->setValue( pen );
}

void SPenPanel::slotMiterLimit( int n )
{
    pen.setMiterLimit( n );
    pPen->setValue( pen );
}

void SPenPanel::slotStyle( Qt::PenStyle n )
{
    pen.setStyle( n );
    pPen->setValue( pen );
}

void SPenPanel::slotWidth( int n )
{
    pen.setWidth( n );
    pPen->setValue( pen );
}

void SPenPanel::slotColorBrush( const QColor &color )
{
    QBrush brush = pen.brush();
    brush.setColor( color );
    pen.setBrush( brush );
    pPen->setValue( pen );
}

void SPenPanel::slotStyleBrush( Qt::BrushStyle n )
{
    QBrush brush = pen.brush();
    brush.setStyle( n );
    pen.setBrush( brush );
    pPen->setValue( pen );
}

void SPenPanel::slotImageBrush()
{
    QImage image;
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select image..."), QString(), "*.png" );
    if ( !stringFileName.isEmpty() ) image.load( stringFileName );

    QBrush brush = pen.brush();
    brush.setTextureImage( image );
    pen.setBrush( brush );
    pPen->setValue( pen );
}

/****************************************************************
 * SPenSwatch
 ****************************************************************/
SPenSwatch::SPenSwatch( QWidget *pParent )
    : QWidget( pParent )
{
    pPen = (SPen*)g_SSettings->getSetting( "SPen" );
    if ( pPen )
    {
        pen = pPen->getValue();
        connect( pPen, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pPen, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SPen].";
    }
}

void SPenSwatch::slotRefresh()
{
    pen = pPen->getValue();
    update();
}

void SPenSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );
    painter.setPen( pen );
    painter.fillRect( 0, 0, width(), height(), pen.color().lighter() );
    painter.drawLine( 0, height()/2, width(), height()/2 );
}

QSize SPenSwatch::sizeHint() const
{
    return QSize( 48, 48 );
}


