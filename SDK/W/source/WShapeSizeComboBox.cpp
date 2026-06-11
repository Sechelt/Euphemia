/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WShapeSizeComboBox.h"

WShapeSizeComboBox::WShapeSizeComboBox( QWidget *pParent )
    : QComboBox( pParent )
{
    addItem( "", 0 );
    addItem( "16", 16 );
    addItem( "32", 32 );
    addItem( "48", 48 );
    addItem( "64", 64 );
    addItem( "96", 96 );
    addItem( "128", 128 );

    setValue( 0 );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );

    setToolTip( tr("common shape sizes") );
}

void WShapeSizeComboBox::setValue( int n )
{
    setCurrentIndex( findData( n, int(Qt::UserRole) ) );
}

int WShapeSizeComboBox::getValue()
{
    return itemData( currentIndex(), Qt::UserRole ).toInt();
}

void WShapeSizeComboBox::slotValue( int n )
{
    setValue( n );
}

void WShapeSizeComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    int n = itemData( nIndex ).toInt();
    emit signalChanged( n );
}

 
