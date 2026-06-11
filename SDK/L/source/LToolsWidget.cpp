/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LToolsWidget.h"

#include <WIconLayout.h>

//
// LToolsWidget
//
LToolsWidget::LToolsWidget( QWidget *pParent )
    : QWidget( pParent )
{
    // put the new widget into a scroll area so we get scroll bars as needed
    pScrollArea = new QScrollArea( this );
    pScrollArea->setWidgetResizable( true );

    // put it all into a layout on this widget
    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pLayout->addWidget( pScrollArea );
}

void LToolsWidget::setLayers( LFile *p )
{
    if ( pLayers ) doDisconnectLayers();
    if ( p ) doConnectLayers( p );
}

void LToolsWidget::doConnectLayers( LFile *p )
{
    Q_ASSERT( p );
    Q_ASSERT( !pLayers );
    pLayers = p;
    LLayer *p2 = p->getLayerCurrent();
    if ( p2 ) doConnectLayer( p2 );

    connect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrent()) );
}

void LToolsWidget::doDisconnectLayers()
{
    Q_ASSERT( pLayers );
    disconnect( pLayers, SIGNAL(signalChangedCurrent()), this, SLOT(slotChangedCurrent()) );
    if ( pLayer ) doDisconnectLayer();
    pLayers = nullptr;
}

void LToolsWidget::doConnectLayer( LLayer *p )
{
    Q_ASSERT( pLayers );
    Q_ASSERT( !pLayer );
    pLayer = p;
    if ( !pLayer ) return;

    doInitActions( pLayer->getTools() );
    doInitButtons();

    connect( pLayer, SIGNAL(signalChangedTool()), this, SLOT(slotChangedTool()) );
}

void LToolsWidget::doDisconnectLayer()
{
    Q_ASSERT( pLayers );
    Q_ASSERT( pLayer );

    // catch case where current layer was deleted
    if ( pLayers->hasLayer( pLayer ) )
    {
        disconnect( pLayer, SIGNAL( signalChangedTool() ), this, SLOT( slotChangedTool() ) );
    }

    doFiniButtons();
    doFiniActions();

    pLayer = nullptr;
}

void LToolsWidget::doConnectTool( int n )
{
    Q_ASSERT( nTool < 0 );
    Q_ASSERT( n >= 0 );

    // find the action
    QAction *p = nullptr;
    foreach( QAction *pAction, vectorActions )
    {
        if ( pAction->property( "Tool" ).toInt() == n )
        {
            p = pAction;
            break;
        }
    }

    Q_ASSERT( p );

    // make it current
    p->setChecked( true );
    nTool = n;
}

void LToolsWidget::doDisconnectTool()
{
    Q_ASSERT( nTool >= 0 );
    // find the action
    QAction *p = nullptr;
    foreach( QAction *pAction, vectorActions )
    {
        if ( pAction->property( "Tool" ).toInt() == nTool )
        {
            p = pAction;
            break;
        }
    }

    Q_ASSERT( p );

    // make it current
    p->setChecked( false );
    nTool = -1;
}

void LToolsWidget::doInitActions( QVector<LTool> vectorTools )
{
    Q_ASSERT( !pActionGroup );

    int nTool = pLayer->getTool();
    pActionGroup = new QActionGroup( this );
    foreach( LTool tool, vectorTools )
    {
        QAction *pAction = new QAction( tool.pixmap, tool.stringText, pActionGroup );
        pAction->setToolTip( tool.stringToolTip );
        pAction->setCheckable( true );
        pAction->setProperty( "Tool", tool.nID );
        if ( tool.nID == nTool ) pAction->setChecked( true );
        vectorActions.append( pAction );
    }

    connect( pActionGroup, SIGNAL(triggered(QAction*)), SLOT(slotTriggered(QAction*)) );
}

void LToolsWidget::doFiniActions()
{
    Q_ASSERT( pActionGroup );
    // no need to disconnect pActionGroup as we will delete the pActionGroup
    delete pActionGroup;
    pActionGroup = nullptr;
    vectorActions.clear();
}

void LToolsWidget::doInitButtons()
{
    Q_ASSERT( !pScrollableWidget );

    // create widget to scroll
    pScrollableWidget = new QWidget( this );
    WIconLayout *pIconLayout = new WIconLayout( pScrollableWidget );
    foreach( QAction *pAction, vectorActions )
    {
        QToolButton *pButton = new QToolButton( this );
        pButton->setDefaultAction( pAction );
        pIconLayout->addWidget( pButton );
    }
    pScrollArea->setWidget( pScrollableWidget );
}

void LToolsWidget::doFiniButtons()
{
    Q_ASSERT( pScrollableWidget );

    pScrollArea->setWidget( nullptr );

    delete pScrollableWidget;
    pScrollableWidget = nullptr;
}

void LToolsWidget::slotTriggered( QAction *p )
{
    nTool = p->property( "Tool" ).toInt();
    pLayer->setTool( nTool ); // this is a request if accepted we will get slotChangedTool
}

void LToolsWidget::slotChangedCurrent()
{
    if ( pLayer ) doDisconnectLayer();
    LLayer *p = pLayers->getLayerCurrent();
    if ( p ) doConnectLayer( p );
}

void LToolsWidget::slotChangedTool()
{
    Q_ASSERT( pLayer );

    if ( nTool >= 0 || vectorActions.count() ) doDisconnectTool();
    int n = pLayer->getTool();
    if ( n >= 0 ) doConnectTool( n );
}


