/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 *
 * This file is part of Euphemia-SDK.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LibInfo.h"
#include "WGeometryWidget.h"
#include "W.h"

#include "WPosWidget.h"
#include "WSizeWidget.h"

WGeometryWidget::WGeometryWidget( const QPointF &pointPos, const QSizeF &size, QWidget *pParent, bool bToolBar )
    : QWidget( pParent )
{
    QLayout *pLayoutTop;

    if ( bToolBar )
    { 
        QHBoxLayout *p = new QHBoxLayout( this );
        pLayoutTop = p;
    }
    else
    {
        QVBoxLayout *p = new QVBoxLayout( this );
        pLayoutTop = p;
    }
    // pos
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Position"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pPosWidget = new WPosWidget( pointPos, pGroupBox );
        pLayout->addWidget( pPosWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pPosWidget, SIGNAL(signalChanged(const QPointF &)), SIGNAL(signalChangedPos(const QPointF &)) );
    }
    // size
    {
        QGroupBox *     pGroupBox    = new QGroupBox( tr("Size"), this );
        QVBoxLayout *   pLayout      = new QVBoxLayout( pGroupBox );

        pSizeWidget = new WSizeWidgetDouble( size, pGroupBox );
        pLayout->addWidget( pSizeWidget );

        pLayoutTop->addWidget( pGroupBox );

        connect( pSizeWidget, SIGNAL(signalChanged(const QSizeF &)), SIGNAL(signalChangedSize(const QSizeF &)) );
    }
}

void WGeometryWidget::setPos( const QPointF &pointPos )
{
    pPosWidget->setValue( pointPos );
}

void WGeometryWidget::setSize( const QSizeF &size )
{
    pSizeWidget->setValue( size );
}

QPointF WGeometryWidget::getPos()                    { return pPosWidget->getValue();                }
QSizeF  WGeometryWidget::getSize()                   { return pSizeWidget->getValue();                }

void WGeometryWidget::slotPos( const QPointF &pointPos )
{
    setPos( pointPos );
}

void WGeometryWidget::slotSize( const QSizeF &size )
{
    setSize( size );
}


