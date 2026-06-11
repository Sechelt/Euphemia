/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "LSPage.h"

#include <SSettings.h>

/****************************************************************
 * LSPage
 ****************************************************************/
LSPage::LSPage()
{
}

void LSPage::setSize( const QSizeF &t )
{
    if ( sizeDefault == t ) return;
    sizeDefault = t;
    emit signalChanged();
}

void LSPage::setMargins( const QSize &t )
{
    if ( sizeMargins == t ) return;
    sizeMargins = t;
    emit signalChanged();
}

void LSPage::setPages( const QSize &t )
{
    if ( sizePages == t ) return;
    sizePages = t;
    emit signalChanged();
}

void LSPage::setPrinter( bool t )
{
    if ( bPrinter == t ) return;
    bPrinter = t;
    emit signalChanged();
}

void LSPage::setDrawMargins( bool t )
{
    if ( bDrawMargins == t ) return;
    bDrawMargins = t;
    emit signalChanged();
}

void LSPage::setAutoPages( bool t )
{
    if ( bAutoPages == t ) return;
    bAutoPages = t;
    emit signalChanged();
}

void LSPage::setSelected( const QSizeF &sizeDefault, const QSize &sizeMargins, const QSize &sizePages, bool bPrinter, bool bDrawMargins, bool bAutoPages )
{
    this->sizeDefault   = sizeDefault;
    this->sizeMargins   = sizeMargins;
    this->sizePages     = sizePages;
    this->bPrinter      = bPrinter;
    this->bDrawMargins  = bDrawMargins;
    this->bAutoPages    = bAutoPages;
    emit signalSync();
}

QWidget *LSPage::getToolBar( QWidget *pParent )
{
    return new LSPageWidget( pParent, Qt::Horizontal );
}

QWidget *LSPage::getPanel( QWidget *pParent )
{
    return new LSPageWidget( pParent, Qt::Vertical );
}

void LSPage::doSave( QDomDocument *pdomDoc, QDomElement *pdomElem )
{
    QDomElement domElem = pdomDoc->createElement( "LSPage" );

    domElem.setAttribute( "DefaultX", sizeDefault.width() );
    domElem.setAttribute( "DefaultY", sizeDefault.height() );
    domElem.setAttribute( "MarginsX", sizeMargins.width() );
    domElem.setAttribute( "MarginsY", sizeMargins.height() );
    domElem.setAttribute( "PagesX", sizePages.width() );
    domElem.setAttribute( "PagesY", sizePages.height() );
    domElem.setAttribute( "Printer", bPrinter );
    domElem.setAttribute( "DrawMargins", bDrawMargins );
    domElem.setAttribute( "AutoPages", bAutoPages );

    pdomElem->appendChild( domElem );
}

void LSPage::doLoad( QDomElement *pdomElem )
{
    sizeDefault.setWidth( pdomElem->attribute( "DefaultX", QString::number( sizeDefault.width() ) ).toInt() );
    sizeDefault.setHeight( pdomElem->attribute( "DefaultY", QString::number( sizeDefault.height() ) ).toInt() );
    sizeMargins.setWidth( pdomElem->attribute( "MarginsX", QString::number( sizeMargins.width() ) ).toInt() );
    sizeMargins.setHeight( pdomElem->attribute( "MarginsY", QString::number( sizeMargins.height() ) ).toInt() );
    sizePages.setWidth( pdomElem->attribute( "PagesX", QString::number( sizePages.width() ) ).toInt() );
    sizePages.setHeight( pdomElem->attribute( "PagesY", QString::number( sizePages.height() ) ).toInt() );
    bPrinter        = pdomElem->attribute( "Printer", QString::number( bPrinter ) ).toInt();
    bDrawMargins    = pdomElem->attribute( "DrawMargins", QString::number( bDrawMargins ) ).toInt();
    bAutoPages      = pdomElem->attribute( "AutoPages", QString::number( bAutoPages ) ).toInt();
}

/****************************************************************
 * LSPageWidget
 ****************************************************************/
LSPageWidget::LSPageWidget( QWidget *pParent, Qt::Orientation n )
    : QWidget( pParent )
{
    setObjectName( "LSPageWidget" );

    pSetting = (LSPage*)g_SSettings->getSetting( "LSPage" );
    if ( !pSetting )
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Could not find [LSPage].";
        return;
    }

    pSizeX = new QSpinBox( this );
    pSizeX->setMinimum( 2 );
    pSizeX->setMaximum( 10000 );
    pSizeX->setValue( pSetting->getSize().width() );
    pSizeX->setToolTip( tr("Default page/canvas width.") );
    connect( pSizeX, SIGNAL(valueChanged(int)), SLOT(slotSizeX(int)) );

    pSizeY = new QSpinBox( this );
    pSizeY->setMinimum( 2 );
    pSizeY->setMaximum( 10000 );
    pSizeY->setValue( pSetting->getSize().height() );
    pSizeY->setToolTip( tr("Default page/canvas height.") );
    connect( pSizeY, SIGNAL(valueChanged(int)), SLOT(slotSizeY(int)) );

    pMarginsX = new QSpinBox( this );
    pMarginsX->setMinimum( 0 );
    pMarginsX->setMaximum( 100 );
    pMarginsX->setValue( pSetting->getMargins().width() );
    pMarginsX->setToolTip( tr("Margin on sides.") );
    connect( pMarginsX, SIGNAL(valueChanged(int)), SLOT(slotMarginsX(int)) );

    pMarginsY = new QSpinBox( this );
    pMarginsY->setMinimum( 0 );
    pMarginsY->setMaximum( 100 );
    pMarginsY->setValue( pSetting->getMargins().height() );
    pMarginsY->setToolTip( tr("Margin top and bottom") );
    connect( pMarginsY, SIGNAL(valueChanged(int)), SLOT(slotMarginsY(int)) );

    pPagesX = new QSpinBox( this );
    pPagesX->setMinimum( 1 );
    pPagesX->setMaximum( 10000 );
    pPagesX->setValue( pSetting->getPages().width() );
    pPagesX->setToolTip( tr("Pages horizontal.") );
    connect( pPagesX, SIGNAL(valueChanged(int)), SLOT(slotPagesX(int)) );

    pPagesY = new QSpinBox( this );
    pPagesY->setMinimum( 1 );
    pPagesY->setMaximum( 10000 );
    pPagesY->setValue( pSetting->getPages().height() );
    pPagesY->setToolTip( tr("Pages vertical.") );
    connect( pPagesY, SIGNAL(valueChanged(int)), SLOT(slotPagesY(int)) );

    pPrinter = new QCheckBox( this );
    pPrinter->setTristate( false );
    pPrinter->setChecked( pSetting->getPrinter() );
    pPrinter->setToolTip( tr("User current printer pages size.") );
    connect( pPrinter, SIGNAL(toggled(bool)), SLOT(slotPrinter(bool)) );

    pDrawMargins = new QCheckBox( this );
    pDrawMargins->setTristate( false );
    pDrawMargins->setChecked( pSetting->getDrawMargins() );
    pDrawMargins->setToolTip( tr("Draw margins.") );
    connect( pDrawMargins, SIGNAL(toggled(bool)), SLOT(slotDrawMargins(bool)) );

    pAutoPages = new QCheckBox( this );
    pAutoPages->setTristate( false );
    pAutoPages->setChecked( pSetting->getAutoPages() );
    pAutoPages->setToolTip( tr("Provide optimal space for contents by automatically creating/deleting pages.") );
    connect( pAutoPages, SIGNAL(toggled(bool)), SLOT(slotAutoPages(bool)) );

    if ( n == Qt::Horizontal )
    {
        pPrinter->setText( tr("Printer Page Size") );
        pDrawMargins->setText( tr("Draw Margins") );
        pAutoPages->setText( tr("Auto Pages") );

        QHBoxLayout *pLayout = new QHBoxLayout( this );
        pLayout->addWidget( pSizeX );
        pLayout->addWidget( pSizeY );
        pLayout->addWidget( pMarginsX );
        pLayout->addWidget( pMarginsY );
        pLayout->addWidget( pPagesX );
        pLayout->addWidget( pPagesY );
        pLayout->addWidget( pPrinter );
        pLayout->addWidget( pDrawMargins );
        pLayout->addWidget( pAutoPages );
        pLayout->addStretch( 10 );
    }
    else
    {
        QFormLayout *pLayout = new QFormLayout( this );
        pLayout->addRow( tr("Page Width"), pSizeX );
        pLayout->addRow( tr("Page Height"), pSizeY );
        pLayout->addRow( tr("Margin Sides"), pMarginsX );
        pLayout->addRow( tr("Margin Top/Bottom"), pMarginsY );
        pLayout->addRow( tr("Pages Horizontal"), pPagesX );
        pLayout->addRow( tr("Pages Vertical"), pPagesY );
        pLayout->addRow( tr("Printer Page Size"), pPrinter );
        pLayout->addRow( tr("Draw Margins"), pDrawMargins );
        pLayout->addRow( tr("Auto Pages"), pAutoPages );
    }

    connect( pSetting, SIGNAL(signalChanged()), SLOT(slotRefresh()) );
    connect( pSetting, SIGNAL(signalSync()), SLOT(slotRefresh()) );
}

void LSPageWidget::slotRefresh()
{
    pSizeX->setValue( pSetting->getSize().width() );
    pSizeY->setValue( pSetting->getSize().height() );
}

void LSPageWidget::slotSizeX( int n )
{
    pSetting->setSize( QSizeF( n, pSizeY->value() ) );
}

void LSPageWidget::slotSizeY( int n )
{
    pSetting->setSize( QSizeF( pSizeX->value(), n ) );
}

void LSPageWidget::slotMarginsX( int n )
{
    pSetting->setMargins( QSize( n, pMarginsY->value() ) );
}

void LSPageWidget::slotMarginsY( int n )
{
    pSetting->setMargins( QSize( pMarginsX->value(), n ) );
}

void LSPageWidget::slotPagesX( int n )
{
    pSetting->setPages( QSize( n, pPagesY->value() ) );
}

void LSPageWidget::slotPagesY( int n )
{
    pSetting->setPages( QSize( pPagesX->value(), n ) );
}

void LSPageWidget::slotPrinter( bool b )
{
    pSetting->setPrinter( b );
}

void LSPageWidget::slotDrawMargins( bool b )
{
    pSetting->setDrawMargins( b );
}

void LSPageWidget::slotAutoPages( bool b )
{
    pSetting->setAutoPages( b );
}


