/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WPasswordWidget.h"
#include "W.h"

WPasswordWidget::WPasswordWidget( const QString &stringPassword, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pPassword = new QLineEdit( stringPassword, this );
    pPassword->setEchoMode( QLineEdit::Password );
    pLayout->addWidget( pPassword );

    pEchoMode = new QToolButton( this );
    pEchoMode->setIcon( QIcon( ":W/EchoModePassword" ) );
    pEchoMode->setCheckable( true );
    pLayout->addWidget( pEchoMode );
    connect( pEchoMode, SIGNAL(clicked(bool)), SLOT(slotEchoMode(bool)) );
}

void WPasswordWidget::slotEchoMode( bool b )
{
    if ( b )
    {
        pEchoMode->setIcon( QIcon( ":W/EchoModeNormal" ) );
        pPassword->setEchoMode( QLineEdit::Normal );
    }
    else
    {
        pEchoMode->setIcon( QIcon( ":W/EchoModePassword" ) );
        pPassword->setEchoMode( QLineEdit::Password );
    }
}


