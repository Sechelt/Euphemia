/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WMarginsWidget.h"
#include "W.h"

WMarginsWidget::WMarginsWidget( const QMargins &margins, QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    pSpinBoxLeft = new QSpinBox( this );
    pSpinBoxLeft->setMinimum( 0 );
    pSpinBoxLeft->setMaximum( 100 );
    pSpinBoxLeft->setValue( margins.left() );
    pSpinBoxLeft->setToolTip( tr("left margin") );
    connect( pSpinBoxLeft, SIGNAL(valueChanged(int)), SLOT(slotChanged()) );

    pSpinBoxTop = new QSpinBox( this );
    pSpinBoxTop->setMinimum( 0 );
    pSpinBoxTop->setMaximum( 100 );
    pSpinBoxTop->setValue( margins.top() );
    pSpinBoxTop->setToolTip( tr("top margin") );
    connect( pSpinBoxTop, SIGNAL(valueChanged(int)), SLOT(slotChanged()) );

    pSpinBoxRight = new QSpinBox( this );
    pSpinBoxRight->setMinimum( 0 );
    pSpinBoxRight->setMaximum( 100 );
    pSpinBoxRight->setValue( margins.right() );
    pSpinBoxRight->setToolTip( tr("right margin") );
    connect( pSpinBoxRight, SIGNAL(valueChanged(int)), SLOT(slotChanged()) );

    pSpinBoxBottom = new QSpinBox( this );
    pSpinBoxBottom->setMinimum( 0 );
    pSpinBoxBottom->setMaximum( 100 );
    pSpinBoxBottom->setValue( margins.bottom() );
    pSpinBoxBottom->setToolTip( tr("bottom margin") );
    connect( pSpinBoxBottom, SIGNAL(valueChanged(int)), SLOT(slotChanged()) );

    if ( bToolBar )
    {
        // no labels and compact
        QGridLayout *pLayout = new QGridLayout( this );
        pLayout->addWidget( pSpinBoxLeft, 0, 0 );
        pLayout->addWidget( pSpinBoxTop, 1, 0 );
        pLayout->addWidget( pSpinBoxRight, 0, 1 );
        pLayout->addWidget( pSpinBoxBottom, 1, 1 );
    }
    else
    {
        // no labels but use space to indicate purpose
        QGridLayout *pLayout = new QGridLayout( this );
        pLayout->addWidget( pSpinBoxTop, 0, 1 );
        pLayout->addWidget( pSpinBoxLeft, 1, 0 );
        pLayout->addWidget( pSpinBoxRight, 1, 2 );
        pLayout->addWidget( pSpinBoxBottom, 2, 1 );
    }
}

void WMarginsWidget::setValue( const QMargins &margins )
{
    pSpinBoxLeft->setValue( margins.left() );  
    pSpinBoxRight->setValue( margins.right() ); 
    pSpinBoxTop->setValue( margins.top() );   
    pSpinBoxBottom->setValue( margins.bottom() );
}

QMargins WMarginsWidget::getValue()
{
    QMargins margins;

    margins.setLeft( pSpinBoxLeft->value() );  
    margins.setRight( pSpinBoxRight->value() ); 
    margins.setTop( pSpinBoxTop->value() );   
    margins.setBottom( pSpinBoxBottom->value() );

    return margins;
}

void WMarginsWidget::slotValue( const QMargins &margins )
{
    setValue( margins );
}

void WMarginsWidget::slotChanged()
{
    emit signalChanged( getValue() );
}


