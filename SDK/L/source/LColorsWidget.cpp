/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#include "LibInfo.h"
#include "LColorsWidget.h"

#include <SColorsWidget.h>
#include <SColorsPickerDialog.h>

/****************************************************************
 * LColorControl
 ****************************************************************/
LColorControl::LColorControl( QWidget *pParent )
    : QWidget( pParent )
{
    pSettingPen     = (SPen*)g_SSettings->getSetting( "SPen" );
    pSettingBrush   = (SBrush*)g_SSettings->getSetting( "SBrush" );
    Q_ASSERT( pSettingPen );
    Q_ASSERT( pSettingBrush );

    colorPen = pSettingPen->getValue().color();
    colorBrush = pSettingBrush->getValue().color();

    doCalcLayout();
    setMinimumSize( size );

    connect( pSettingPen, SIGNAL(signalChanged()), SLOT(slotRefreshPen()) );
    connect( pSettingBrush, SIGNAL(signalChanged()), SLOT(slotRefreshBrush()) );
}

void LColorControl::slotColorPicker()
{
    bColorPicker = true;
    grabMouse( QCursor( QPixmap( ":W/ColorPicker" ), 0, 0 ) );

    // note: grabMouse() is probably doing the following for us...
    // QGuiApplication::setOverrideCursor()
    // QGuiApplication::restoreOverrideCursor()
}

// something changed the color (and is not aware of foreground/background control state or settings
void LColorControl::slotChange( const QColor &color )
{
    // from widget not aware of g_SSetting
    // - set in self 
    if ( nSwatch == SwatchPen )
    {
        if ( colorPen == color ) return;
        colorPen = color;
        QPen pen = pSettingPen->getValue();
        pen.setColor( colorPen );
        pSettingPen->setValue( pen );
        update();
        return;
    }

    if ( colorBrush == color ) return;
    colorBrush = color;
    QBrush brush = pSettingBrush->getValue();
    brush.setColor( colorBrush );
    pSettingBrush->setValue( brush );
    update();
}

void LColorControl::slotRefreshPen()
{
    colorPen = pSettingPen->getValue().color();
    if ( nSwatch == SwatchPen )
    {
        emit signalChanged( colorPen );
    }
    update();
}

void LColorControl::slotRefreshBrush()
{
    colorBrush = pSettingBrush->getValue().color();
    if ( nSwatch == SwatchBrush )
    {
        emit signalChanged( colorBrush );
    }
    update();
}

void LColorControl::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );
    painter.setRenderHint( QPainter::SmoothPixmapTransform );

    QPen pen( Qt::darkGray );
    pen.setWidth( 2 );

    if ( nSwatch == SwatchBrush ) pen.setStyle( Qt::DotLine );
    else pen.setStyle( Qt::NoPen );
    painter.setPen( pen );
    painter.setBrush( QBrush( colorBrush ) );
    painter.drawRect( rectBrush );

    if ( nSwatch == SwatchPen ) pen.setStyle( Qt::DotLine );
    else pen.setStyle( Qt::NoPen );
    painter.setPen( pen );
    painter.setBrush( QBrush( colorPen ) );
    painter.drawRect( rectPen );
}

void LColorControl::resizeEvent( QResizeEvent *pEvent )
{
    Q_UNUSED( pEvent );
    doCalcLayout();
}

void LColorControl::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    if ( bColorPicker ) return;

    if ( rectPen.contains( pEvent->pos() ) )
    {
        nSwatch = SwatchPen;
        bool bOk = false;
        QColor color = SColorsPickerDialog::getColor( &bOk, colorPen, this, false );
        if ( !bOk ) return;
        if ( colorPen == color ) return;
        // update context
        colorPen = color;
        QPen pen = pSettingPen->getValue();
        pen.setColor( color );
        pSettingPen->setValue( pen );
        // update other
        emit signalChanged( colorPen );

    }
    else if ( rectBrush.contains( pEvent->pos() ) )
    {
        nSwatch = SwatchBrush;
        bool bOk = false;
        QColor color = SColorsPickerDialog::getColor( &bOk, colorBrush, this, false );
        if ( !bOk ) return;
        if ( colorBrush == color ) return;
        // update context
        colorBrush = color;
        QBrush brush = pSettingBrush->getValue();
        brush.setColor( color );
        pSettingBrush->setValue( brush );
        // update other
        emit signalChanged( colorBrush );
    }
}

void LColorControl::mousePressEvent( QMouseEvent *pEvent )
{
    if ( bColorPicker  )
    {
        QScreen *screen = QGuiApplication::primaryScreen();
        if ( const QWindow *window = windowHandle() ) screen = window->screen();
        if ( !screen )
        {
            bColorPicker = false;
            releaseMouse();
            return;
        }

        QPixmap pixmap = screen->grabWindow( 0, 0, 0, screen->virtualSize().width(), screen->virtualSize().height() );
        slotChange( pixmap.toImage().pixelColor( pEvent->globalPosition().toPoint() ) );

        // ALT
/*
        QWidget *p = window();                                                            
        Q_ASSERT( p );                                                                    
#if QT_VERSION < 0x060000                                                                 
        slotChange( p->grab().toImage().pixelColor( pEvent->windowPos().toPoint() ) );    
#else                                                                                     
        slotChange( p->grab().toImage().pixelColor( pEvent->scenePosition().toPoint() ) );
#endif                                                                                    
*/

        bColorPicker = false;
        releaseMouse();
        return;
    }

    if ( rectPen.contains( pEvent->pos() ) )
    {
        if ( nSwatch == SwatchPen ) return;
        nSwatch = SwatchPen;
        emit signalChanged( nSwatch );
        update();
    }
    else if ( rectBrush.contains( pEvent->pos() ) )
    {
        if ( nSwatch == SwatchBrush ) return;
        nSwatch = SwatchBrush;
        emit signalChanged( nSwatch );
        update();
    }
}

QSize LColorControl::sizeHint() const
{
    return size;
}

void LColorControl::doCalcLayout()
{
    // layout is relative to center of contentsRect
    // we want a 50% overlap 
    rectPen.moveBottomRight( contentsRect().center() + QPoint( rectPen.width() / 4, rectPen.height() / 4 ) );
    rectBrush.moveTopLeft( contentsRect().center() - QPoint( rectBrush.width() / 4, rectBrush.height() / 4 ) );
}

/****************************************************************
 * LColorToolBar
 ****************************************************************/
LColorToolBar::LColorToolBar( QWidget *pParent )
    : QWidget( pParent )
{
    pLayout = new QHBoxLayout( this );

    // button to start a color picker process 
    QToolButton *pColorPicker = new QToolButton( this );
    pColorPicker->setIcon( QIcon( ":W/ColorPicker" ) );
    pColorPicker->setToolTip( tr("pick a color from the screen") );
    pLayout->addWidget( pColorPicker );

    // foreground/background color control
    pColorControl = new LColorControl( this );
    pLayout->addWidget( pColorControl );

    // pen/brush tool bar reflecting foreground/background state of control
    if ( pColorControl->getSwatch() == LColorControl::SwatchPen )
    {
        pPenToolBar = new LColorsPenWidget( this );
        pLayout->addWidget( pPenToolBar );
    }
    else
    {
        pBrushToolBar = new LColorsBrushWidget( this );
        pLayout->addWidget( pBrushToolBar );
    }

    // initiate a color picker process
    connect( pColorPicker, SIGNAL(released()), pColorControl, SLOT(slotColorPicker()) );
    // echo out a color change for those that care 
    connect( pColorControl, SIGNAL(signalChanged(const QColor &)), SIGNAL(signalChanged(const QColor &)) );
    // control has switched (selectted foreground/background) so we need to change our tool bar
    connect( pColorControl, SIGNAL(signalChanged(LColorControl::Swatchs)), SLOT(slotRefresh(LColorControl::Swatchs)) );
}

void LColorToolBar::slotChange( const QColor &color )
{
    pColorControl->slotChange( color );
}

void LColorToolBar::slotRefresh( LColorControl::Swatchs n )
{
    if ( n == LColorControl::SwatchPen )
    {
        Q_ASSERT( pBrushToolBar );
        delete pBrushToolBar;
        pBrushToolBar = nullptr;
        pPenToolBar = new LColorsPenWidget( this );
        pLayout->addWidget( pPenToolBar );
    }
    else if ( n == LColorControl::SwatchBrush )
    {
        Q_ASSERT( pPenToolBar );
        delete pPenToolBar;
        pPenToolBar = nullptr;
        pBrushToolBar = new LColorsBrushWidget( this );
        pLayout->addWidget( pBrushToolBar );
    }
}

/****************************************************************
 * LColorsWidget
 ****************************************************************/
LColorsWidget::LColorsWidget( QWidget *pParent )
    : QWidget ( pParent )
{
    QVBoxLayout *pVLayout = new QVBoxLayout( this );
    QHBoxLayout *pHLayout = new QHBoxLayout();

    LColorToolBar *pColorControl = new LColorToolBar( this );
    pColorControl->setToolTip( tr("configure pen (top) and fill (bottom)") );
    pHLayout->addWidget( pColorControl );
    pHLayout->addStretch( 10 );

    pVLayout->addLayout( pHLayout );

    SColorsWidget *pPaletteColor = new SColorsWidget( this );
    pVLayout->addWidget( pPaletteColor );

    connect( pPaletteColor, SIGNAL(signalSelected(const QColor &)), pColorControl, SLOT(slotChange(const QColor &)) );
    connect( pColorControl, SIGNAL(signalChanged(const QColor &)), pPaletteColor, SLOT(slotRefresh(const QColor &)) );
}

/****************************************************************
 * LColorsPenWidget
 ****************************************************************/
LColorsPenWidget::LColorsPenWidget( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "LColorsPenWidget" );

    pPen = (SPen*)g_SSettings->getSetting( "SPen" );
    if ( pPen )
    {
        pen = pPen->getValue();

        QHBoxLayout *pLayout = new QHBoxLayout( this );

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

void LColorsPenWidget::slotRefresh()
{
    pen = pPen->getValue();
    pStyle->setValue( pen.style() );                
    pWidth->setValue( pen.width() );
}

void LColorsPenWidget::slotStyle( Qt::PenStyle n )
{
    pen.setStyle( n );
    pPen->setValue( pen );
}

void LColorsPenWidget::slotWidth( int n )
{
    pen.setWidth( n );
    pPen->setValue( pen );
}

void LColorsPenWidget::slotMore()
{
    SDialog dialog( this, "SPen", "Pen" );
    dialog.exec();
}

/****************************************************************
 * LColorsBrushWidget
 ****************************************************************/
LColorsBrushWidget::LColorsBrushWidget( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "LColorsBrushWidget" );

    pBrush = (SBrush*)g_SSettings->getSetting( "SBrush" );
    if ( pBrush )
    {
        brush    = pBrush->getValue();

        QHBoxLayout *pLayout = new QHBoxLayout( this );

        pStyle = new WBrushStyleComboBox( this, brush.style() );
        pStyle->setToolTip( tr("brush style") );
        pLayout->addWidget( pStyle );
        connect( pStyle, SIGNAL(signalChanged(Qt::BrushStyle)), SLOT(slotStyle(Qt::BrushStyle)) );

        pMore = new QToolButton( this );
        pMore->setToolTip( tr("more") );
        pMore->setText( ":" );
        pLayout->addWidget( pMore );
        connect( pMore, SIGNAL(clicked()), SLOT(slotMore()) );

        pLayout->addStretch( 10 );

        connect( pBrush, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
        connect( pBrush, SIGNAL(signalSync()), SLOT(slotRefresh()) );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find key [SBrush].";
    }
}

void LColorsBrushWidget::slotRefresh()
{
    brush = pBrush->getValue();
    pStyle->setValue( brush.style() );
}

void LColorsBrushWidget::slotStyle( Qt::BrushStyle n )
{
    brush.setStyle( n );
    pBrush->setValue( brush );
}

void LColorsBrushWidget::slotMore()
{
    SDialog dialog( this, "SBrush", tr("Brush") );
    dialog.exec();
}

