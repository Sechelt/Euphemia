/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WZoomWidget.h"

WZoomWidget::WZoomWidget( QWidget *pParent, int n )
    : QWidget( pParent )
{
    nMin    = 10;
    nMax    = 6000;
    nInc    = 1;        // need 1 so we can get back to exactly 100 after zoomed for Fit
    nZoom   = n;

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pMinus = new QToolButton( this );
    // pMinus->setAutoRepeat( true  );
    // pMinus->setAutoRepeatInterval( 33 );
    // pMinus->setAutoRepeatDelay( 0 );
    pMinus->setIcon( QIcon( ":W/ZoomOut48x48" ) );
    pMinus->setToolTip( tr("zoom out") );
    pLayout->addWidget( pMinus );

    pSlider = new QSlider( Qt::Horizontal, this );
    pSlider->setMinimum( nMin );
    pSlider->setMaximum( nMax );
    pSlider->setSingleStep( nInc );
    pSlider->setPageStep( nInc * 10 );
    pSlider->setValue( nZoom );
    pSlider->setToolTip( tr("set zoom by sliding") );
    pLayout->addWidget( pSlider, 10 );

    pPlus = new QToolButton( this );
    // pPlus->setAutoRepeat( true  );
    // pPlus->setAutoRepeatInterval( 33 );
    // pPlus->setAutoRepeatDelay( 0 );
    pPlus->setIcon( QIcon( ":W/ZoomIn48x48" ) );
    pPlus->setToolTip( tr("zoom in") );
    pLayout->addWidget( pPlus );

    pLineEdit = new QLineEdit( QString::number( nZoom ), this );
    pValidator = new QIntValidator( nMin, nMax, this );
    pLineEdit->setValidator( pValidator );
    pLineEdit->setToolTip( tr("set zoom by entering a percent") );
    pLayout->addWidget( pLineEdit );

    QLabel *pLabel = new QLabel( this );
    pLabel->setText( "%" );
    pLayout->addWidget( pLabel );

    pFit = new QComboBox( this );
    pFit->addItem( tr("Width") );
    pFit->addItem( tr("Height") );
    pFit->addItem( tr("All") );
    pFit->addItem( tr("") );
    pFit->setCurrentText( "" );
    pFit->setToolTip( tr("Set zoom to show; width, height or all.\nSpecify blank to ignore this method.") );
    pLayout->addWidget( pFit );

    connect( pSlider, SIGNAL(valueChanged(int)), SLOT(slotSlider(int)) );
    connect( pPlus, SIGNAL(clicked()), SLOT(slotPlus()) );
    connect( pMinus, SIGNAL(clicked()), SLOT(slotMinus()) );
    connect( pLineEdit, SIGNAL(editingFinished()), SLOT(slotLineEdit()) );
    connect( pFit, SIGNAL(currentIndexChanged(int)), SLOT(slotFit(int)) );
}

void WZoomWidget::setMin( int n )
{
    nMin = n;
    pSlider->setMinimum( nMin );
    pValidator->setBottom( nMin );
}

void WZoomWidget::setMax( int n )
{
    nMax = n;
    pSlider->setMaximum( nMax );
    pValidator->setTop( nMax );
}

void WZoomWidget::setPage( int n )
{
    pSlider->setPageStep( n );
}

void WZoomWidget::setInc( int n )
{
    nInc = n;
    pSlider->setSingleStep( nInc );
}

void WZoomWidget::setZoom( int n )
{
    nZoom = n;
    pSlider->setValue( nZoom );
    pLineEdit->setText( QString::number( nZoom ) );
}

void WZoomWidget::setFit( FitTypes n )
{
    nFit = n;
    pFit->setCurrentIndex( nFit );
}

void WZoomWidget::setVisibleFit( bool b )
{
    pFit->setVisible( b );
}

int WZoomWidget::getZoom()
{
    return nZoom;
}

WZoomWidget::FitTypes WZoomWidget::getFit()
{
    return nFit;
}

void WZoomWidget::slotRefresh( WZoomWidget::FitTypes nFit, int nZoom )
{
    setZoom( nZoom );
    setFit( nFit );
}


QSize WZoomWidget::sizeHint() const
{
/*
    QSize size = pFit->sizeHint();  
    if ( nMax > 1000 )              
    {                               
        size.setWidth( nMax / 100 );
        return size;                
    }                               
*/
    return QWidget::sizeHint();
}

void WZoomWidget::slotSlider( int nValue )
{
    setZoom( nValue );
    emit signalZoom( nFit, nZoom );
}

void WZoomWidget::slotPlus()
{
    setZoom( pSlider->value() + nInc );
    emit signalZoom( nFit, nZoom );
}

void WZoomWidget::slotMinus()
{
    setZoom( pSlider->value() - nInc );
    emit signalZoom( nFit, nZoom );
}

void WZoomWidget::slotLineEdit()
{
    setZoom( pLineEdit->text().toInt() );
    emit signalZoom( nFit, nZoom );
}

void WZoomWidget::slotFit( int nIndex )
{
    nFit = (FitTypes)nIndex;
    switch ( nIndex )
    {
        case FitWidth:
        case FitHeight:
        case FitAll:
            pSlider->setEnabled( false );
            pPlus->setEnabled( false );
            pMinus->setEnabled( false );
            pLineEdit->setEnabled( false );
            emit signalZoom( nFit, nZoom );
            break;
        case FitIgnore:
            pSlider->setEnabled( true );
            pPlus->setEnabled( true );
            pMinus->setEnabled( true );
            pLineEdit->setEnabled( true );
            emit signalZoom( nFit, nZoom );
            break;
    }
}


