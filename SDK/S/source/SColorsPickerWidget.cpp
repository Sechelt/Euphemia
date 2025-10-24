/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "SColorsPickerWidget.h"

#include <SColors.h>

#include "SColorsWidget.h"

//
// SColorsSpectrumWidget
//
SColorsSpectrumWidget::SColorsSpectrumWidget( const QColor &color, QWidget *pParent )
    : QWidget( pParent )
{
    this->color = color;

    setMinimumWidth( 40 );
    setMinimumHeight( 40 );
}

void SColorsSpectrumWidget::setColor( const QColor &c )
{
    if ( color == c ) return;
    color =  c;

    // would be nice to find the pos of the color and adjust this->point

    update();
}

void SColorsSpectrumWidget::mousePressEvent( QMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton )
    {
        QWidget::mousePressEvent( pEvent );
        return;
    }

    point = pEvent->pos();

    // adopt Hue and Sat (but nothing else) from color at point
    {
        QPixmap pixmap = grab();
        QImage image( pixmap.toImage() );
        QColor c( image.pixel( point ) );

        int nH = 0;
        int nS = 0;
        int nV = 0;
        int nA = 0;
        int nHNew = 0;
        int nSNew = 0;
        int nVNew = 0;
        int nANew = 0;

        color.getHsv( &nH, &nS, &nV, &nA );
        c.getHsv( &nHNew, &nSNew, &nVNew, &nANew );

        color.setHsv( nHNew, nSNew, nVNew, nA );
    }

    emit signalValueChanged( color );
    update();
}

/*!
 * \brief Paint a circle with colors expressed using Hue and Saturation and allow selection. 
 *  
 * The Hue changes based upon the angle. 
 *  
 * The Saturation changes based upon the radius (distance from center). 
 *  
 * Value and Alpha are maintained in a 'current' QColor but are not used or altered here. 
 *  
 * \author pharvey (12/4/22)
 */
void SColorsSpectrumWidget::paintEvent( QPaintEvent * )
{
    QPainter painter( this );
    QPen pen;
    pen.setWidth(1);

    for ( int nX = 0; nX <= width(); nX++ )
    {
        for ( int nY  = 0; nY <= height(); nY++ )
        {
            // coordinate transformation so that the origin is at the centre of the item
            // also y values should increase as you go up and decrease as you go down
            int nXCoord = nX - width() / 2;
            int nYCoord = height() / 2 - nY;

            Polar p = cartesianToPolar( nXCoord, nYCoord );

            // only paint pixels inside the circle
            if ( p.radius <= width() / 2 )
            {
                // Hue is determined by the angle (value must be 0-1).
                // 
                // For some reason 0 is supposed to be presented at NE. Ours is presented at N.
                // We ignore this as it makes no difference to the selection.
                // 
                // ie   NE=0=0              Yellow
                //      SE=300/360=0.83     Purple
                //      S=240/360=0.67      Blue
                //      N=60/360=0.17       Cyan
                //      SW=180/360=0.5      Green
                qreal nHue = p.angle / ( 2 * M_PI );

                // Sat is determined by how far we are from the center (value must be 0-1)
                // ie   0   Center
                //      1   At the circumference
                qreal nSat = p.radius / width() * 2;

                QColor color;
                color.setHsvF( nHue, nSat, 1.0 );
                pen.setColor( color );

                painter.setPen( pen );
                painter.drawPoint( QPoint( nX, nY ) );
            }
        }
    }

    // draw cursor (cross-hair)
    int nOffset = 6;
    pen.setColor( Qt::black );
    painter.setPen( pen );
    painter.drawLine( point.x() - nOffset, point.y(), point.x() - 1, point.y() );
    painter.drawLine( point.x() + 1, point.y(), point.x() + nOffset, point.y() );

    painter.drawLine( point.x(), point.y() - nOffset, point.x(), point.y() - 1 );
    painter.drawLine( point.x(), point.y() + 1, point.x(), point.y() + nOffset );
}

/*! 
 * \brief Convert Cartesian coordinates to Polar position. 
 *  
 * Cartesian is a simple x,y position (in this case - relative to the center of the circle). 
 * Polar is the angle (radians) and radius (distance). 
 *  
 * We need to convert from Cartesian to Polar because we want the angle to calc the Hue and we 
 * want the radius to calc the Sat. 
 *  
 * We can then set the pixel Hue/Sat at the x,y position. 
 *  
 * Conversion is done using; 
 *  
 * r = √ ( x2 + y2 ) 
 * θ = inverse-tangent ( y / x ) 
 *  
 * tan with a superscripted -1 has been replaced with inverse-tangent because we can not do superscripted here. 
 * The inverse-tangent calculates the angle from the ratio of x,y (thats not divide there).
 *  
 * \author pharvey (12/4/22)
 * 
 * \param x 
 * \param y 
 * 
 * \return SColorsSpectrumWidget::Polar 
 */
SColorsSpectrumWidget::Polar SColorsSpectrumWidget::cartesianToPolar( qreal x, qreal y )
{
    // θ = tan-1 ( y / x )
    qreal nAngle = qAtan2( y, x );
    if ( nAngle < 0 )
    {
        nAngle += (2* M_PI);
    }

    // r = √ ( x2 + y2 )
    qreal nRadius = qSqrt( (x*x) + (y*y) );
    Polar p;
    p.angle = nAngle;
    p.radius = nRadius;

    return p;
}

//
// SColorsPickerSwatchWidget
//
SColorsPickerSwatchWidget::SColorsPickerSwatchWidget( const QColor &c, QWidget *pParent )
    : QWidget( pParent )
{
    color = c;

    setMinimumWidth( 16 );
    setFixedHeight( 16 );
}

void SColorsPickerSwatchWidget::slotColor( const QColor &c )
{
    if ( color == c ) return;
    color =  c;
    update();
}

void SColorsPickerSwatchWidget::paintEvent( QPaintEvent * )
{
    QPainter painter( this );

    if ( color.alpha() == 0 )
    {
        painter.setBrush( QBrush( QImage( ":W/Transparent" ) ) );
        painter.drawRect( rect() );
    }
    else
    {
        painter.setBrush( QBrush( color ) );
        painter.drawRect( rect() );
    }
}


//
// SColorsPickerWidget
//
SColorsPickerWidget::SColorsPickerWidget( const QColor &c, QWidget *pParent, bool bShowPaletteColor )
    : QWidget( pParent )
{
    int nH = 0;
    int nS = 0;
    int nV = 0;
    int nA = 0;

    color = c;

    color.getHsv( &nH, &nS, &nV, &nA );

    QHBoxLayout *pLayoutTop         = new QHBoxLayout( this );
    QVBoxLayout *pLayoutMain        = new QVBoxLayout();
    QGridLayout *pLayoutControls    = new QGridLayout();

    // picker
    pSpectrum = new SColorsSpectrumWidget(color, this);
    pSpectrum->setToolTip( tr("select Hue and Saturation") );
    pLayoutMain->addWidget( pSpectrum );
    connect( pSpectrum, SIGNAL(signalValueChanged(const QColor&)), SLOT(slotSpectrum(const QColor&)) );

    // controls
    QLabel *pLabelHue = new QLabel( tr("Hue:"), this );
    QLabel *pLabelSat = new QLabel( tr("Sat:"), this );
    QLabel *pLabelVal = new QLabel( tr("Val:"), this );
    QLabel *pLabelAlp = new QLabel( tr("Alp:"), this );

    pLayoutControls->addWidget( pLabelHue, 0, 0 );
    pLayoutControls->addWidget( pLabelSat, 1, 0 );
    pLayoutControls->addWidget( pLabelVal, 2, 0 );
    pLayoutControls->addWidget( pLabelAlp, 3, 0 );

    pSliderHue = new QSlider( Qt::Horizontal, this );
    pSliderSat = new QSlider( Qt::Horizontal, this );
    pSliderVal = new QSlider( Qt::Horizontal, this );
    pSliderAlp = new QSlider( Qt::Horizontal, this );

    pSliderHue->setRange( 0, 359 ); // Hue max is 359
    pSliderSat->setRange( 0, 255 );
    pSliderVal->setRange( 0, 255 );
    pSliderAlp->setRange( 0, 255 );

    pSliderHue->setValue( nH );
    pSliderSat->setValue( nS );
    pSliderVal->setValue( nV );
    pSliderAlp->setValue( nA );

    pSliderHue->setToolTip( tr("set Hue") );
    pSliderSat->setToolTip( tr("set Saturation") );
    pSliderVal->setToolTip( tr("set Value") );
    pSliderAlp->setToolTip( tr("set Alpha (transparency)") );

    pLayoutControls->addWidget( pSliderHue, 0, 1 );
    pLayoutControls->addWidget( pSliderSat, 1, 1 );
    pLayoutControls->addWidget( pSliderVal, 2, 1 );
    pLayoutControls->addWidget( pSliderAlp, 3, 1 );

    pSpinHue    = new QSpinBox( this );
    pSpinSat    = new QSpinBox( this );
    pSpinVal    = new QSpinBox( this );
    pSpinAlp    = new QSpinBox( this );

    pSpinHue->setMinimum( 0 );
    pSpinSat->setMinimum( 0 );
    pSpinVal->setMinimum( 0 );
    pSpinAlp->setMinimum( 0 );

    pSpinHue->setMaximum( 359 ); // Hue max is 359
    pSpinSat->setMaximum( 255 );
    pSpinVal->setMaximum( 255 );
    pSpinAlp->setMaximum( 255 );

    pSpinHue->setValue( nH );
    pSpinSat->setValue( nS );
    pSpinVal->setValue( nV );
    pSpinAlp->setValue( nA );

    pLayoutControls->addWidget( pSpinHue, 0, 2 );
    pLayoutControls->addWidget( pSpinSat, 1, 2 );
    pLayoutControls->addWidget( pSpinVal, 2, 2 );
    pLayoutControls->addWidget( pSpinAlp, 3, 2 );

    pLayoutMain->addLayout( pLayoutControls );

    connect( pSliderHue, SIGNAL(valueChanged(int)), SLOT(slotHue(int)) );
    connect( pSliderSat, SIGNAL(valueChanged(int)), SLOT(slotSat(int)) );
    connect( pSliderVal, SIGNAL(valueChanged(int)), SLOT(slotVal(int)) );
    connect( pSliderAlp, SIGNAL(valueChanged(int)), SLOT(slotAlp(int)) );

    // swatch
    pSwatch = new SColorsPickerSwatchWidget( color, this );
    pLayoutMain->addWidget( pSwatch );

    pLayoutTop->addLayout( pLayoutMain );

    // Palette Color (optional)
    // - SColorsWidget should never show in response to select a color for color palette - others - probably yes
    if ( bShowPaletteColor ) 
    {
        QVBoxLayout *pLayout = new QVBoxLayout();
        pLayout->addWidget( new QLabel( tr("User Defined"), this ) );

        pPalette = new SColorsWidget( this );
        pLayout->addWidget( pPalette, 10 );
        pLayoutTop->addLayout( pLayout );

        connect( pPalette, SIGNAL(signalSelected(const QColor &)), SLOT(slotPalette(const QColor &)) );
    }
}

void SColorsPickerWidget::setColor( const QColor &c )
{
    slotPalette( c );
}

/*!
 * \brief Adopt changes to Hue and Sat made by the color picker. 
 *  
 * The spectrum DOES NOT select Val or Alp but they are maintained in its current color.
 * 
 * \author pharvey (12/4/22)
 * 
 * \param color 
 */
void SColorsPickerWidget::slotSpectrum( const QColor &c )
{
    int nHue    = 0;
    int nSat    = 0;
    int nVal    = 0;
    int nAlp    = 0;

    color = c;
    color.getHsv( &nHue, &nSat, &nVal, &nAlp );

    pSliderHue->setValue( nHue );
    pSliderSat->setValue( nSat );
    pSliderVal->setValue( nVal );
    pSliderAlp->setValue( nAlp );

    pSpinHue->setValue( nHue );
    pSpinSat->setValue( nSat );
    pSpinVal->setValue( nVal );
    pSpinAlp->setValue( nAlp );

    pSwatch->slotColor( color );
}

void SColorsPickerWidget::slotPalette( const QColor &c )
{
    int nHue    = 0;
    int nSat    = 0;
    int nVal    = 0;
    int nAlp    = 0;

    // update color
    color = c;
    color.getHsv( &nHue, &nSat, &nVal, &nAlp );
    // update controls
    pSliderHue->setValue( nHue );
    pSliderSat->setValue( nSat );
    pSliderVal->setValue( nVal );
    pSliderAlp->setValue( nAlp );

    pSpinHue->setValue( nHue );
    pSpinSat->setValue( nSat );
    pSpinVal->setValue( nVal );
    pSpinAlp->setValue( nAlp );

    pSpectrum->setColor( color );
    pSwatch->slotColor( color );
}

void SColorsPickerWidget::slotHue( int nH )
{
    int n  = 0;
    int nS = 0;
    int nV = 0;
    int nA = 0;

    // update color
    color.getHsv( &n, &nS, &nV, &nA );
    color.setHsv( nH, nS, nV, nA );
    // update controls
    pSliderHue->setValue( nH );
    pSpinHue->setValue( nH );

    pSpectrum->setColor( color );
    pSwatch->slotColor( color );
}

void SColorsPickerWidget::slotSat( int nS )
{
    int nH = 0;
    int n  = 0;
    int nV = 0;
    int nA = 0;

    // update color
    color.getHsv( &nH, &n, &nV, &nA );
    color.setHsv( nH, nS, nV, nA );
    // update controls
    pSliderSat->setValue( nS );
    pSpinSat->setValue( nS );

    pSpectrum->setColor( color );
    pSwatch->slotColor( color );
}

void SColorsPickerWidget::slotVal( int nV )
{
    int nH = 0;
    int nS = 0;
    int n  = 0;
    int nA = 0;

    // update color
    color.getHsv( &nH, &nS, &n, &nA );
    color.setHsv( nH, nS, nV, nA );
    // update controls
    pSliderVal->setValue( nV );
    pSpinVal->setValue( nV );

    pSpectrum->setColor( color );
    pSwatch->slotColor( color );
}

void SColorsPickerWidget::slotAlp( int nA )
{
    int nH = 0;
    int nS = 0;
    int nV = 0;
    int n  = 0;

    // update color
    color.getHsv( &nH, &nS, &nV, &n );
    color.setHsv( nH, nS, nV, nA );
    // update controls
    pSliderAlp->setValue( nA );
    pSpinAlp->setValue( nA );

    pSpectrum->setColor( color );
    pSwatch->slotColor( color );
}



