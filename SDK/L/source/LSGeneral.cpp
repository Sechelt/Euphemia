/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LSGeneral.h"

/****************************************************************
 * LSGeneral
 ****************************************************************/
LSGeneral::LSGeneral()
{
}

void LSGeneral::setAutoCommit( bool t )
{
    if ( bAutoCommit == t ) return;
    bAutoCommit = t;
    emit signalChanged();
}

void LSGeneral::setPage( bool t )
{
    if ( bPage == t ) return;
    bPage = t;
    emit signalChanged();
}

void LSGeneral::setGrid( bool t )
{
    if ( bGrid == t ) return;
    bGrid = t;
    emit signalChanged();
}

void LSGeneral::setRuler( bool t )
{
    if ( bRuler == t ) return;
    bRuler = t;
    emit signalChanged();
}

void LSGeneral::setSelected( bool bAutoCommit, bool bPage, bool bGrid, bool bRuler )
{
    this->bAutoCommit   = bAutoCommit;
    this->bPage         = bPage;
    this->bGrid         = bGrid;
    this->bRuler        = bRuler;
    emit signalSync();
}

QWidget *LSGeneral::getToolBar( QWidget *pParent )
{
    return new LSGeneralWidget( pParent, Qt::Horizontal );
}

QWidget *LSGeneral::getPanel( QWidget *pParent )
{
    return new LSGeneralWidget( pParent, Qt::Vertical );
}

void LSGeneral::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSGeneral" );

    domElem.setAttribute( "AutoCommit", bAutoCommit );
    domElem.setAttribute( "Page", bPage );
    domElem.setAttribute( "Grid", bGrid );
    domElem.setAttribute( "Ruler", bRuler );

    pdomElem->appendChild( domElem );
}

void LSGeneral::doLoad( QDomElement *pdomElem )
{
    bAutoCommit = pdomElem->attribute( "AutoCommit", QString::number( bAutoCommit ) ).toInt();
    bPage   = pdomElem->attribute( "Page", QString::number( bPage ) ).toInt();
    bGrid       = pdomElem->attribute( "Grid", QString::number( bGrid ) ).toInt();
    bRuler      = pdomElem->attribute( "Ruler", QString::number( bRuler ) ).toInt();
}

/****************************************************************
 * LSGeneralWidget
 ****************************************************************/
LSGeneralWidget::LSGeneralWidget( QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "LSGeneralWidget" );

    pSetting = (LSGeneral*)g_SSettings->getSetting( "LSGeneral" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSGeneral].";
        return;
    }

    pAutoCommit = new QCheckBox( this );
    pAutoCommit->setToolTip( tr("Auto Commit Shapes") );
    pAutoCommit->setChecked( pSetting->getAutoCommit() );
    connect( pAutoCommit, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotAutoCommit(Qt::CheckState)) );

    pPage = new QCheckBox( this );
    pPage->setTristate( false );
    pPage->setCheckState( pSetting->getPage() ? Qt::Checked : Qt::Unchecked );
    pPage->setToolTip( tr("draw page: draw a border around the page/canvas") );
    connect( pPage, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotPage(Qt::CheckState)) );

    pGrid = new QCheckBox( this );
    pGrid->setTristate( false );
    pGrid->setToolTip( tr("Show Grid") );
    pGrid->setCheckState( pSetting->getGrid() ? Qt::Checked : Qt::Unchecked );
    connect( pGrid, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotGrid(Qt::CheckState)) );

    pRuler = new QCheckBox( this );
    pRuler->setTristate( false );
    pRuler->setToolTip( tr("Show Ruler") );
    pRuler->setCheckState( pSetting->getRuler() ? Qt::Checked : Qt::Unchecked );
    connect( pRuler, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotRuler(Qt::CheckState)) );

    if ( n == Qt::Horizontal )
    {
        pAutoCommit->setText( tr("Auto Commit Shapes") );
        pPage->setText( tr("Draw Page") );
        pGrid->setText( tr("Show Grid") );
        pRuler->setText( tr("Show Ruler") );

        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pAutoCommit );
        pLayout->addWidget( pPage );
        pLayout->addWidget( pGrid );
        pLayout->addWidget( pRuler );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Auto Commit Shapes"), pAutoCommit );
        pLayout->addRow( tr("Draw Page"), pPage );
        pLayout->addRow( tr("Show Grid"), pGrid );
        pLayout->addRow( tr("Show Ruler"), pRuler );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSGeneralWidget::slotRefresh()
{
    pAutoCommit->setChecked( pSetting->getAutoCommit() );
    pPage->setCheckState( pSetting->getPage() ? Qt::Checked : Qt::Unchecked );
    pGrid->setCheckState( pSetting->getGrid() ? Qt::Checked : Qt::Unchecked );
    pRuler->setCheckState( pSetting->getRuler() ? Qt::Checked : Qt::Unchecked );
}

void LSGeneralWidget::slotAutoCommit( Qt::CheckState n )
{
    pSetting->setAutoCommit( n );
}

void LSGeneralWidget::slotPage( Qt::CheckState n )
{
    pSetting->setPage( n );
}

void LSGeneralWidget::slotGrid( Qt::CheckState n )
{
    pSetting->setGrid( n );
}

void LSGeneralWidget::slotRuler( Qt::CheckState n )
{
    pSetting->setRuler( n );
}



