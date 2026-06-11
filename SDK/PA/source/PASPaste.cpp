/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "PASPaste.h"

#include "SSettings.h"

/****************************************************************
 * PASPaste
 ****************************************************************/
PASPaste::PASPaste()
{
}

void PASPaste::setCompositionMode( QPainter::CompositionMode t )
{
    if ( t == nCompositionMode ) return;
    nCompositionMode = t;
    emit signalChanged();
}

void PASPaste::setStamp( bool t )
{
    if ( t == bStamp ) return;
    bStamp = t;
    emit signalChanged();
}

void PASPaste::setSelected( QPainter::CompositionMode nCompositionMode, bool bStamp )
{
    this->nCompositionMode  = nCompositionMode;
    this->bStamp            = bStamp;
    emit signalSync();
}

QWidget *PASPaste::getToolBar( QWidget *pParent )
{
    return new PASPasteToolBar( pParent );
}

QWidget *PASPaste::getPanel( QWidget *pParent )
{
    return new PASPastePanel( pParent );
}

void PASPaste::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "PASPaste" );

    domElem.setAttribute( "CompositionMode", nCompositionMode );
    domElem.setAttribute( "Stamp", bStamp );

    pdomElem->appendChild( domElem );
}

void PASPaste::doLoad( QDomElement *pdomElem )
{
    nCompositionMode = QPainter::CompositionMode(pdomElem->attribute( "CompositionMode", QString::number( int(nCompositionMode) ) ).toInt());
    bStamp = pdomElem->attribute( "Stamp", QString::number( bStamp ) ).toInt();
}

/****************************************************************
 * PASPasteToolBar
 ****************************************************************/
PASPasteToolBar::PASPasteToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASPaste*)g_SSettings->getSetting( "PASPaste" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASPaste].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pComposition = new QComboBox( this );
    pComposition->addItem( "SourceOver", QPainter::CompositionMode_SourceOver );
    pComposition->addItem( "DestinationOver", QPainter::CompositionMode_DestinationOver );
    pComposition->addItem( "Clear", QPainter::CompositionMode_Clear );
    pComposition->addItem( "Source", QPainter::CompositionMode_Source );
    pComposition->addItem( "Destination", QPainter::CompositionMode_Destination );
    pComposition->addItem( "SourceIn", QPainter::CompositionMode_SourceIn );
    pComposition->addItem( "DestinationIn", QPainter::CompositionMode_DestinationIn );
    pComposition->addItem( "SourceOut", QPainter::CompositionMode_SourceOut );
    pComposition->addItem( "DestinationOut", QPainter::CompositionMode_DestinationOut );
    pComposition->addItem( "SourceAtop", QPainter::CompositionMode_SourceAtop );
    pComposition->addItem( "DestinationAtop", QPainter::CompositionMode_DestinationAtop );
    pComposition->addItem( "Xor", QPainter::CompositionMode_Xor );
    //svg 1.2 blend modes
    pComposition->addItem( "Plus", QPainter::CompositionMode_Plus );
    pComposition->addItem( "Multiply", QPainter::CompositionMode_Multiply );
    pComposition->addItem( "Screen", QPainter::CompositionMode_Screen );
    pComposition->addItem( "Overlay", QPainter::CompositionMode_Overlay );
    pComposition->addItem( "Darken", QPainter::CompositionMode_Darken );
    pComposition->addItem( "Lighten", QPainter::CompositionMode_Lighten );
    pComposition->addItem( "ColorDodge", QPainter::CompositionMode_ColorDodge );
    pComposition->addItem( "ColorBurn", QPainter::CompositionMode_ColorBurn );
    pComposition->addItem( "HardLight", QPainter::CompositionMode_HardLight );
    pComposition->addItem( "SoftLight", QPainter::CompositionMode_SoftLight );
    pComposition->addItem( "Difference", QPainter::CompositionMode_Difference );
    pComposition->addItem( "Exclusion", QPainter::CompositionMode_Exclusion );
    // ROPs (X11)
    pComposition->addItem( "SourceOrDestination", QPainter::RasterOp_SourceOrDestination );
    pComposition->addItem( "SourceAndDestination", QPainter::RasterOp_SourceAndDestination );
    pComposition->addItem( "SourceXorDestination", QPainter::RasterOp_SourceXorDestination );
    pComposition->addItem( "NotSourceAndNotDestination", QPainter::RasterOp_NotSourceAndNotDestination );
    pComposition->addItem( "NotSourceOrNotDestination", QPainter::RasterOp_NotSourceOrNotDestination );
    pComposition->addItem( "NotSourceXorDestination", QPainter::RasterOp_NotSourceXorDestination );
    pComposition->addItem( "NotSource", QPainter::RasterOp_NotSource );
    pComposition->addItem( "NotSourceAndDestination", QPainter::RasterOp_NotSourceAndDestination );
    pComposition->addItem( "SourceAndNotDestination", QPainter::RasterOp_SourceAndNotDestination );
    pComposition->addItem( "NotSourceOrDestination", QPainter::RasterOp_NotSourceOrDestination );
    pComposition->addItem( "SourceOrNotDestination", QPainter::RasterOp_SourceOrNotDestination );
    pComposition->addItem( "ClearDestination", QPainter::RasterOp_ClearDestination );
    pComposition->addItem( "SetDestination", QPainter::RasterOp_SetDestination );
    pComposition->addItem( "NotDestination", QPainter::RasterOp_NotDestination );
    pComposition->setCurrentIndex( pComposition->findData( (int)pSetting->getCompositionMode() ) );
    connect( pComposition, SIGNAL(currentIndexChanged(int)), SLOT(slotComposition(int)) );
    pLayout->addWidget( pComposition );

    pStamp = new QCheckBox( tr("Stamp"), this );
    pStamp->setTristate( false );
    pStamp->setChecked( pSetting->getStamp() );
    connect( pStamp, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStamp(Qt::CheckState)) );
    pLayout->addWidget( pStamp );

    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASPasteToolBar::slotRefresh()
{
    pComposition->setCurrentIndex( pComposition->findData( (int)pSetting->getCompositionMode() ) );
    pStamp->setChecked( pSetting->getStamp() );
}

void PASPasteToolBar::slotComposition( int )
{
    pSetting->setCompositionMode( QPainter::CompositionMode(pComposition->currentData().toInt()) );
}

void PASPasteToolBar::slotStamp( Qt::CheckState n )
{
    pSetting->setStamp( n );
}

/****************************************************************
 * PASPastePanel
 ****************************************************************/
PASPastePanel::PASPastePanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (PASPaste*)g_SSettings->getSetting( "PASPaste" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [PASPaste].";
        return;
    }

    QFormLayout *pLayout = new QFormLayout( this );

    pComposition = new QComboBox( this );
    pComposition->addItem( "SourceOver", QPainter::CompositionMode_SourceOver );
    pComposition->addItem( "DestinationOver", QPainter::CompositionMode_DestinationOver );
    pComposition->addItem( "Clear", QPainter::CompositionMode_Clear );
    pComposition->addItem( "Source", QPainter::CompositionMode_Source );
    pComposition->addItem( "Destination", QPainter::CompositionMode_Destination );
    pComposition->addItem( "SourceIn", QPainter::CompositionMode_SourceIn );
    pComposition->addItem( "DestinationIn", QPainter::CompositionMode_DestinationIn );
    pComposition->addItem( "SourceOut", QPainter::CompositionMode_SourceOut );
    pComposition->addItem( "DestinationOut", QPainter::CompositionMode_DestinationOut );
    pComposition->addItem( "SourceAtop", QPainter::CompositionMode_SourceAtop );
    pComposition->addItem( "DestinationAtop", QPainter::CompositionMode_DestinationAtop );
    pComposition->addItem( "Xor", QPainter::CompositionMode_Xor );
    //svg 1.2 blend modes
    pComposition->addItem( "Plus", QPainter::CompositionMode_Plus );
    pComposition->addItem( "Multiply", QPainter::CompositionMode_Multiply );
    pComposition->addItem( "Screen", QPainter::CompositionMode_Screen );
    pComposition->addItem( "Overlay", QPainter::CompositionMode_Overlay );
    pComposition->addItem( "Darken", QPainter::CompositionMode_Darken );
    pComposition->addItem( "Lighten", QPainter::CompositionMode_Lighten );
    pComposition->addItem( "ColorDodge", QPainter::CompositionMode_ColorDodge );
    pComposition->addItem( "ColorBurn", QPainter::CompositionMode_ColorBurn );
    pComposition->addItem( "HardLight", QPainter::CompositionMode_HardLight );
    pComposition->addItem( "SoftLight", QPainter::CompositionMode_SoftLight );
    pComposition->addItem( "Difference", QPainter::CompositionMode_Difference );
    pComposition->addItem( "Exclusion", QPainter::CompositionMode_Exclusion );
    // ROPs (X11)
    pComposition->addItem( "SourceOrDestination", QPainter::RasterOp_SourceOrDestination );
    pComposition->addItem( "SourceAndDestination", QPainter::RasterOp_SourceAndDestination );
    pComposition->addItem( "SourceXorDestination", QPainter::RasterOp_SourceXorDestination );
    pComposition->addItem( "NotSourceAndNotDestination", QPainter::RasterOp_NotSourceAndNotDestination );
    pComposition->addItem( "NotSourceOrNotDestination", QPainter::RasterOp_NotSourceOrNotDestination );
    pComposition->addItem( "NotSourceXorDestination", QPainter::RasterOp_NotSourceXorDestination );
    pComposition->addItem( "NotSource", QPainter::RasterOp_NotSource );
    pComposition->addItem( "NotSourceAndDestination", QPainter::RasterOp_NotSourceAndDestination );
    pComposition->addItem( "SourceAndNotDestination", QPainter::RasterOp_SourceAndNotDestination );
    pComposition->addItem( "NotSourceOrDestination", QPainter::RasterOp_NotSourceOrDestination );
    pComposition->addItem( "SourceOrNotDestination", QPainter::RasterOp_SourceOrNotDestination );
    pComposition->addItem( "ClearDestination", QPainter::RasterOp_ClearDestination );
    pComposition->addItem( "SetDestination", QPainter::RasterOp_SetDestination );
    pComposition->addItem( "NotDestination", QPainter::RasterOp_NotDestination );
    pComposition->setCurrentIndex( pComposition->findData( (int)pSetting->getCompositionMode() ) );
    connect( pComposition, SIGNAL(currentIndexChanged(int)), SLOT(slotComposition(int)) );

    pStamp = new QCheckBox( this );
    pStamp->setTristate( false );
    pStamp->setChecked( pSetting->getStamp() );
    connect( pStamp, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStamp(Qt::CheckState)) );

    pLayout->addRow( tr("Composition"), pComposition );
    pLayout->addRow( tr("Stamp"), pStamp );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void PASPastePanel::slotRefresh()
{
    pComposition->setCurrentIndex( pComposition->findData( (int)pSetting->getCompositionMode() ) );
    pStamp->setChecked( pSetting->getStamp() );
}

void PASPastePanel::slotComposition( int )
{
    pSetting->setCompositionMode( QPainter::CompositionMode(pComposition->currentData().toInt()) );
}

void PASPastePanel::slotStamp( Qt::CheckState n )
{
    pSetting->setStamp( (bool)n );
}

