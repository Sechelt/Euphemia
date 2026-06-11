/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "AppInfo.h"
#include "SAPreferencesDialog.h"

#include <SSettings.h>

// SSetting classes we know about...
#include <SPen.h>       
#include <SBrush.h>     
#include <SFont.h>      
#include <SText.h>      
#include <SRuler.h>       
#include <SGrid.h>       

SAPreferencesDialog::SAPreferencesDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Preferences") );

    QVBoxLayout *pLayoutDialog = new QVBoxLayout( this );

    pSplitter = new QSplitter( Qt::Horizontal, this );
    pSplitter->setChildrenCollapsible( false );
    pSplitter->setStretchFactor( 1, 10 );
    pLayoutDialog->addWidget( pSplitter );

    pTree = new QTreeWidget( pSplitter );
    pTree->setColumnCount( 1 );
    pTree->headerItem()->setText( 0, tr("Category") );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Start Up") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Pen") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Brush") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Font") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Text") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Ruler") ) ) ) );
    pTree->addTopLevelItem( new QTreeWidgetItem( QStringList( QString( tr("Grid") ) ) ) );
    pSplitter->addWidget( pTree );

    pPanel = new QStackedWidget( pSplitter );   
    pPanel->addWidget( new SAPreferencesStartUpPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SPen" )->getPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SBrush" )->getPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SFont" )->getPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SText" )->getPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SRuler" )->getPanel( pPanel ) );
    pPanel->addWidget( g_SSettings->getSetting( "SGrid" )->getPanel( pPanel ) );
    pSplitter->addWidget( pPanel );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutDialog->addWidget( pButtonBox ); 

    connect( pTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(slotItemActivated(QTreeWidgetItem*,int)) );
    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
}

void SAPreferencesDialog::doPreferences( QWidget *pParent )
{
    // load settings (current defaults)
    SSettings settings( SSettings::ModePreference );
    settings.doInsert( "SPen", new SPen() );       
    settings.doInsert( "SBrush", new SBrush() );     
    settings.doInsert( "SFont", new SFont() );      
    settings.doInsert( "SText", new SText() );      
    settings.doInsert( "SRuler", new SRuler() );      
    settings.doInsert( "SGrid", new SGrid() );      
    settings.doLoad();
    // temp make ourself the global settings as that is what the various widgets will work with
    SSettings *pSettings = g_SSettings;
    g_SSettings = &settings;
    // invoke preferences dialog with the various widgets for editing the settings
    SAPreferencesDialog dialog( pParent );
    dialog.exec();
    // restore global settings (context)
    g_SSettings = pSettings;
    // save settings (new defaults)
    settings.doSave();
}

void SAPreferencesDialog::slotItemActivated( QTreeWidgetItem *pItem, int )
{
    pPanel->setCurrentIndex( pTree->indexOfTopLevelItem( pItem ) );
}

/****************************************************************
 * SAPreferencesStartUpPanel
 ****************************************************************/
SAPreferencesStartUpPanel::SAPreferencesStartUpPanel( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "SAPreferencesStartUpPanel" );

    QFormLayout *pLayout = new QFormLayout( this );

    pRestoreState = new QCheckBox( this );
    pRestoreState->setChecked( QSettings().value( "RestoreState", false ).toInt() );
    pLayout->addRow( tr("Restore Window State"), pRestoreState );
    connect( pRestoreState, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRestoreState(Qt::CheckState)) );

    pSplash = new QCheckBox( this );
    pSplash->setChecked( QSettings().value( "Splash", false ).toInt() );
    pLayout->addRow( tr("Show Splash"), pSplash );
    connect( pSplash, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotSplash(Qt::CheckState)) );
}

void SAPreferencesStartUpPanel::slotRestoreState( Qt::CheckState n )
{
    QSettings().setValue( "RestoreState", int(n) );
}

void SAPreferencesStartUpPanel::slotSplash( Qt::CheckState n )
{
    QSettings().setValue( "Splash", int(n) );
}

