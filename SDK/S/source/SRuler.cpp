/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "SRuler.h"

#include <SSettings.h>

/****************************************************************
 * SRuler
 ****************************************************************/
SRuler::SRuler( QObject *pParent )
    : SSetting( pParent )
{
}

void SRuler::setPixelMajour( int t )
{
    if ( nPixelMajour == t ) return;
    nPixelMajour = t;
    emit signalChanged();
}

void SRuler::setPixelMinor( int t )
{
    if ( nPixelMinor == t ) return;
    nPixelMinor = t;
    emit signalChanged();
}

void SRuler::setSelected( int nPixelMajour, int nPixelMinor )
{
    this->nPixelMajour      = nPixelMajour;
    this->nPixelMinor       = nPixelMinor;
    emit signalSync();
}

QWidget *SRuler::getToolBar( QWidget *pParent )
{
    return new SRulerWidget( this, pParent, Qt::Horizontal );
}

QWidget *SRuler::getPanel( QWidget *pParent )
{
    return new SRulerWidget( this, pParent, Qt::Vertical );
}

void SRuler::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    QDomElement domElemThis = pdomDoc->createElement( "SRuler" );

    domElemThis.setAttribute( "PixelMajour", nPixelMajour );
    domElemThis.setAttribute( "PixelMinor", nPixelMinor );

    pdomElemParent->appendChild( domElemThis );
}

void SRuler::doLoad( QDomElement *pdomElemThis )
{
    nPixelMajour = pdomElemThis->attribute( "PixelMajour", QString::number( nPixelMajour ) ).toInt();
    nPixelMinor = pdomElemThis->attribute( "PixelMinor", QString::number( nPixelMinor ) ).toInt();
}

/****************************************************************
 * SRulerWidget
 ****************************************************************/
SRulerWidget::SRulerWidget( SRuler *pRuler, QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "SRulerWidget" );

    this->pRuler = pRuler;

    pMajour = new QSpinBox( this );
    pMajour->setMinimum( 0 );
    pMajour->setMaximum( 100 );
    pMajour->setToolTip( tr("majour tick\n0=no tick") );
    pMajour->setValue( pRuler->getPixelMajour() );
    connect( pMajour, SIGNAL(valueChanged(int)), SLOT(slotMajour(int)) );

    pMinor = new QSpinBox( this );
    pMinor->setMinimum( 0 );
    pMinor->setMaximum( 100 );
    pMinor->setToolTip( tr("minor tick\n0=no tick") );
    pMinor->setValue( pRuler->getPixelMinor() );
    connect( pMinor, SIGNAL(valueChanged(int)), SLOT(slotMinor(int)) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pMajour );
        pLayout->addWidget( pMinor );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Majour"), pMajour );
        pLayout->addRow( tr("Minor"), pMinor );
    }

    connect( pRuler, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pRuler, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SRulerWidget::slotRefresh()
{
    pMajour->setValue( pRuler->getPixelMajour() );
    pMinor->setValue( pRuler->getPixelMinor() );
}

void SRulerWidget::slotMajour( int n )
{
    pRuler->setPixelMajour( n );
}

void SRulerWidget::slotMinor( int n )
{
    pRuler->setPixelMinor( n );
}


