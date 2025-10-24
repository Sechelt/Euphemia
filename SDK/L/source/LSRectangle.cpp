/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "LSRectangle.h"

#include "SSettings.h"

/****************************************************************
 * LSRectangle
 ****************************************************************/
LSRectangle::LSRectangle()
{
}

void LSRectangle::setBind( bool t )
{
    if ( t == bBind ) return;
    bBind = t;
    emit signalChanged();
}

void LSRectangle::setSizeMode( Qt::SizeMode t )     
{
    if ( t == nSizeMode ) return;
    nSizeMode = t;
    emit signalChanged();
}

void LSRectangle::setXRadius( qreal t )      
{
    if ( t == nXRadius ) return;
    nXRadius = t;
    emit signalChanged();
}

void LSRectangle::setYRadius( qreal t )      
{
    if ( t == nYRadius ) return;
    nYRadius = t;
    emit signalChanged();
}

void LSRectangle::setSelected( bool bBind, Qt::SizeMode nSizeMode, qreal nXRadius, qreal nYRadius )
{
    this->bBind     = bBind;
    this->nSizeMode = nSizeMode;
    this->nXRadius  = nXRadius;
    this->nYRadius  = nYRadius;
    emit signalSync();
}

QWidget *LSRectangle::getToolBar( QWidget *pParent )
{
    return new LSRectangleToolBar( pParent );
}

QWidget *LSRectangle::getPanel( QWidget *pParent )
{
    return new LSRectanglePanel( pParent );
}

void LSRectangle::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSRectangle" );

    domElem.setAttribute( "Bind", bBind );
    domElem.setAttribute( "SizeMode", (int)nSizeMode );
    domElem.setAttribute( "XRadius", nXRadius );
    domElem.setAttribute( "YRadius", nYRadius );

    pdomElem->appendChild( domElem );
}

void LSRectangle::doLoad( QDomElement *pdomElem )
{
    bBind  = pdomElem->attribute( "Bind", QString::number( bBind ) ).toInt();
    nSizeMode       = (Qt::SizeMode)pdomElem->attribute( "SizeMode", QString::number( nSizeMode ) ).toInt();
    nXRadius        = pdomElem->attribute( "XRadius", QString::number( nXRadius ) ).toInt();
    nYRadius        = pdomElem->attribute( "YRadius", QString::number( nYRadius ) ).toInt();
}

/****************************************************************
 * LSRectangleToolBar
 ****************************************************************/
LSRectangleToolBar::LSRectangleToolBar( QWidget *p )
    : QWidget( p )
{
    pSetting = (LSRectangle*)g_SSettings->getSetting( "LSRectangle" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSRectangle].";
        return;
    }

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    pBind = new QCheckBox( tr("Bind Horiz/Vert Lengths"), this );
    pBind->setTristate( false );
    pBind->setToolTip( tr("geometry: change V/H lengths the same amount to create a square or a circle") );
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    pLayout->addWidget( pBind );
    connect( pBind, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotBind(Qt::CheckState)) );

    pXRadius = new QDoubleSpinBox( this );
    pXRadius->setToolTip( tr("corner: X radius") );
    pXRadius->setMinimum( 0 );
    pXRadius->setValue( pSetting->getXRadius() );
    pLayout->addWidget( pXRadius );
    connect( pXRadius, SIGNAL(valueChanged(double)), SLOT(slotXRadius(qreal)) );

    pYRadius = new QDoubleSpinBox( this );
    pYRadius->setToolTip( tr("corner: Y radius") );
    pYRadius->setMinimum( 0 );
    pYRadius->setValue( pSetting->getYRadius() );
    pLayout->addWidget( pYRadius );
    connect( pYRadius, SIGNAL(valueChanged(double)), SLOT(slotYRadius(qreal)) );

    if ( pSetting->getSizeMode() == Qt::RelativeSize ) 
    {
        pXRadius->setMaximum( 100 );
        pYRadius->setMaximum( 100 );
    }
    else
    {
        pXRadius->setMaximum( 30000 );
        pYRadius->setMaximum( 30000 );
    }

    pMode = new QComboBox( this );
    pMode->setToolTip( tr("corner: mode") );
    pMode->addItem( tr("Absolute Size"), Qt::AbsoluteSize );
    pMode->addItem( tr("Relative Size"), Qt::RelativeSize );
    pMode->setCurrentIndex( pMode->findData( pSetting->getSizeMode() ) );
    pLayout->addWidget( pMode );
    connect( pMode, SIGNAL(currentIndexChanged(int)), SLOT(slotMode(int)) );

    pLayout->addStretch( 10 );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSRectangleToolBar::slotRefresh()
{
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    pMode->setCurrentIndex( pMode->findData( pSetting->getSizeMode() ) );
    if ( pSetting->getSizeMode() == Qt::RelativeSize ) 
    {
        pXRadius->setMaximum( 100 );
        pYRadius->setMaximum( 100 );
    }
    else
    {
        pXRadius->setMaximum( 30000 );
        pYRadius->setMaximum( 30000 );
    }

    pXRadius->setValue( pSetting->getXRadius() );
    pYRadius->setValue( pSetting->getYRadius() );
}

void LSRectangleToolBar::slotBind( Qt::CheckState n )
{
    pSetting->setBind( n );
}

void LSRectangleToolBar::slotMode( int )
{
    pSetting->setSizeMode( (Qt::SizeMode)pMode->currentData().toInt() );
}

void LSRectangleToolBar::slotXRadius( qreal n )
{
    pSetting->setXRadius( n );
}

void LSRectangleToolBar::slotYRadius( qreal n )
{
    pSetting->setYRadius( n );
}

/****************************************************************
 * LSRectanglePanel
 ****************************************************************/
LSRectanglePanel::LSRectanglePanel( QWidget *p )
    : QWidget( p )
{
    pSetting = (LSRectangle*)g_SSettings->getSetting( "LSRectangle" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not load [LSRectangle].";
        return;
    }

    pBind = new QCheckBox( this );
    pBind->setTristate( false );
    pBind->setToolTip( tr("geometry: change V/H lengths the same amount to create a square or a circle") );
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    connect( pBind, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotBind(Qt::CheckState)) );

    pXRadius = new QDoubleSpinBox( this );
    pXRadius->setToolTip( tr("corner: X radius") );
    pXRadius->setMinimum( 0 );
    pXRadius->setValue( pSetting->getXRadius() );
    connect( pXRadius, SIGNAL(valueChanged(double)), SLOT(slotXRadius(qreal)) );

    pYRadius = new QDoubleSpinBox( this );
    pYRadius->setToolTip( tr("corner: Y radius") );
    pYRadius->setMinimum( 0 );
    pYRadius->setValue( pSetting->getYRadius() );
    connect( pYRadius, SIGNAL(valueChanged(double)), SLOT(slotYRadius(qreal)) );

    if ( pSetting->getSizeMode() == Qt::RelativeSize ) 
    {
        pXRadius->setMaximum( 100 );
        pYRadius->setMaximum( 100 );
    }
    else
    {
        pXRadius->setMaximum( 30000 );
        pYRadius->setMaximum( 30000 );
    }

    pMode = new QComboBox( this );
    pMode->setToolTip( tr("corner: mode") );
    pMode->addItem( tr("Absolute Size"), Qt::AbsoluteSize );
    pMode->addItem( tr("Relative Size"), Qt::RelativeSize );
    pMode->setCurrentIndex( pMode->findData( pSetting->getSizeMode() ) );
    connect( pMode, SIGNAL(currentIndexChanged(int)), SLOT(slotMode(int)) );

    QFormLayout *pLayout = new QFormLayout( this );
    pLayout->addRow( tr("Bind"), pBind );
    pLayout->addRow( tr("XRadius"), pXRadius );
    pLayout->addRow( tr("YRadius"), pYRadius );
    pLayout->addRow( tr("Mode"), pMode );

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSRectanglePanel::slotRefresh()
{
    pBind->setCheckState( pSetting->getBind() ? Qt::Checked : Qt::Unchecked );
    pMode->setCurrentIndex( pMode->findData( pSetting->getSizeMode() ) );
    pXRadius->setValue( pSetting->getXRadius() );
    pYRadius->setValue( pSetting->getYRadius() );
    if ( pSetting->getSizeMode() == Qt::RelativeSize ) 
    {
        pXRadius->setMaximum( 100 );
        pYRadius->setMaximum( 100 );
    }
    else
    {
        pXRadius->setMaximum( 30000 );
        pYRadius->setMaximum( 30000 );
    }
}

void LSRectanglePanel::slotBind( Qt::CheckState n )
{
    pSetting->setBind( n );
}

void LSRectanglePanel::slotMode( int )
{
    pSetting->setSizeMode( (Qt::SizeMode)pMode->currentData().toInt() );
}

void LSRectanglePanel::slotXRadius( qreal n )
{
    pSetting->setXRadius( n );
}

void LSRectanglePanel::slotYRadius( qreal n )
{
    pSetting->setYRadius( n );
}


