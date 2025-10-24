/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 */

#include "LibInfo.h"
#include "SGrid.h"

#include "SSettings.h"

/****************************************************************
 * SGrid
 ****************************************************************/
SGrid::SGrid( QObject *pParent )
    : SSetting( pParent )
{
}

void SGrid::setVisible( bool b )
{
    if ( bVisible == b ) return;
    bVisible = b;
    emit signalChanged();
}

void SGrid::setSizeRulerMajour( bool b )
{
    if ( bRulerMajour == b ) return;
    bRulerMajour = b;
    emit signalChanged();
}

void SGrid::setSizeRulerMinor( bool b )
{
    if ( bRulerMinor == b ) return;
    bRulerMinor = b;
    emit signalChanged();
}

void SGrid::setSize( const QSize &t )
{
    if ( sizeCell == t ) return;
    sizeCell = t;
    emit signalChanged();
}

void SGrid::setSnap( S::GridSnaps t )
{
    if ( nSnap == t ) return;
    nSnap = t;
    emit signalChanged();
}

void SGrid::setSelected( bool bVisible, bool bRulerMajour, bool bRulerMinor, const QSize &sizeCell, S::GridSnaps nSnap )
{
    this->bVisible      = bVisible;
    this->bRulerMajour  = bRulerMajour;
    this->bRulerMinor   = bRulerMinor;
    this->sizeCell      = sizeCell;
    this->nSnap         = nSnap;
    emit signalSync();
}

QWidget *SGrid::getToolBar( QWidget *pParent )
{
    return new SGridWidget( pParent, Qt::Horizontal, this );
}

QWidget *SGrid::getPanel( QWidget *pParent )
{
    return new SGridWidget( pParent, Qt::Vertical, this );
}

void SGrid::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "SGrid" );

    domElem.setAttribute( "Visible", bVisible );
    domElem.setAttribute( "RulerMajour", bVisible );
    domElem.setAttribute( "RulerMinor", bVisible );
    domElem.setAttribute( "Width", sizeCell.width() );
    domElem.setAttribute( "Height", sizeCell.height() );
    domElem.setAttribute( "Snap", (int)nSnap );

    pdomElem->appendChild( domElem );
}

void SGrid::doLoad( QDomElement *pdomElem )
{
    bVisible    = pdomElem->attribute( "Visible", QString::number( bVisible ) ).toInt();
    bRulerMajour= pdomElem->attribute( "RulerMajour", QString::number( bRulerMajour ) ).toInt();
    bRulerMinor = pdomElem->attribute( "RulerMinor", QString::number( bRulerMinor ) ).toInt();
    sizeCell.setWidth( pdomElem->attribute( "Width", QString::number( sizeCell.width() ) ).toInt() );
    sizeCell.setHeight( pdomElem->attribute( "Height", QString::number( sizeCell.height() ) ).toInt() );
    nSnap       = (S::GridSnaps)pdomElem->attribute( "Snap", QString::number( (int)nSnap ) ).toInt();
}

/****************************************************************
 * SGridWidget
 ****************************************************************/
SGridWidget::SGridWidget( QWidget *pParent, Qt::Orientation n, SGrid *p )
    : QWidget( pParent )
{
    setObjectName( "SGridWidget" );

    if ( p ) pSetting = p; else pSetting = (SGrid *)g_SSettings->getSetting( "SGrid" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [SGrid].";
        return;
    }

    pVisible = new QCheckBox( this );
    pVisible->setChecked( pSetting->getVisible() );
    pVisible->setToolTip( tr("Show/Hide grid.") );
    connect( pVisible, SIGNAL(toggled(bool)), SLOT(slotVisible(bool)) );

    pRulerMajour = new QCheckBox( this );
    pRulerMajour->setChecked( pSetting->getSizeRulerMajour() );
    pRulerMajour->setToolTip( tr("Cell size same as ruler majour.") );
    connect( pRulerMajour, SIGNAL(toggled(bool)), SLOT(slotRulerMajour(bool)) );

    pRulerMinor = new QCheckBox( this );
    pRulerMinor->setChecked( pSetting->getSizeRulerMinor() );
    pRulerMinor->setToolTip( tr("Cell size same as ruler minor.") );
    connect( pRulerMinor, SIGNAL(toggled(bool)), SLOT(slotRulerMinor(bool)) );

    pCellX = new QSpinBox( this );
    pCellX->setMinimum( 10 );
    pCellX->setMaximum( 100 );
    pCellX->setValue( pSetting->getSize().width() );
    pCellX->setToolTip( tr("Cell width.") );
    connect( pCellX, SIGNAL(valueChanged(int)), SLOT(slotCellX(int)) );

    pCellY = new QSpinBox( this );
    pCellY->setMinimum( 10 );
    pCellY->setMaximum( 100 );
    pCellY->setValue( pSetting->getSize().height() );
    pCellY->setToolTip( tr("Cell height.") );
    connect( pCellY, SIGNAL(valueChanged(int)), SLOT(slotCellY(int)) );

    pSnap = new QComboBox( this );
    pSnap->addItem( tr("None"), S::GridSnapNone  );
    pSnap->addItem( tr("Line"), S::GridSnapLine  );
    pSnap->addItem( tr("HLine"), S::GridSnapHLine  );
    pSnap->addItem( tr("VLine"), S::GridSnapVLine  );
    pSnap->addItem( tr("Center"), S::GridSnapCenter  );
    pSnap->setCurrentIndex( pSnap->findData( int(pSetting->getSnap()) ) );
    pSnap->setToolTip( tr("Mouse cursor will 'snap' to location based upon this setting.") );
    connect( pSnap, SIGNAL(activated(int)), SLOT(slotSnap(int)) );

    if ( n == Qt::Horizontal )
    {
        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pVisible );
        pLayout->addWidget( pRulerMajour );
        pLayout->addWidget( pRulerMinor );
        pLayout->addWidget( pCellX );
        pLayout->addWidget( pCellY );
        pLayout->addWidget( pSnap );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Visible"), pVisible );
        pLayout->addRow( tr("Ruler (majour)"), pRulerMajour );
        pLayout->addRow( tr("Ruler (minor)"), pRulerMinor );
        pLayout->addRow( tr("Width"), pCellX );
        pLayout->addRow( tr("Height"), pCellY );
        pLayout->addRow( tr("Snap"), pSnap );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void SGridWidget::slotRefresh()
{
    pVisible->setChecked( pSetting->getVisible() );
    pRulerMajour->setChecked( pSetting->getSizeRulerMajour() );
    pRulerMinor->setChecked( pSetting->getSizeRulerMinor() );
    pCellX->setValue( pSetting->getSize().width() );
    pCellY->setValue( pSetting->getSize().height() );
    pSnap->setCurrentIndex( pSnap->findData( int(pSetting->getSnap()) ) );
}

void SGridWidget::slotVisible( bool b )
{
    pSetting->setVisible( b );
}

void SGridWidget::slotRulerMajour( bool b )
{
    pSetting->setSizeRulerMajour( b );
}

void SGridWidget::slotRulerMinor( bool b )
{
    pSetting->setSizeRulerMinor( b );
}

void SGridWidget::slotSnap( int n )
{
    pSetting->setSnap( (S::GridSnaps)pSnap->itemData( n ).toInt() );
}

void SGridWidget::slotCellX( int n )
{
    pSetting->setSize( QSize( n, pCellY->value() ) );
}

void SGridWidget::slotCellY( int n )
{
    pSetting->setSize( QSize( pCellX->value(), n ) );
}


