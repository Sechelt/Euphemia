/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WPosWidget.h"
#include "W.h"

WPosWidget::WPosWidget( const QPointF &pointPos, QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    pSpinBoxX = new QDoubleSpinBox( this );
    pSpinBoxX->setMinimum( -10000 );
    pSpinBoxX->setMaximum( 10000 );
    pSpinBoxX->setValue( pointPos.x() );
    pSpinBoxX->setToolTip( tr("x coordinate") );
    connect( pSpinBoxX, SIGNAL(valueChanged(double)), SLOT(slotChanged()) );

    pSpinBoxY = new QDoubleSpinBox( this );
    pSpinBoxY->setMinimum( -10000 );
    pSpinBoxY->setMaximum( 10000 );
    pSpinBoxY->setValue( pointPos.y() );
    pSpinBoxY->setToolTip( tr("y coordinate") );
    connect( pSpinBoxY, SIGNAL(valueChanged(double)), SLOT(slotChanged()) );

    if ( bToolBar )
    { 
        QGridLayout *pLayout = new QGridLayout( this );
        pLayout->addWidget( pSpinBoxX, 0, 0 );
        pLayout->addWidget( pSpinBoxY, 1, 0 );
    }
    else
    {
        QFormLayout *pLayoutF = new QFormLayout( this );
        pLayoutF->addRow( new QLabel( tr("x:"), this ), pSpinBoxX );
        pLayoutF->addRow( new QLabel( tr("y:"), this ), pSpinBoxY );
    }
}

void WPosWidget::setValue( const QPointF &pointPos )
{
    pSpinBoxX->setValue( pointPos.x() );
    pSpinBoxY->setValue( pointPos.y() );
}

QPointF WPosWidget::getValue() 
{ 
    return QPointF( pSpinBoxX->value(), pSpinBoxY->value() ); 
}

void WPosWidget::slotValue( const QPointF &pointPos )
{
    setValue( pointPos );
}

void WPosWidget::slotChanged()
{
    emit signalChanged( getValue() );
}


