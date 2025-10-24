/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LLayersWidget.h"

//
// LLayersListWidget
//
LLayersListWidget::LLayersListWidget( QWidget *pParent )
    : QListWidget( pParent )
{
    setSelectionMode( QAbstractItemView::SingleSelection );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), SLOT(slotCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)) );
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)) );
}

void LLayersListWidget::setLayers( LFile *p )
{
    if ( pLayers ) doDisconnectLayers();
    if ( p ) doConnectLayers( p );
}

void LLayersListWidget::doConnectLayers( LFile *p )
{
    Q_ASSERT( p );
    pLayers = p;
    connect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotRefresh()) );
    connect( pLayers, SIGNAL(signalChangedOrder()), this, SLOT(slotRefresh()) );
    connect( pLayers, SIGNAL(signalChangedCount()), this, SLOT(slotRefresh()) );
    slotRefresh();
}

void LLayersListWidget::doDisconnectLayers()
{
    Q_ASSERT( pLayers );
    disconnect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotRefresh()) );
    disconnect( pLayers, SIGNAL(signalChangedOrder()), this, SLOT(slotRefresh()) );
    disconnect( pLayers, SIGNAL(signalChangedCount()), this, SLOT(slotRefresh()) );
    pLayers = nullptr;
    clear();
}

void LLayersListWidget::slotRefresh()
{
    Q_ASSERT( pLayers );
    clear();

    //
    QList<ADObject*> listChildren = pLayers->getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {
        if ( !pChild->inherits( "LLayer" ) ) continue;
        LLayer *pLayer = dynamic_cast<LLayer*>( pChild );
        addItem( new QListWidgetItem( pLayer->getIcon(), pLayer->getName() ) );
    }
    int n = pLayers->getLayerCurrentIndex();
    if ( n >=0 ) setCurrentRow( n );
}

void LLayersListWidget::slotCurrentItemChanged( QListWidgetItem *pCurrent, QListWidgetItem *pPrevious )
{
    Q_UNUSED( pPrevious );
//    Q_ASSERT( pCurrent );

    if ( !pCurrent ) return;

    pLayers->setLayerCurrentIndex( indexFromItem( pCurrent ).row() );
}

void LLayersListWidget::slotContextMenu( const QPoint &point )
{
    if ( !pLayers ) return;
    QPoint pointGlobal = mapToGlobal( point );
    QMenu *pMenu = pLayers->getLayerContextMenu( this );
    if ( !pMenu ) return;
    pMenu->exec( pointGlobal );
    delete pMenu;
}

//
// LLayersControlWidget
//
LLayersControlWidget::LLayersControlWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pVisible = new QToolButton( this );
    pVisible->setToolTip( tr("toggle visibility") );
    pVisible->setText( "<o>" );
    pVisible->setCheckable( true );
    pVisible->setDisabled( true );
    pLayout->addWidget( pVisible );
    connect( pVisible, SIGNAL(clicked()), SLOT(slotVisible()) );

    pBack = new QToolButton( this );
    pBack->setToolTip( tr("move layer back") );
    pBack->setText( "^" );
    pBack->setDisabled( true );
    pLayout->addWidget( pBack );
    connect( pBack, SIGNAL(clicked()), SLOT(slotBack()) );

    pForward = new QToolButton( this );
    pForward->setToolTip( tr("move layer forward") );
    pForward->setText( "v" );
    pForward->setDisabled( true );
    pLayout->addWidget( pForward );
    connect( pForward, SIGNAL(clicked()), SLOT(slotForward()) );

    pAdd = new QToolButton( this );
    pAdd->setToolTip( tr("add layer") );
    pAdd->setText( "+" );
    pAdd->setDisabled( true );
    pLayout->addWidget( pAdd );
    connect( pAdd, SIGNAL(clicked()), SLOT(slotAdd()) );

    pRemove = new QToolButton( this );
    pRemove->setToolTip( tr("delete layer") );
    pRemove->setText( "-" );
    pRemove->setDisabled( true );
    pLayout->addWidget( pRemove );
    connect( pRemove, SIGNAL(clicked()), SLOT(slotRemove()) );

    pLayout->addStretch( 10 );
}

void LLayersControlWidget::setLayers( LFile *p )
{
    if ( pLayers ) doDisconnectLayers();
    if ( p ) doConnectLayers( p );
}

void LLayersControlWidget::doConnectLayers( LFile *p )
{
    Q_ASSERT( p );
    pLayers = p;
    connect( pLayers, SIGNAL( signalChangedCurrent() ), this, SLOT(slotRefresh()) );
    connect( pLayers, SIGNAL(signalChangedOrder()), this, SLOT(slotRefresh()) );
    connect( pLayers, SIGNAL(signalChangedCount()), this, SLOT(slotRefresh()) );
    slotRefresh();
}

void LLayersControlWidget::doDisconnectLayers()
{
    Q_ASSERT( pLayers );
    disconnect( pLayers, SIGNAL( signalChangedCurrent() ), this, SLOT(slotRefresh()) );
    disconnect( pLayers, SIGNAL(signalChangedOrder()), this, SLOT(slotRefresh()) );
    disconnect( pLayers, SIGNAL(signalChangedCount()), this, SLOT(slotRefresh()) );

    pLayers = nullptr;
    slotRefresh();
}

void LLayersControlWidget::slotRefresh()
{
    pVisible->setText( "<o>" );
    pVisible->setDisabled( true );
    pBack->setDisabled( true );
    pForward->setDisabled( true );
    pAdd->setDisabled( true );
    pRemove->setDisabled( true );

    if ( !pLayers ) return;

    pAdd->setEnabled( true );

    LLayer *pLayer  = pLayers->getLayerCurrent();
    if ( !pLayer ) return;

    int nLayer      = pLayers->getLayerCurrentIndex();
    int nLayerLast  = pLayers->getLayerCount() - 1;

    if ( pLayer->isVisible() ) pVisible->setText( "<o>" );
    else pVisible->setText( "<>" );

    if ( nLayer >= 0 ) pVisible->setEnabled( true );            // have a layer
    if ( nLayer > 0 ) pBack->setEnabled( true );                // layer not at back
    if ( nLayer < nLayerLast ) pForward->setEnabled( true );    // layer not at front
    if ( nLayer >= 0 ) pRemove->setEnabled( true );             // must have a current layer
}

void LLayersControlWidget::slotVisible()
{
    LLayer *pLayer = pLayers->getLayerCurrent();
    Q_ASSERT( pLayer );
    pLayer->setVisible( !pLayer->isVisible() );

    if ( pLayer->isVisible() ) pVisible->setText( "<o>" );
    else pVisible->setText( "<>" );
}

void LLayersControlWidget::slotBack()
{
    Q_ASSERT( pLayers );
    pLayers->doLayerToPrev();
}

void LLayersControlWidget::slotForward()
{
    Q_ASSERT( pLayers );
    pLayers->doLayerToNext();
}

/*!
 * \brief Handle request to add a new layer.
 * 
 * \author pharvey (2/16/25)
 */
void LLayersControlWidget::slotAdd()
{
    QMenu menu;                                                         

    QStringList listClasses = pLayers->getClasses( false );
    foreach( QString s, listClasses ) 
    {
        ADMeta meta = pLayers->getMeta( s );
        menu.addAction( meta.icon, meta.stringFriendlyName );
    }

    QAction *pAction = menu.exec( pAdd->mapToGlobal( QPoint( 0, 0 ) ) );
    if ( pAction )
    {
        pLayers->doLayerNew( pAction->text() );
    }
}

void LLayersControlWidget::slotRemove()
{
    LLayer *pLayer = pLayers->getLayerCurrent();
    Q_ASSERT( pLayer );
    pLayers->doLayerDelete();
}

//
// LLayersWidget
//
LLayersWidget::LLayersWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pListWidget = new LLayersListWidget( this );
    pLayout->addWidget( pListWidget, 10 );

    pControlWidget = new LLayersControlWidget( this );
    pLayout->addWidget( pControlWidget );
}

void LLayersWidget::setLayers( LFile *p )
{
    pListWidget->setLayers( p );
    pControlWidget->setLayers( p );
}


