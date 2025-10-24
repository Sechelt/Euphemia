/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WTextHAlignComboBox.h"

WTextHAlignComboBox::WTextHAlignComboBox( Qt::AlignmentFlag n, QWidget *pParent, bool bText )
    : QComboBox( pParent )
{
    addItem( ( bText ? tr("Left") : "" ), (int)Qt::AlignLeft  );
    setItemIcon( 0, QIcon( ":W/AlignLeft16x16" ) );

    addItem( ( bText ? tr("Center") : "" ), (int)Qt::AlignHCenter );
    setItemIcon( 1, QIcon( ":W/AlignHCenter16x16" ) );

    addItem( ( bText ? tr("Right") : "" ), (int)Qt::AlignRight );
    setItemIcon( 2, QIcon( ":W/AlignRight16x16" ) );

    setValue( n );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );

    setToolTip( tr("horizontal text alignment") );
}

void WTextHAlignComboBox::setValue( Qt::AlignmentFlag n )
{
    setCurrentIndex( findData( int(n), int(Qt::UserRole) ) );
}

Qt::AlignmentFlag WTextHAlignComboBox::getValue()
{
    return Qt::AlignmentFlag(itemData( currentIndex(), Qt::UserRole ).toInt());
}

void WTextHAlignComboBox::slotValue( Qt::AlignmentFlag n )
{
    setValue( n );
}

void WTextHAlignComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    Qt::AlignmentFlag n = Qt::AlignmentFlag(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
