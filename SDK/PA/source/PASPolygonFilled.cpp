/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASPolygonFilled.h"

#include "SSettings.h"

/****************************************************************
 * PASPolygonFilled
 ****************************************************************/
PASPolygonFilled::PASPolygonFilled()
{
}

void PASPolygonFilled::setFillRule( Qt::FillRule t )
{
    if ( t == nFillRule ) return;
    nFillRule = t;
    emit signalChanged();
}

void PASPolygonFilled::setSelected( Qt::FillRule t )
{
    if ( t == nFillRule ) return;
    nFillRule = t;
    emit signalSync();
}

QWidget *PASPolygonFilled::getToolBar( QWidget *pParent )
{
    return new PASPolygonFilledToolBar( pParent );
}

QWidget *PASPolygonFilled::getPanel( QWidget *pParent )
{
    return new PASPolygonFilledPanel( pParent );
}

void PASPolygonFilled::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASPolygonFilled" );

    domElem.setAttribute( "FillRule", nFillRule );

    pdomElem->appendChild( domElem );
}

void PASPolygonFilled::doLoad( QDomElement *pdomElem )
{
    nFillRule = Qt::FillRule(pdomElem->attribute( "FillRule", QString::number( nFillRule ) ).toInt());
}

/****************************************************************
 * PASPolygonFilledToolBar
 ****************************************************************/
PASPolygonFilledToolBar::PASPolygonFilledToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASPolygonFilled*)g_SSettings->getSetting( "PASPolygonFilled" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASPolygonFilled].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pFillRule = new QComboBox( this );
    pFillRule->addItem( "Odd/Even Fill", Qt::OddEvenFill );
    pFillRule->addItem( "Winding Fill", Qt::WindingFill );
    pFillRule->setCurrentIndex( pFillRule->findData( pSetting->getFillRule() ) );
    pLayout->addWidget( pFillRule );
    connect( pFillRule, SIGNAL(activated(int)), SLOT(slotFillRule(int)) );

    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASPolygonFilledToolBar::slotRefresh()
{
    pFillRule->setCurrentIndex( pFillRule->findData( pSetting->getFillRule() ) );
}

void PASPolygonFilledToolBar::slotFillRule( int n )
{
    pSetting->setFillRule( (Qt::FillRule)pFillRule->itemData( n ).toInt() );
}

/****************************************************************
 * PASPolygonFilledPanel
 ****************************************************************/
PASPolygonFilledPanel::PASPolygonFilledPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASPolygonFilled*)g_SSettings->getSetting( "PASPolygonFilled" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASPolygonFilled].";
        return;
    }

    pFillRule = new QComboBox( this );
    pFillRule->addItem( "Odd/Even Fill", Qt::OddEvenFill );
    pFillRule->addItem( "Winding Fill", Qt::WindingFill );
    pFillRule->setCurrentIndex( pFillRule->findData( pSetting->getFillRule() ) );
    connect( pFillRule, SIGNAL(activated(int)), SLOT(slotFillRule(int)) );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Fill Rule"), pFillRule );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASPolygonFilledPanel::slotRefresh()
{
    pFillRule->setCurrentIndex( pFillRule->findData( pSetting->getFillRule() ) );
}

void PASPolygonFilledPanel::slotFillRule( int n )
{
    pSetting->setFillRule( (Qt::FillRule)pFillRule->itemData( n ).toInt() );
}

