/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WLineStyleComboBox.h"

WLineStyleComboBox::WLineStyleComboBox( Qt::PenStyle n, QWidget *pParent )
    : QComboBox( pParent )
{
    setIconSize( QSize( 64, 16 ) );

    addItem( "", (int)Qt::NoPen  );
    setItemIcon( 0, getIcon( Qt::NoPen ) );

    addItem( "", (int)Qt::SolidLine );
    setItemIcon( 1, getIcon( Qt::SolidLine ) );

    addItem( "", (int)Qt::DashLine );
    setItemIcon( 2, getIcon( Qt::DashLine ) );

    addItem( "", (int)Qt::DotLine );
    setItemIcon( 3, getIcon( Qt::DotLine ) );

    addItem( "", (int)Qt::DashDotLine );
    setItemIcon( 4, getIcon( Qt::DashDotLine ) );

    addItem( "", (int)Qt::DashDotDotLine );
    setItemIcon( 5, getIcon( Qt::DashDotDotLine ) );

/*
    addItem( "NoPen", (int)Qt::NoPen  );                 
    setItemIcon( 0, getIcon( Qt::NoPen ) );              
                                                         
    addItem( "Solid", (int)Qt::SolidLine );              
    setItemIcon( 1, getIcon( Qt::SolidLine ) );          
                                                         
    addItem( "DashLine", (int)Qt::DashLine );            
    setItemIcon( 2, getIcon( Qt::DashLine ) );           
                                                         
    addItem( "DotLine", (int)Qt::DotLine );              
    setItemIcon( 3, getIcon( Qt::DotLine ) );            
                                                         
    addItem( "DashDotLine", (int)Qt::DashDotLine );      
    setItemIcon( 4, getIcon( Qt::DashDotLine ) );        
                                                         
    addItem( "DashDotDotLine", (int)Qt::DashDotDotLine );
    setItemIcon( 5, getIcon( Qt::DashDotDotLine ) );     
*/

    setValue( n );

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(slotChanged(int)) );
}

void WLineStyleComboBox::setValue( Qt::PenStyle n )
{
    setCurrentIndex( findData( int(n), int(Qt::UserRole) ) );
}

Qt::PenStyle WLineStyleComboBox::getValue()
{
    return Qt::PenStyle(itemData( currentIndex(), Qt::UserRole ).toInt());
}

QPixmap WLineStyleComboBox::getIcon( Qt::PenStyle nStyle, int nWidth, int nHeight )
{
    QPixmap pixmap( nWidth, nHeight );
    QPainter painter( &pixmap );

    QBrush brush;
    brush.setColor( Qt::white );
    brush.setStyle( Qt::SolidPattern );
    painter.fillRect( 0, 0, nWidth, nHeight, brush );

    QPen pen;
    pen.setColor( Qt::black );
    pen.setStyle( nStyle );
    painter.setPen( pen );
    painter.drawLine( 0, nHeight / 2, nWidth, nHeight / 2 );

    return pixmap;
}

void WLineStyleComboBox::slotChanged( int nIndex )
{
    if ( itemData( nIndex ).isNull() ) return;
    Qt::PenStyle n = Qt::PenStyle(itemData( nIndex ).toInt());
    emit signalChanged( n );
}

 
