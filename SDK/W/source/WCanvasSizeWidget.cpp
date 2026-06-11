/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WCanvasSizeWidget.h"

WCanvasSizeWidget::WCanvasSizeWidget( const QSizeF &size, bool bPrinter, QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayout = new QVBoxLayout( this );

    // printer
    {
        pUsePrinter = new QCheckBox( tr("Use Printer Page Size"), this );
        pUsePrinter->setChecked( bPrinter );
        pLayout->addWidget( pUsePrinter );
    }
    // manual size
    {
        QFormLayout *pLayoutForm = new QFormLayout();

        pLineEditX = new QLineEdit( this );
        pLineEditX->setText( QString::number( size.width() ) );
        pLineEditX->setValidator( new QDoubleValidator( 10.0, 10000.0, 2, pLineEditX ) );
        pLayoutForm->addRow( tr("&Width:"), pLineEditX );

        pLineEditY = new QLineEdit( this );
        pLineEditY->setText( QString::number( size.height() ) );
        pLineEditY->setValidator( new QDoubleValidator( 10.0, 10000.0, 2, pLineEditY ) );
        pLayoutForm->addRow( tr("&Height:"), pLineEditY );

        pLayout->addLayout( pLayoutForm );
    }

    if ( bPrinter )
    {
         pLineEditX->setEnabled( false );
         pLineEditY->setEnabled( false );
    }

    connect( pUsePrinter, SIGNAL(clicked(bool)), SIGNAL(signalPrinter(bool)) );
    connect( pLineEditX, SIGNAL(textEdited(const QString &)), SLOT(slotSize(const QString &)) );
    connect( pLineEditY, SIGNAL(textEdited(const QString &)), SLOT(slotSize(const QString &)) );
}

void WCanvasSizeWidget::slotSize( const QSizeF &size )
{
    pLineEditX->setText( QString::number( size.width() ) );
    pLineEditY->setText( QString::number( size.height() ) );
}

void WCanvasSizeWidget::slotPrinter( bool bPrinter )
{
    if ( bPrinter )
    {
         pLineEditX->setEnabled( false );
         pLineEditY->setEnabled( false );
    }
    else
    {
        pLineEditX->setEnabled( true );
        pLineEditY->setEnabled( true );
    }

    pUsePrinter->setChecked( bPrinter );
}

void WCanvasSizeWidget::slotSize( const QString & )
{
    QSizeF size( pLineEditX->text().toDouble(), pLineEditY->text().toDouble() );
    emit signalSize( size );
}























