/*
 * Copyright 2022-2023, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Merva codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "WValueListWidget.h"
    
WValueListWidget::WValueListWidget( const QStringList &listStrings, QWidget *pwidgetParent, const QString &stringName )
    : QMainWindow( pwidgetParent )
{
    setObjectName( stringName );

    // list widget
    pListWidget = new QListWidget( this );
    pListWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    pListWidget->setSelectionMode( QAbstractItemView::SingleSelection );

    QString s;
    foreach( s, listStrings )
    {
        QListWidgetItem *pItem = new QListWidgetItem( pListWidget );
        pItem->setText( s );
        pItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
    }

    setCentralWidget( pListWidget );

    // TOOLBAR
    pActionNew      = new QAction( QIcon( ":W/RowNew16x16" ), tr("New"), this );
    pActionDelete   = new QAction( QIcon( ":W/RowDelete16x16" ), tr("Delete"), this );
    pActionUp       = new QAction( QIcon( ":W/RowUp16x16" ), tr("Up"), this );
    pActionDown     = new QAction( QIcon( ":W/RowDown16x16" ), tr("Down"), this );

    connect( pActionNew, SIGNAL(triggered()), this, SLOT(slotNew()) );
    connect( pActionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()) );
    connect( pActionUp, SIGNAL(triggered()), this, SLOT(slotUp()) );
    connect( pActionDown, SIGNAL(triggered()), this, SLOT(slotDown()) );

    connect( pListWidget, SIGNAL(currentRowChanged(int)), SLOT(slotRowChanged(int)) );

    QToolBar *pToolBar = addToolBar( tr("") );
    pToolBar->addAction( pActionNew );
    pToolBar->addAction( pActionDelete );
    pToolBar->addAction( pActionUp );
    pToolBar->addAction( pActionDown );
}

WValueListWidget::~WValueListWidget()
{
}

QStringList WValueListWidget::getValues()
{
    QStringList listStrings;

    for ( int nRow = 0; nRow < pListWidget->count(); nRow++ ) 
    {
        listStrings.append( pListWidget->item( nRow )->text() );
    }

    return listStrings;
}

void WValueListWidget::slotRowChanged( int )
{
    emit signalChanged( getValues() );
}

void WValueListWidget::slotNew()
{
    QListWidgetItem *pItem = new QListWidgetItem( pListWidget );
    pItem->setText( "" );
    pItem->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled );
    pListWidget->setCurrentRow( pListWidget->count() - 1 );
}

void WValueListWidget::slotDelete()
{
    QListWidgetItem *p = pListWidget->takeItem( pListWidget->currentRow() );
    if ( !p ) return;
    delete p;
}

void WValueListWidget::slotUp()
{
    int nRow = pListWidget->currentRow();
    if ( nRow < 1 ) return;
    QListWidgetItem *p = pListWidget->takeItem( pListWidget->currentRow() );
    if ( !p ) return;
    pListWidget->insertItem( nRow--, p );
}

void WValueListWidget::slotDown()
{
    int nRow = pListWidget->currentRow();
    if ( nRow >= pListWidget->count() - 1 ) return;
    QListWidgetItem *p = pListWidget->takeItem( pListWidget->currentRow() );
    if ( !p ) return;
    pListWidget->insertItem( nRow++, p );
}


