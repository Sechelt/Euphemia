/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LSEllipse.h"

#include "SSettings.h"

/****************************************************************
 * LSEllipse
 ****************************************************************/
LSEllipse::LSEllipse()
{
}

void LSEllipse::setBind( bool t )
{
    if ( t == bBind ) return;
    bBind = t;
    emit signalChanged();
}

void LSEllipse::setSelected( bool t )
{
    if ( t == bBind ) return;
    bBind = t;
    emit signalSync();
}

QWidget *LSEllipse::getToolBar( QWidget *pParent )
{
    return new LSEllipseToolBar( pParent );
}

QWidget *LSEllipse::getPanel( QWidget *pParent )
{
    return new LSEllipsePanel( pParent );
}

void LSEllipse::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSEllipse" );

    domElem.setAttribute( "Bind", bBind );

    pdomElem->appendChild( domElem );
}

void LSEllipse::doLoad( QDomElement *pdomElem )
{
    bBind  = pdomElem->attribute( "Bind", QString::number( bBind ) ).toInt();
}

/****************************************************************
 * LSEllipseToolBar
 ****************************************************************/
LSEllipseToolBar::LSEllipseToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (LSEllipse*)g_SSettings->getSetting( "LSEllipse" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSEllipse].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pBind = new QCheckBox( tr("Bind Horiz/Vert Lengths"), this );
    pBind->setTristate( false );
    pBind->setToolTip( tr("geometry: change V/H lengths the same amount to create a square or a circle") );
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    pLayout->addWidget( pBind );
    connect( pBind, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotBind(Qt::CheckState)) );

    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSEllipseToolBar::slotRefresh()
{
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
}

void LSEllipseToolBar::slotBind( Qt::CheckState n )
{
    pSetting->setBind( n );
}

/****************************************************************
 * LSEllipsePanel
 ****************************************************************/
LSEllipsePanel::LSEllipsePanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (LSEllipse*)g_SSettings->getSetting( "LSEllipse" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSEllipse].";
        return;
    }

    QFormLayout *pLayout = new QFormLayout( this );

    pBind = new QCheckBox( this );
    pBind->setTristate( false );
    pBind->setToolTip( tr("geometry: change V/H lengths the same amount to create a circle") );
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    pLayout->addRow( tr("Bind"), pBind );
    connect( pBind, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotBind(Qt::CheckState)) );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSEllipsePanel::slotRefresh()
{
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
}

void LSEllipsePanel::slotBind( Qt::CheckState n )
{
    pSetting->setBind( (bool)n );
}


