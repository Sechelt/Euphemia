/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WColorsWidget.h"

#include <SColors.h>

#include "WColorsPickerDialog.h"

#define CellWidth 16
#define CellHeight 16

//
// WColorsMatrix
//
WColorsMatrix::WColorsMatrix( QWidget *pParent )
    : QWidget( pParent )
{
    vectorColors = g_SColors->getColors();
    setMinimumWidth( CellWidth * 3 );
    setMinimumHeight( CellHeight * 3 );

    connect( g_SColors, SIGNAL(signalModifiedPalette()), SLOT(slotRefresh()) );
}


void WColorsMatrix::slotRefresh()
{
    vectorColors = g_SColors->getColors();
    update();
}

/*!
 * \brief Click to select cell making it the selected color.
 * 
 * \author pharvey (11/29/22)
 * 
 * \param pEvent 
 */
void WColorsMatrix::mousePressEvent( QMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton )
    {
        QWidget::mousePressEvent( pEvent );
        return;
    }

    int nIndex = getIndex( pEvent->pos() );

    if ( nIndex >= vectorColors.count() ) return;

    emit signalSelected( vectorColors[nIndex] );
}

/*!
 * \brief Double click left mouse button to edit cell/color.
 *  
 * Also selects the cell making it current as mousePressEvent will get invoked before we get here. 
 * Any changes will be made to the global palette. 
 *  
 * \author pharvey (11/29/22)
 * 
 * \param pEvent 
 */
void WColorsMatrix::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    if ( pEvent->button() != Qt::LeftButton )
    {
        QWidget::mouseReleaseEvent( pEvent );
        return;
    }

    int nIndex = getIndex( pEvent->pos() );
    Q_ASSERT( nIndex >= 0 );

    QColor color;
    bool bOk = false;

    if ( nIndex < vectorColors.count() ) color = vectorColors[nIndex];
    QColor colorNew = WColorsPickerDialog::getColor( &bOk, color, this );
    if ( bOk )
    {
        g_SColors->setValue( nIndex, colorNew );
        emit signalSelected( colorNew );
    }
}

void WColorsMatrix::paintEvent( QPaintEvent *pEvent )
{
    Q_UNUSED(pEvent);

    QPainter painter( this );
    painter.setPen( QPen( Qt::black ) );

    QBrush brush( Qt::SolidPattern );
    QRect r = rect();
    int nColumns = r.width() / CellWidth;

    int nX = 0;
    int nY = 0;
    int nIndex = 0;
    do
    {
        // IF end of row THEN continue at next row
        if ( nX / CellWidth >= nColumns )
        {
             nX = 0;
             nY += CellHeight;
        }

        // IF no room for row THEN we are done
        if ( nY + CellHeight > rect().height() ) break;

        // IF we have color THEN draw using color ELSE draw with no color
        if ( nIndex < vectorColors.count() )
        {
            if ( vectorColors[nIndex].alpha() == 0 )
            {
                painter.setBrush( QBrush( QImage( ":W/Transparent" ) ) );       // transparent so show checkerd pattern
            }
            else
            {
                painter.setBrush( QBrush( vectorColors[nIndex] ) );             // palette color
            }
            nIndex++;
        }
        else
        {
            painter.setBrush( QBrush( QColor( Qt::transparent ) ) );            // NoOp
        }

        painter.drawRect( nX, nY, CellWidth, CellHeight );

        // next cell
        nX += CellWidth;
    }
    while ( 1 );
}

int WColorsMatrix::getIndex( const QPoint &point )
{
    int nColumns    = rect().width() / CellWidth;
    int nColumn     = point.x() / CellWidth;
    int nRow        = point.y() / CellHeight;

    int nIndex = nRow * nColumns + nColumn;

    return nIndex;
}

//
// WColorsWidget
//
WColorsWidget::WColorsWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // palette
    {
        WColorsMatrix *p = new WColorsMatrix( this );
        pLayoutTop->addWidget( p, 11 );
        connect( p, SIGNAL(signalSelected(const QColor &)), SIGNAL(signalSelected(const QColor &)) );
    }

    // tool bar
    {
        QHBoxLayout *pLayoutToolBar = new QHBoxLayout();
        pLoad       = new QToolButton( this );
        pSave       = new QToolButton( this );
        pSaveAs     = new QToolButton( this );
        pReset      = new QToolButton( this );
        pModified   = new QLabel( this );

        pLoad->setIcon( QIcon( ":W/FileOpen16x16" ) );
        pSave->setIcon( QIcon( ":W/Save16x16" ) );
        pSaveAs->setIcon( QIcon( ":W/Save16x16" ) );
        pReset->setIcon( QIcon( ":W/ResetToDefault" ) );

        pLoad->setToolTip( tr("load color palette") );
        pSave->setToolTip( tr("save color palette") );
        pSaveAs->setToolTip( tr("save color palette as") );
        pReset->setToolTip( tr("reset to default") );
        pModified->setToolTip( tr("modified status") );

        pModified->setFixedSize( 16, 16 );

        pLayoutToolBar->addWidget( pLoad );
        pLayoutToolBar->addWidget( pSave );
        pLayoutToolBar->addWidget( pSaveAs );
        pLayoutToolBar->addWidget( pReset );
        pLayoutToolBar->addStretch( 10 );
        pLayoutToolBar->addWidget( pModified );

        pLayoutTop->addLayout( pLayoutToolBar );

        connect( pLoad, SIGNAL(clicked()), g_SColors, SLOT(slotLoad()) );
        connect( pSave, SIGNAL(clicked()), g_SColors, SLOT(slotSave()) );
        connect( pSaveAs, SIGNAL(clicked()), g_SColors, SLOT(slotSaveAs()) );
        connect( pReset, SIGNAL(clicked()), g_SColors, SLOT(slotReset()) );

        connect( g_SColors, SIGNAL(signalModifiedPalette()), SLOT(slotRefresh()) );
    }
}

void WColorsWidget::slotRefresh( const QColor & )
{
}

void WColorsWidget::slotRefresh()
{
    if ( g_SColors->isModified() )
    {
        pSave->setEnabled( true );
        pModified->setPixmap( QPixmap( ":W/Draw16x16" ) );
    }
    else
    {
        pSave->setEnabled( false ); 
        pModified->setPixmap( QPixmap() );
    }
}


