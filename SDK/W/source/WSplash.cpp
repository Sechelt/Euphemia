/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */
#include "LibInfo.h"
#include "WSplash.h"

WSplash::WSplash( QWidget *pwidgetParent, const QPixmap &pixmap, bool bShowCheckBox, bool bShowButton, int nDelaySeconds )
     : QDialog( pwidgetParent, Qt::CustomizeWindowHint | Qt::FramelessWindowHint )
{
    // we are modal with no title bar - so we must have some way to close - we can always use Esc key
    setModal( true );

    QGridLayout *pgridlayoutTop = new QGridLayout( this );
    QGridLayout *pgridlayout    = new QGridLayout();

    QLabel *plabel = new QLabel( this );
    plabel->setPixmap( pixmap );
    pgridlayoutTop->addWidget( plabel, 0, 0 );

    pgridlayoutTop->addLayout( pgridlayout, 1, 0 );
    
    pgridlayoutTop->setRowStretch( 0, 10 );
    pgridlayout->setColumnStretch( 0, 10 );
    
    if ( bShowCheckBox )
    {
        pShow = new QCheckBox( tr( "Show Splash" ), this );
        pShow->setChecked( QSettings().value( "Splash", false ).toInt() );
        pgridlayout->addWidget( pShow, 0, 0 );
        connect( pShow, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotShow(Qt::CheckState)) );
    }

    if ( bShowButton ) 
    {
        pOk = new QPushButton( "Ok", this );
        pgridlayout->addWidget( pOk, 0, 1 );
        connect( pOk, SIGNAL(clicked()), SLOT(accept()) );
    }
    
    if ( nDelaySeconds > 0 )
    {
        QTimer::singleShot( nDelaySeconds * 1000, this, SLOT(reject()) );
    }
}

void WSplash::slotShow( Qt::CheckState n )
{
    // this will go into the app settings as defined in its main.cpp
    QSettings().setValue( "Splash", int(n) );
}


