/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WTextVAlignComboBox.h"

WTextVAlignComboBox::WTextVAlignComboBox( Qt::AlignmentFlag n, QWidget *pParent, bool bText )
    : QComboBox( pParent )
{
    addItem( ( bText ? tr("Top") : "" ), (int)Qt::AlignTop );
    setItemIcon( 0, QIcon( ":W/AlignTop16x16" ) );

    addItem( ( bText ? tr("Center") : "" ), (int)Qt::AlignVCenter );
    setItemIcon( 1, QIcon( ":W/AlignVCenter16x16" ) );

    addItem( ( bText ? tr("Bottom") : "" ), (int)Qt::AlignBottom );
    setItemIcon( 2, QIcon( ":W/AlignBottom16x16" ) );

    setValue( n );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );

    setToolTip( tr("vertical text alignment") );
}

void WTextVAlignComboBox::setValue( Qt::AlignmentFlag n )
{
    setCurrentIndex( findData( int(n), int(Qt::UserRole) ) );
}

Qt::AlignmentFlag WTextVAlignComboBox::getValue()
{
    return Qt::AlignmentFlag(itemData( currentIndex(), Qt::UserRole ).toInt());
}

void WTextVAlignComboBox::slotValue( Qt::AlignmentFlag n )
{
    setValue( n );
}

void WTextVAlignComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    Qt::AlignmentFlag n = Qt::AlignmentFlag(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
