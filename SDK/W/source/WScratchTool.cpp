/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "WScratchTool.h"

WScratchSwatch::WScratchSwatch( QWidget *pParent )
    : QWidget( pParent )
{
}

void WScratchSwatch::setImage( const QImage &i )
{
    image = i;
    update();
}

void WScratchSwatch::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    if ( image.isNull() ) return;
    QPainter painter( this );
    painter.fillRect( rect(), QBrush( QImage( ":W/Transparent" ) ) );
    painter.drawImage( QRect( 0, 0, rect().width(), rect().height() ), image );
}

WScratchTool::WScratchTool( QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // main
    {
        QHBoxLayout *pLayoutMain = new QHBoxLayout();

        pSwatch  = new WScratchSwatch( this );
        pSlider  = new QSlider( Qt::Vertical, this );
        pSlider->setMinimum( 0 );
        pSlider->setMaximum( 0 );

        pLayoutMain->addWidget( pSwatch );
        pLayoutMain->addWidget( pSlider );

        pLayoutTop->addLayout( pLayoutMain );

        connect( pSlider, SIGNAL(valueChanged(int)), SLOT(slotSlider(int)) );
    }

    // tool bar
    {
        QHBoxLayout *pLayoutToolBar = new QHBoxLayout();
        pAdd       = new QToolButton( this );
        pRemove    = new QToolButton( this );
        pPaste     = new QToolButton( this );

        pAdd->setText( "+" );
        pRemove->setText( "-" );
        pPaste->setIcon( QIcon( ":W/Paste48x48" ) );

        pAdd->setEnabled( false );
        pRemove->setEnabled( false );
        pPaste->setEnabled( false );

        pLayoutToolBar->addWidget( pAdd );
        pLayoutToolBar->addWidget( pRemove );
        pLayoutToolBar->addWidget( pPaste );
        pLayoutToolBar->addStretch( 10 );

        pLayoutTop->addLayout( pLayoutToolBar );

        connect( pAdd, SIGNAL(clicked()), SIGNAL(signalAdd()) );
        connect( pRemove, SIGNAL(clicked()), SLOT(slotRemove()) );
        connect( pPaste, SIGNAL(clicked()), SLOT(slotPaste()) );
    }
}

void WScratchTool::setMax( int n )
{
    if ( n < 1 ) return;
    if ( n > 100 ) n = 100;
    nMax = n;
}

void WScratchTool::doAppend( const QImage &image )
{
    vectorImages.append( image );
    if ( vectorImages.count() > 1 )
    {
        pSlider->setMaximum( pSlider->maximum() + 1 );
        pSlider->setValue( pSlider->maximum() );
    }
    else
    {
        pRemove->setEnabled( true );
        pPaste->setEnabled( true );
    }
    pSwatch->setImage( image );
}

void WScratchTool::doClear()
{
    vectorImages.clear();
    pSlider->setMaximum( 0 );
    pRemove->setEnabled( false );
    pPaste->setEnabled( false );
    pSwatch->setImage( QImage() );
}

void WScratchTool::doEnableAdd( bool b )
{
    pAdd->setEnabled( b );
}

void WScratchTool::slotSlider( int n )
{
    if ( n >= vectorImages.count() ) return;
    pSwatch->setImage( vectorImages.at( n ) );
}

void WScratchTool::slotRemove()
{
    // must have an image to remove
    Q_ASSERT( vectorImages.count() );

    // current index
    int n = pSlider->value();
    Q_ASSERT( n < vectorImages.count() );

    // remove image
    vectorImages.remove( n );

    // sync interface
    if ( vectorImages.count() )
    {
        pSlider->setMaximum( vectorImages.count() - 1 );
        pSwatch->setImage( vectorImages.at( pSlider->value() ) );
    }
    else
    {
        doClear();
    }
}

void WScratchTool::slotPaste()
{
    // must have an image to remove
    Q_ASSERT( vectorImages.count() );

    // current index
    int n = pSlider->value();
    Q_ASSERT( n < vectorImages.count() );

    // tell folks that care
    emit signalPaste( vectorImages.at( n ) );
}


