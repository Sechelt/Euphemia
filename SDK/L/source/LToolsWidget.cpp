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
    : QSplitter( pParent )
{
    setOrientation( Qt::Vertical );

    // top area is a scroll area containing all tool actions
    {
        pScrollAreaTools = new QScrollArea( this );
        pScrollAreaTools->setToolTip( tr("tools") );
        pScrollAreaTools->setWidgetResizable( true );
        addWidget( pScrollAreaTools );
    }

    // bottom area is the settings panel (if any) for the currently selected tool
    {
        pScrollAreaPanel = new QScrollArea( this );
        pScrollAreaPanel->setToolTip( tr("settings panel for active tool (if available)") );
        pScrollAreaPanel->setWidgetResizable( true );
        addWidget( pScrollAreaPanel );
    }
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
    doInitPanel();

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

    doFiniPanel();
    doFiniButtons();
    doFiniActions();

    pLayer = nullptr;
}

void LToolsWidget::doConnectTool( int n )
{
    Q_ASSERT( nTool < 0 );  // must NOT be connected to a tool
    Q_ASSERT( n >= 0 );     // requested tool must be a valid tool

    // set tool button active
    {
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

    // set panel (if available)
    doInitPanel();
}

void LToolsWidget::doDisconnectTool()
{
    // set tool button inactive
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

    // get rid of panel (if there is one)
    doFiniPanel();
}

void LToolsWidget::doInitActions( QVector<LTool> vectorTools )
{
    Q_ASSERT( !pActionGroup );

    int nTool = pLayer->getTool();
    pActionGroup = new QActionGroup( this );
    foreach( LTool tool, vectorTools )
    {
        // create a modified version to (better) show when selected
        QIcon icon( tool.pixmap );
        {
            QPixmap pixmapSelected = tool.pixmap;
            QPainter painter( &pixmapSelected );
            QPen pen( QColor( Qt::darkMagenta ) );
            pen.setWidth( 10 );
            painter.setPen( pen );
            painter.drawRect( 0, pixmapSelected.height() - 8, pixmapSelected.width(), pixmapSelected.height() - 8 );
            icon.addPixmap( pixmapSelected, QIcon::Normal, QIcon::On );
        }
        // add the tool action
        QAction *pAction = new QAction( icon, tool.stringText, pActionGroup );
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
    // init tool buttons
    Q_ASSERT( !pScrollableWidgetTools );

    // create widget to scroll
    pScrollableWidgetTools = new QWidget( this );
    WIconLayout *pIconLayout = new WIconLayout( pScrollableWidgetTools );
    foreach( QAction *pAction, vectorActions )
    {
        QToolButton *pButton = new QToolButton( this );
        pButton->setDefaultAction( pAction );
        pIconLayout->addWidget( pButton );
    }
    pScrollAreaTools->setWidget( pScrollableWidgetTools );
}

void LToolsWidget::doFiniButtons()
{
    Q_ASSERT( pScrollableWidgetTools );

    pScrollAreaTools->setWidget( nullptr );

    delete pScrollableWidgetTools;
    pScrollableWidgetTools = nullptr;
}

void LToolsWidget::doInitPanel()
{
    Q_ASSERT( pLayer );
    Q_ASSERT( !pScrollableWidgetPanel );
    pScrollableWidgetPanel = pLayer->getPanel( this );
    if ( pScrollableWidgetPanel ) pScrollAreaPanel->setWidget( pScrollableWidgetPanel );
}

void LToolsWidget::doFiniPanel()
{
    if ( pScrollableWidgetPanel ) delete pScrollableWidgetPanel;
    pScrollableWidgetPanel = nullptr;
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


