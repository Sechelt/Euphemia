/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSGrid.h"

#include <SSettings.h>

/****************************************************************
 * LSGrid
 ****************************************************************/
LSGrid::LSGrid()
{
}

void LSGrid::setType( L::GridTypes t )
{
    if ( nType == t ) return;
    nType = t;
    emit signalChanged();
}

void LSGrid::setSnap( L::GridSnaps t )
{
    if ( nSnap == t ) return;
    nSnap = t;
    emit signalChanged();
}

void LSGrid::setCellSize( const QSize &t )
{
    if ( sizeCell == t ) return;
    sizeCell = t;
    emit signalChanged();
}

void LSGrid::setSizeMin( int t )
{
    if ( nSizeMin == t ) return;
    nSizeMin = t;
    emit signalChanged();
}

void LSGrid::setSelected( L::GridTypes nType, L::GridSnaps nSnap, const QSize &sizeCell, int nSizeMin )
{
    this->nType     = nType;
    this->nSnap     = nSnap;
    this->sizeCell  = sizeCell;
    this->nSizeMin  = nSizeMin;
    emit signalSync();
}

QWidget *LSGrid::getToolBar( QWidget *pParent )
{
    return new LSGridWidget( pParent, Qt::Horizontal );
}

QWidget *LSGrid::getPanel( QWidget *pParent )
{
    return new LSGridWidget( pParent, Qt::Vertical );
}

void LSGrid::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSGrid" );

    domElem.setAttribute( "Type", (int)nType );
    domElem.setAttribute( "Snap", (int)nSnap );
    domElem.setAttribute( "CellXidth", sizeCell.width() );
    domElem.setAttribute( "CellYeight", sizeCell.height() );
    domElem.setAttribute( "Min", nSizeMin );

    pdomElem->appendChild( domElem );
}

void LSGrid::doLoad( QDomElement *pdomElem )
{
    nType       = (L::GridTypes)pdomElem->attribute( "Type", QString::number( (int)nType ) ).toInt();
    nSnap       = (L::GridSnaps)pdomElem->attribute( "Snap", QString::number( (int)nSnap ) ).toInt();
    sizeCell.setWidth( pdomElem->attribute( "CellXidth", QString::number( sizeCell.width() ) ).toInt() );
    sizeCell.setHeight( pdomElem->attribute( "CellYeight", QString::number( sizeCell.height() ) ).toInt() );
    nSizeMin    = pdomElem->attribute( "Min", QString::number( nSizeMin ) ).toInt();
}

/****************************************************************
 * LSGridWidget
 ****************************************************************/
LSGridWidget::LSGridWidget( QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "LSGridWidget" );

    pSetting = (LSGrid*)g_SSettings->getSetting( "LSGrid" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSGrid].";
        return;
    }

    pType = new QComboBox( this );
    pType->addItem( tr("None"), L::GridTypeNone  );
    pType->addItem( tr("Standard"), L::GridTypeStandard  );
    pType->addItem( tr("Pixel"), L::GridTypePixel  );
    pType->setCurrentIndex( pType->findData( int(pSetting->getType()) ) );
    pType->setToolTip( tr("Type of grid.") );
    connect( pType, SIGNAL(activated(int)), SLOT(slotType(int)) );

    pSnap = new QComboBox( this );
    pSnap->addItem( tr("None"), L::GridSnapNone  );
    pSnap->addItem( tr("Line"), L::GridSnapLine  );
    pSnap->addItem( tr("HLine"), L::GridSnapHLine  );
    pSnap->addItem( tr("VLine"), L::GridSnapVLine  );
    pSnap->addItem( tr("Center"), L::GridSnapCenter  );
    pSnap->setCurrentIndex( pSnap->findData( int(pSetting->getSnap()) ) );
    pSnap->setToolTip( tr("Mouse cursor will 'snap' to location based upon this setting.") );
    connect( pSnap, SIGNAL(activated(int)), SLOT(slotSnap(int)) );

    pCellX = new QSpinBox( this );
    pCellX->setMinimum( 10 );
    pCellX->setMaximum( 100 );
    pCellX->setValue( pSetting->getCellSize().width() );
    pCellX->setToolTip( tr("Cell width used if Standard grid.") );
    connect( pCellX, SIGNAL(valueChanged(int)), SLOT(slotCellX(int)) );

    pCellY = new QSpinBox( this );
    pCellY->setMinimum( 10 );
    pCellY->setMaximum( 100 );
    pCellY->setValue( pSetting->getCellSize().height() );
    pCellY->setToolTip( tr("Cell height used if Standard grid.") );
    connect( pCellY, SIGNAL(valueChanged(int)), SLOT(slotCellY(int)) );

    pMin = new QSpinBox( this );
    pMin->setMinimum( 4 );
    pMin->setMaximum( 100 );
    pMin->setValue( pSetting->getSizeMin() );
    pMin->setToolTip( tr("Minimum cell size needed to display grid.\nUsed by Pixel grid.") );
    connect( pMin, SIGNAL(valueChanged(int)), SLOT(slotMin(int)) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pType );
        pLayout->addWidget( pSnap );
        pLayout->addWidget( pCellX );
        pLayout->addWidget( pCellY );
        pLayout->addWidget( pMin );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Type"), pType );
        pLayout->addRow( tr("Snap"), pSnap );
        pLayout->addRow( tr("Width"), pCellX );
        pLayout->addRow( tr("Height"), pCellY );
        pLayout->addRow( tr("Minimum"), pMin );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSGridWidget::slotRefresh()
{
    pType->setCurrentIndex( pType->findData( int(pSetting->getType()) ) );
    pSnap->setCurrentIndex( pSnap->findData( int(pSetting->getSnap()) ) );
    pCellX->setValue( pSetting->getCellSize().width() );
    pCellY->setValue( pSetting->getCellSize().height() );
    pMin->setValue( pSetting->getSizeMin() );
}

void LSGridWidget::slotType( int n )
{
    pSetting->setType( (L::GridTypes)pType->itemData( n ).toInt() );
}

void LSGridWidget::slotSnap( int n )
{
    pSetting->setSnap( (L::GridSnaps)pSnap->itemData( n ).toInt() );
}

void LSGridWidget::slotCellX( int n )
{
    pSetting->setCellSize( QSize( n, pCellY->value() ) );
}

void LSGridWidget::slotCellY( int n )
{
    pSetting->setCellSize( QSize( pCellX->value(), n ) );
}

void LSGridWidget::slotMin( int n )
{
    pSetting->setSizeMin( n );
}


