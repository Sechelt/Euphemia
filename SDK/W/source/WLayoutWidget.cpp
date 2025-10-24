/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WLayoutWidget.h"
#include "W.h"

#include "WSizeHintWidget.h"
#include "WMarginsWidget.h"
#include "WSizeConstraintsWidget.h"
#include "WStretchWidget.h"

WLayoutWidget::WLayoutWidget( QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    QLayout *pLayoutTop;

    if ( bToolBar )
    { 
        QHBoxLayout *p = new QHBoxLayout( this );
        pLayoutTop = p;
    }
    else
    {
        QVBoxLayout *p = new QVBoxLayout( this );
        pLayoutTop = p;
    }
    // size hint
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Size Hint"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pSizeHintWidget = new WSizeHintWidget( QSizeF(), pGroupBox );
        pLayout->addWidget( pSizeHintWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pSizeHintWidget, SIGNAL(signalChanged(const QSizeF &)), SIGNAL(signalChangedSizeHint(const QSizeF &)) );
    }
    // constraints
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Size Constraints"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pSizeConstraintsWidget = new WSizeConstraintsWidget( QSizeF(), QSizeF(), pGroupBox );
        pLayout->addWidget( pSizeConstraintsWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pSizeConstraintsWidget, SIGNAL(signalChanged(const QSizeF &,const QSizeF &)), SIGNAL(signalChangedSizeConstraints(const QSizeF &,const QSizeF &)) );
    }
    // margins
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Margins"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pMarginsWidget = new WMarginsWidget( QMargins(), pGroupBox );
        pLayout->addWidget( pMarginsWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pMarginsWidget, SIGNAL(signalChanged(const QMargins &)), SIGNAL(signalChangedMargins(const QMargins &)) );
    }
    // stretch
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Stretch"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pStretchWidget = new WStretchWidget( QSize(), pGroupBox );
        pLayout->addWidget( pStretchWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pStretchWidget, SIGNAL(signalChanged(const QSizeF &)), SIGNAL(signalChangedStretch(const QSizeF &)) );
    }
}

void WLayoutWidget::setSizeHint( const QSizeF &size )
{
    pSizeHintWidget->setValue( size );
}

void WLayoutWidget::setMargins( const QMargins &m )
{
    pMarginsWidget->setValue( m );
}

void WLayoutWidget::setSizeConstraints( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum )
{
    pSizeConstraintsWidget->setMinimum( sizeMinimum );
    pSizeConstraintsWidget->setMaximum( sizeMaximum );
}

void WLayoutWidget::setStretch( const QSizeF &sizeStretch )
{
    pStretchWidget->setValue( sizeStretch );
}

QSizeF      WLayoutWidget::getSizeHint()               { return pSizeHintWidget->getValue();               }
QMargins    WLayoutWidget::getMargins()                { return pMarginsWidget->getValue();            }
QSizeF      WLayoutWidget::getSizeConstraintMinimum()  { return pSizeConstraintsWidget->getMinimum();  }
QSizeF      WLayoutWidget::getSizeConstraintMaximum()  { return pSizeConstraintsWidget->getMaximum();  }
QSizeF      WLayoutWidget::getStretch()                { return pStretchWidget->getValue();            }

void WLayoutWidget::slotSizeHint( const QSizeF &size )
{
    setSizeHint( size );
}

void WLayoutWidget::slotMargins( const QMargins &m )
{
    setMargins( m );
}

void WLayoutWidget::slotSizeConstraints( const QSizeF &sizeMinimum, const QSizeF &sizeMaximum )
{
    setSizeConstraints( sizeMinimum, sizeMaximum );
}

void WLayoutWidget::slotStretch( const QSizeF &sizeStretch )
{
    setStretch( sizeStretch );
}


