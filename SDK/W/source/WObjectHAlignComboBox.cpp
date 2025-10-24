/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WObjectHAlignComboBox.h"

WObjectHAlignComboBox::WObjectHAlignComboBox( Qt::AlignmentFlag n, QWidget *pParent, bool bText )
    : QComboBox( pParent )
{
    addItem( ( bText ? tr("Left") : "" ), (int)Qt::AlignLeft );
    setItemIcon( 0, QIcon( ":W/ObjAlignLeft16x16" ) );

    addItem( ( bText ? tr("Center") : "" ), (int)Qt::AlignHCenter );
    setItemIcon( 1, QIcon( ":W/ObjAlignHCenter16x16" ) );

    addItem( ( bText ? tr("Right") : "" ), (int)Qt::AlignRight );
    setItemIcon( 2, QIcon( ":W/ObjAlignRight16x16" ) );

    setValue( n );

    // send when changed
    // connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );

    // send even when not changed
    connect( this, SIGNAL(activated(int)), SLOT(slotChanged(int)) );

    setToolTip( tr("horizontal object alignment") );
}

void WObjectHAlignComboBox::setValue( Qt::AlignmentFlag n )
{
    setCurrentIndex( findData( int(n), int(Qt::UserRole) ) );
}

Qt::AlignmentFlag WObjectHAlignComboBox::getValue()
{
    return Qt::AlignmentFlag(itemData( currentIndex(), Qt::UserRole ).toInt());
}

void WObjectHAlignComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    Qt::AlignmentFlag n = Qt::AlignmentFlag(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
