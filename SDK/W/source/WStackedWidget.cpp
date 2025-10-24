/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WStackedWidget.h"

WStackedWidget::WStackedWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    pComboBox = new QComboBox( this );
    pLayoutTop->addWidget( pComboBox );
    connect( pComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotIndex(int)) );

    pStackedWidget = new QStackedWidget( this );
    pLayoutTop->addWidget( pStackedWidget, 10 );
}

void WStackedWidget::setName( int nIndex, const QString &stringName )
{
    pComboBox->setItemText( nIndex, stringName );
}

void WStackedWidget::addWidget( const QString &stringName, QWidget *pWidget )
{
    // scroll bars may be needed so...
    QScrollArea *pScroll = new QScrollArea();
    pScroll->setWidgetResizable( true );
    pScroll->setWidget( pWidget ); // auto reparented

    // we use this to xref pWidget and pScroll... 
    vectorWidgets.append( pWidget );

    //
    pStackedWidget->addWidget( pScroll ); // auto reparented

    // last as it will trigger a call to slotIndex
    pComboBox->addItem( stringName );
}

void WStackedWidget::removeWidget( const QString &stringName )
{
    int nIndex = pComboBox->findText( stringName );
    if ( nIndex < 0 ) return;
    disconnect( pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotIndex(int)) );

    pComboBox->removeItem( nIndex );
    vectorWidgets.remove( nIndex );
    pStackedWidget->removeWidget( pStackedWidget->widget( nIndex ) );

    connect( pComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotIndex(int)) );
}

void WStackedWidget::removeWidget( QWidget *pWidget )
{
    int nIndex = vectorWidgets.indexOf( pWidget );
    if ( nIndex < 0 ) return;
    disconnect( pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotIndex(int)) );

    pComboBox->removeItem( nIndex );
    vectorWidgets.remove( nIndex );
    pStackedWidget->removeWidget( pWidget );

    connect( pComboBox, SIGNAL(currentIndexChanged(int)), SLOT(slotIndex(int)) );
}

void WStackedWidget::setCurrent( const QString &stringName )
{
    pComboBox->setCurrentIndex( pComboBox->findText( stringName ) );
}

void WStackedWidget::setCurrent( QWidget *pWidget )
{
    pComboBox->setCurrentIndex( vectorWidgets.indexOf( pWidget ) );
}

int WStackedWidget::count() 
{ 
    return vectorWidgets.count(); 
}

QWidget *WStackedWidget::widget( int nIndex ) 
{
    Q_ASSERT( nIndex >= 0 );
    Q_ASSERT( nIndex < vectorWidgets.count() ); 
    return vectorWidgets.at( nIndex ); 
}

void WStackedWidget::slotIndex( int nIndex )
{
   pStackedWidget->setCurrentIndex( nIndex );
   emit signalCurrentChanged( ( nIndex < 0 ? nullptr : vectorWidgets.at( nIndex ) ) );
   emit signalCurrentChanged( pComboBox->currentText() );
}


