/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WBrushStyleComboBox.h"

WBrushStyleComboBox::WBrushStyleComboBox( QWidget *pParent, Qt::BrushStyle n )
    : QComboBox( pParent )
{
    setIconSize( QSize( 32, 16 ) );

    addItem( "", (int)Qt::NoBrush  );
    setItemIcon( 0, getIcon( Qt::NoBrush ) );

    addItem( "", (int)Qt::SolidPattern );
    setItemIcon( 1, getIcon( Qt::SolidPattern ) );

    addItem( "", (int)Qt::Dense1Pattern );
    setItemIcon( 2, getIcon( Qt::Dense1Pattern ) );

    addItem( "", (int)Qt::Dense2Pattern );
    setItemIcon( 3, getIcon( Qt::Dense2Pattern ) );

    addItem( "", (int)Qt::Dense3Pattern );
    setItemIcon( 4, getIcon( Qt::Dense3Pattern ) );

    addItem( "", (int)Qt::Dense4Pattern );
    setItemIcon( 5, getIcon( Qt::Dense4Pattern ) );

    addItem( "", (int)Qt::Dense5Pattern );
    setItemIcon( 6, getIcon( Qt::Dense5Pattern ) );

    addItem( "", (int)Qt::Dense6Pattern );
    setItemIcon( 7, getIcon( Qt::Dense6Pattern ) );

    addItem( "", (int)Qt::Dense7Pattern );
    setItemIcon( 8, getIcon( Qt::Dense7Pattern ) );

    addItem( "", (int)Qt::HorPattern );
    setItemIcon( 9, getIcon( Qt::HorPattern ) );

    addItem( "", (int)Qt::VerPattern );
    setItemIcon( 10, getIcon( Qt::VerPattern ) );

    addItem( "", (int)Qt::CrossPattern );
    setItemIcon( 11, getIcon( Qt::CrossPattern ) );

    addItem( "", (int)Qt::BDiagPattern );
    setItemIcon( 12, getIcon( Qt::BDiagPattern ) );

    addItem( "", (int)Qt::FDiagPattern );
    setItemIcon( 13, getIcon( Qt::FDiagPattern ) );

    addItem( "", (int)Qt::DiagCrossPattern );
    setItemIcon( 14, getIcon( Qt::DiagCrossPattern ) );

    setValue( n );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );
}

void WBrushStyleComboBox::setValue( Qt::BrushStyle n )
{
    setCurrentIndex( findData( int(n), int(Qt::UserRole) ) );
}

Qt::BrushStyle WBrushStyleComboBox::getValue()
{
    return Qt::BrushStyle(itemData( currentIndex(), Qt::UserRole ).toInt());
}

QPixmap WBrushStyleComboBox::getIcon( Qt::BrushStyle nStyle, int nWidth, int nHeight )
{
    QPixmap pixmap( nWidth, nHeight );
    QPainter painter( &pixmap );

    QBrush brush;

    // background white
    brush.setColor( Qt::white );
    brush.setStyle( Qt::SolidPattern );
    painter.fillRect( 0, 0, nWidth, nHeight, brush );

    // foreground black
    brush.setColor( Qt::black );
    brush.setStyle( nStyle );
    painter.fillRect( 0, 0, nWidth, nHeight, brush );

    return pixmap;
}

void WBrushStyleComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    Qt::BrushStyle n = Qt::BrushStyle(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
