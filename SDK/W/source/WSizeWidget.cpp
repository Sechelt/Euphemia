/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WSizeWidget.h"
#include "W.h"

WSizeWidgetDouble::WSizeWidgetDouble( const QSizeF &size, QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    pSpinBoxWidth = new QDoubleSpinBox( this );
    pSpinBoxWidth->setMinimum( 0 );
    pSpinBoxWidth->setMaximum( 10000 );
    pSpinBoxWidth->setValue( size.width() );
    pSpinBoxWidth->setToolTip( tr("width") );
    connect( pSpinBoxWidth, SIGNAL(valueChanged(double)), SLOT(slotChanged()) );

    pSpinBoxHeight = new QDoubleSpinBox( this );
    pSpinBoxHeight->setMinimum( 0 );
    pSpinBoxHeight->setMaximum( 10000 );
    pSpinBoxHeight->setValue( size.height() );
    pSpinBoxHeight->setToolTip( tr("height") );
    connect( pSpinBoxHeight, SIGNAL(valueChanged(double)), SLOT(slotChanged()) );

    if ( bToolBar )
    { 
        QGridLayout *pLayout = new QGridLayout( this );
        pLayout->addWidget( pSpinBoxWidth, 0, 0 );
        pLayout->addWidget( pSpinBoxHeight, 1, 0 );
    }
    else
    {
        QFormLayout *pLayoutF = new QFormLayout( this );
        pLayoutF->addRow( new QLabel( tr("Width:"), this ), pSpinBoxWidth );
        pLayoutF->addRow( new QLabel( tr("Height:"), this ), pSpinBoxHeight );
    }
}

void WSizeWidgetDouble::setValue( const QSizeF &size )
{
    pSpinBoxWidth->setValue( size.width() );
    pSpinBoxHeight->setValue( size.height() );
}

QSizeF WSizeWidgetDouble::getValue() 
{ 
    return QSizeF( pSpinBoxWidth->value(), pSpinBoxHeight->value() ); 
}

void WSizeWidgetDouble::slotValue( const QSizeF &size )
{
    setValue( size );
}

void WSizeWidgetDouble::slotChanged()
{
    emit signalChanged( getValue() );
}

//
//
//

WSizeWidgetInt::WSizeWidgetInt( const QSize &size, QWidget *pParent, bool bToolBar, bool bAspect )
    : QWidget( pParent )
{
    sizeOriginal = size;

    pSpinBoxWidth = new QSpinBox( this );
    pSpinBoxWidth->setMinimum( 0 );
    pSpinBoxWidth->setMaximum( 10000 );
    pSpinBoxWidth->setValue( size.width() );
    pSpinBoxWidth->setToolTip( tr("width") );
    connect( pSpinBoxWidth, SIGNAL(valueChanged(int)), SLOT(slotChangedWidth()) );

    pSpinBoxHeight = new QSpinBox( this );
    pSpinBoxHeight->setMinimum( 0 );
    pSpinBoxHeight->setMaximum( 10000 );
    pSpinBoxHeight->setValue( size.height() );
    pSpinBoxHeight->setToolTip( tr("height") );
    connect( pSpinBoxHeight, SIGNAL(valueChanged(int)), SLOT(slotChangedHeight()) );

    if ( bAspect )
    {
        pButtonLockAspect = new QToolButton( this );
        pButtonLockAspect->setCheckable( true );
        pButtonLockAspect->setChecked( true );
        pButtonLockAspect->setIcon( QPixmap( ":W/Lock" ) );
        connect( pButtonLockAspect, SIGNAL(toggled(bool)), SLOT(slotLockAspect(bool)) );
    }
    else 
        pButtonLockAspect = nullptr;

    if ( bToolBar )
    { 
        QGridLayout *pLayout = new QGridLayout( this );
        pLayout->addWidget( pSpinBoxWidth, 0, 0 );
        pLayout->addWidget( pSpinBoxHeight, 1, 0 );
        if ( pButtonLockAspect ) pLayout->addWidget( pButtonLockAspect, 2, 0 );
    }
    else
    {
        QFormLayout *pLayoutF = new QFormLayout( this );
        pLayoutF->addRow( new QLabel( tr("Width:"), this ), pSpinBoxWidth );
        pLayoutF->addRow( new QLabel( tr("Height:"), this ), pSpinBoxHeight );
        if ( pButtonLockAspect ) pLayoutF->addRow( new QLabel( tr("Aspect:"), this ), pButtonLockAspect );
    }
}

void WSizeWidgetInt::setValue( const QSize &size )
{
    sizeOriginal = size;
    pSpinBoxWidth->setValue( size.width() );
    pSpinBoxHeight->setValue( size.height() );
}

QSize WSizeWidgetInt::getValue() 
{ 
    return QSize( pSpinBoxWidth->value(), pSpinBoxHeight->value() ); 
}

void WSizeWidgetInt::slotValue( const QSize &size )
{
    setValue( size );
}

void WSizeWidgetInt::slotChangedWidth()
{
    if ( pButtonLockAspect && pButtonLockAspect->isChecked() )
    {
        // sync height
        
        // diff as ratio to original
        qreal nDiff = pSpinBoxWidth->value() - sizeOriginal.width();
        qreal nRatio = sizeOriginal.width() / nDiff;
        // apply to height (disconnect signal temporarily)
        qreal nHeight = sizeOriginal.height() + sizeOriginal.height() / nRatio;
        disconnect( pSpinBoxHeight, SIGNAL(valueChanged(int)), this, SLOT(slotChangedHeight()) );
        pSpinBoxHeight->setValue( nHeight );
        connect( pSpinBoxHeight, SIGNAL(valueChanged(int)), SLOT(slotChangedHeight()) );
    }
    emit signalChanged( getValue() );
}

void WSizeWidgetInt::slotChangedHeight()
{
    if ( pButtonLockAspect && pButtonLockAspect->isChecked() )
    {
        // sync width

        // diff as ratio to original
        qreal nDiff = pSpinBoxHeight->value() - sizeOriginal.height();
        qreal nRatio = sizeOriginal.height() / nDiff;
        // apply to height (disconnect signal temporarily)
        qreal nWidth = sizeOriginal.width() + sizeOriginal.width() / nRatio;
        disconnect( pSpinBoxWidth, SIGNAL(valueChanged(int)), this, SLOT(slotChangedWidth()) );
        pSpinBoxWidth->setValue( nWidth );
        connect( pSpinBoxWidth, SIGNAL(valueChanged(int)), SLOT(slotChangedWidth()) );
    }
    emit signalChanged( getValue() );
}

void WSizeWidgetInt::slotLockAspect( bool b )
{
    if ( b ) pButtonLockAspect->setIcon( QPixmap( ":W/Lock" ) );
    else pButtonLockAspect->setIcon( QPixmap( ":W/Unlock" ) );
}


