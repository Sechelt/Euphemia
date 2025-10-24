/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "AppInfo.h"
#include "EPreferencesDialog.h"

#include <SSettings.h>

// SSetting classes we know about...
#include <SPen.h>       
#include <SRuler.h>       
#include <SBrush.h>     
#include <SFont.h>      
#include <SRuler.h>      

#include <LSGeneral.h>     
#include <LSBackground.h>     
#include <LSPage.h>     
#include <LSGrid.h>      
#include <LSPointer.h>      
#include <LSLine.h>     
#include <LSEllipse.h>     
#include <LSRender.h>     
#include <LSImage.h>     
#include <LSText.h>      
#include <LSRectangle.h> 

#include <PASErase.h>     
#include <PASFreeHand.h>  
#include <PASGradient.h>  
#include <PASPaste.h>     
#include <PASPolygonFilled.h>
#include <PASSpray.h>     

EPreferencesDialog::EPreferencesDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setWindowTitle( tr("Preferences") );

    QTreeWidgetItem *pItem01;
    QTreeWidgetItem *pItem02;

    QVBoxLayout *pLayoutDialog = new QVBoxLayout( this );

    pSplitter = new QSplitter( Qt::Horizontal, this );
    pSplitter->setChildrenCollapsible( false );
    pSplitter->setStretchFactor( 1, 10 );
    pLayoutDialog->addWidget( pSplitter );

    pTree = new QTreeWidget( pSplitter );
    pTree->setColumnCount( 1 );
    pTree->headerItem()->setText( 0, tr("Category") );

    pPanels = new QStackedWidget( pSplitter );   

    pItem01 = doAddPanel( tr("StartUp"), new EPreferencesStartUpPanel( pPanels ) );
    pItem01 = doAddPanel( tr("Render"), g_SSettings->getSetting( "LSRender" )->getPanel( pPanels ) );

    pItem01 = doAddPanel( tr("Edit"), new QWidget( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("General"), g_SSettings->getSetting( "LSGeneral" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Background"), g_SSettings->getSetting( "LSBackground" )->getPanel( pPanels ) );

    pItem01 = doAddPanel( tr("View"), new QWidget( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Page"), g_SSettings->getSetting( "LSPage" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Grid"), g_SSettings->getSetting( "LSGrid" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Ruler"), g_SSettings->getSetting( "SRuler" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Pointer"), g_SSettings->getSetting( "LSPointer" )->getPanel( pPanels ) );

    pItem01 = doAddPanel( tr("Tools"), new QWidget( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Pen"), g_SSettings->getSetting( "SPen" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Brush"), g_SSettings->getSetting( "SBrush" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Font"), g_SSettings->getSetting( "SFont" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Erase"), g_SSettings->getSetting( "PASErase" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Free Hand"), g_SSettings->getSetting( "PASFreeHand" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Line"), g_SSettings->getSetting( "LSLine" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Rectangle"), g_SSettings->getSetting( "LSRectangle" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Ellipse"), g_SSettings->getSetting( "LSEllipse" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Spray"), g_SSettings->getSetting( "PASSpray" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Text"), g_SSettings->getSetting( "LSText" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Paste"), g_SSettings->getSetting( "PASPaste" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Gradient"), g_SSettings->getSetting( "PASGradient" )->getPanel( pPanels ) );
    pItem02 = doAddPanel( pItem01, tr("Image"), g_SSettings->getSetting( "LSImage" )->getPanel( pPanels ) );

    pSplitter->addWidget( pTree );
    pSplitter->addWidget( pPanels );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutDialog->addWidget( pButtonBox ); 

    connect( pTree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(slotItemActivated(QTreeWidgetItem*,int)) );
    connect( pButtonBox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pButtonBox, SIGNAL(rejected()), this, SLOT(slotRejected()) );

    doLoadState();

}

void EPreferencesDialog::doPreferences( QWidget *pParent )
{
    // load settings (current defaults)
    SSettings settings( SSettings::ModePreference );
    settings.doInsert( "SPen", new SPen() );       
    settings.doInsert( "SBrush", new SBrush() );     
    settings.doInsert( "SFont", new SFont() );      
    settings.doInsert( "LSRender", new LSRender() );      
    settings.doInsert( "LSImage", new LSImage() );      
    settings.doInsert( "LSPage", new LSPage() );      
    settings.doInsert( "LSGrid", new LSGrid() );      
    settings.doInsert( "LSPointer", new LSPointer() );      
    settings.doInsert( "LSEllipse", new LSEllipse() );     
    settings.doInsert( "PASErase", new PASErase() );     
    settings.doInsert( "PASFreeHand", new PASFreeHand() );  
    settings.doInsert( "PASGradient", new PASGradient() );  
    settings.doInsert( "PASPaste", new PASPaste() );     
    settings.doInsert( "PASPolygonFilled", new PASPolygonFilled() );
    settings.doInsert( "LSLine", new LSLine() ); 
    settings.doInsert( "LSRectangle", new LSRectangle() ); 
    settings.doInsert( "PASSpray", new PASSpray() );     
    settings.doInsert( "LSText", new LSText() );      
    settings.doInsert( "LSBackground", new LSBackground() );     
    settings.doInsert( "LSGeneral", new LSGeneral() );     
    settings.doInsert( "SRuler", new SRuler() );     
    settings.doLoad();
    // temp make ourself the global settings as that is what the various widgets will work with
    SSettings *pSettings = g_SSettings;
    g_SSettings = &settings;
    // invoke preferences dialog with the various widgets for editing the settings
    EPreferencesDialog dialog( pParent );
    dialog.exec();
    // restore global settings (context)
    g_SSettings = pSettings;
    // save settings (new defaults)
    settings.doSave();
}

QTreeWidgetItem *EPreferencesDialog::doAddPanel( const QString &stringText, QWidget *pPanel )
{
    QTreeWidgetItem *pItem;
    pTree->addTopLevelItem( pItem = new QTreeWidgetItem( QStringList( stringText ) ) );
    pPanels->addWidget( pPanel );

    mapItemPanel.insert( pItem, pPanels->count() - 1 );

    return pItem;
}

QTreeWidgetItem *EPreferencesDialog::doAddPanel( QTreeWidgetItem *pItemParent, const QString &stringText, QWidget *pPanel )
{
    QTreeWidgetItem *pItem;
    pItem = new QTreeWidgetItem( pItemParent, QStringList( stringText ) );
    pPanels->addWidget( pPanel );

    mapItemPanel.insert( pItem, pPanels->count() - 1 );

    return pItem;
}

void EPreferencesDialog::doLoadState()
{
    QString s = objectName() + "/";
    QSettings settings;

    if ( settings.value( s + "geometry" ).isNull() ) return;
    
    restoreGeometry(settings.value(s + "geometry").toByteArray());

    pSplitter->restoreState( settings.value( s + "splitterSizes" ).toByteArray() );
}

void EPreferencesDialog::doSaveState()
{
    QString s = objectName() + "/";
    QSettings settings;
    settings.setValue( s + "geometry", saveGeometry() );

    settings.setValue( s + "splitterSizes", pSplitter->saveState() );
}

void EPreferencesDialog::slotItemActivated( QTreeWidgetItem *pItem, int )
{
    pPanels->setCurrentIndex( mapItemPanel.value( pItem ) );
}

void EPreferencesDialog::slotOk()
{
    doSaveState();
    done( QDialog::Accepted );
}

void EPreferencesDialog::slotRejected()
{
    doSaveState();
    done( QDialog::Rejected );
}

/****************************************************************
 * EPreferencesStartUpPanel
 ****************************************************************/
EPreferencesStartUpPanel::EPreferencesStartUpPanel( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "EPreferencesStartUpPanel" );

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

void EPreferencesStartUpPanel::slotRestoreState( Qt::CheckState n )
{
    QSettings().setValue( "RestoreState", (int)n );
}

void EPreferencesStartUpPanel::slotSplash( Qt::CheckState n )
{
    QSettings().setValue( "Splash", (int)n );
}

