/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "PASSpray.h"

#include "SSettings.h"

/****************************************************************
 * PASSpray
 ****************************************************************/
PASSpray::PASSpray()
{
}

void PASSpray::setRadius( int t )
{
    if ( t == nRadius ) return;
    nRadius = t;
    emit signalChanged();
}

void PASSpray::setPoints( int t )
{
    if ( t == nPoints ) return;
    nPoints = t;
    emit signalChanged();
}

void PASSpray::setSelected( int nRadius, int nPoints )
{
    this->nRadius   = nRadius;
    this->nPoints   = nPoints;
    emit signalSync();
}

QWidget *PASSpray::getToolBar( QWidget *pParent )
{
    return new PASSprayToolBar( pParent );
}

QWidget *PASSpray::getPanel( QWidget *pParent )
{
    return new PASSprayPanel( pParent );
}

void PASSpray::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASSpray" );

    domElem.setAttribute( "Radius", nRadius );
    domElem.setAttribute( "Points", nPoints );

    pdomElem->appendChild( domElem );
}

void PASSpray::doLoad( QDomElement *pdomElem )
{
    nRadius = pdomElem->attribute( "Radius", QString::number( nRadius ) ).toInt();
    nPoints = pdomElem->attribute( "Points", QString::number( nPoints ) ).toInt();
}

/****************************************************************
 * PASSprayToolBar
 ****************************************************************/
PASSprayToolBar::PASSprayToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASSpray*)g_SSettings->getSetting( "PASSpray" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASSpray].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    //
    pRadius = new QSpinBox( this );
    pRadius->setMinimum( 1 );
    pRadius->setMaximum( 50 );
    pRadius->setValue( pSetting->getRadius() );
    pRadius->setToolTip( tr("radius") );
    pLayout->addWidget( pRadius );
    connect( pRadius, SIGNAL(valueChanged(int)), SLOT(slotRadius(int)) );
    //
    pPoints = new QSpinBox( this );
    pPoints->setMinimum( 1 );
    pPoints->setMaximum( 50 );
    pPoints->setValue( pSetting->getPoints() );
    pPoints->setToolTip( tr("points") );
    pLayout->addWidget( pPoints );
    connect( pPoints, SIGNAL(valueChanged(int)), SLOT(slotPoints(int)) );
    //
    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASSprayToolBar::slotRefresh()
{
    pRadius->setValue( pSetting->getRadius() );
    pPoints->setValue( pSetting->getPoints() );
}

void PASSprayToolBar::slotRadius( int n )
{
    pSetting->setRadius( n );
}

void PASSprayToolBar::slotPoints( int n )
{
    pSetting->setPoints( n );
}

/****************************************************************
 * PASSprayPanel
 ****************************************************************/
PASSprayPanel::PASSprayPanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASSpray*)g_SSettings->getSetting( "PASSpray" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASSpray].";
        return;
    }

    //
    pRadius = new QSpinBox( this );
    pRadius->setMinimum( 1 );
    pRadius->setMaximum( 50 );
    pRadius->setValue( pSetting->getRadius() );
    pRadius->setToolTip( tr("radius") );
    connect( pRadius, SIGNAL(valueChanged(int)), SLOT(slotRadius(int)) );
    //
    pPoints = new QSpinBox( this );
    pPoints->setMinimum( 1 );
    pPoints->setMaximum( 50 );
    pPoints->setValue( pSetting->getPoints() );
    pPoints->setToolTip( tr("points") );
    connect( pPoints, SIGNAL(valueChanged(int)), SLOT(slotPoints(int)) );
    //
    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Radius"), pRadius );
    pLayout->addRow( tr("Points"), pPoints );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASSprayPanel::slotRefresh()
{
    pRadius->setValue( pSetting->getRadius() );
    pPoints->setValue( pSetting->getPoints() );
}

void PASSprayPanel::slotRadius( int n )
{
    pSetting->setRadius( n );
}

void PASSprayPanel::slotPoints( int n )
{
    pSetting->setPoints( n );
}

