/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "SSetting.h"

#include "SSettings.h"

SSetting::SSetting( QObject *pParent )
    : QObject( pParent )
{
}

QWidget *SSetting::getButton( QWidget * )
{
    return nullptr;
}

QWidget *SSetting::getToolBar( QWidget * )
{
    return nullptr;
}

QWidget *SSetting::getPanel( QWidget * )
{
    return nullptr;
}

QWidget *SSetting::getPanelMax( QWidget *pParent )
{
    // default to panel + swatch but many may return a qtoolbox to handle a lot of settings
    QWidget *p = new QWidget( pParent );
    QWidget *pPanel = getPanel( p );
    if ( !pPanel ) return p;
    QVBoxLayout *pLayout = new QVBoxLayout( p );
    pLayout->addWidget( pPanel );
    QWidget *pSwatch = getSwatch( p );
    if ( pSwatch )
    {
        pLayout->addWidget( pSwatch );
//        pLayout->addStretch( 10 );
    }
    return p;
}

QWidget *SSetting::getSwatch( QWidget * )
{
    return nullptr;
}

void SSetting::doDialog( QWidget *pParent, bool bThis )
{
    // NOTE: Default is to use the className as the key and as the window title.
    //
    //       Assumes that the toolbar/panel/swatch are autonomous in that they each 
    //       use the setting object directly for read/write/refresh. 
    //
    //       You should override if you want something else. 

    if ( bThis )
    {
        SDialog dialog( pParent, this );
        dialog.exec();
    }
    else
    {
        SDialog dialog( pParent, metaObject()->className() );
        dialog.exec();
    }
}

void SSetting::doDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle, QWidget *pPanel )
{
    SDialog dialog( pParent, stringKey, stringTitle, pPanel );
    dialog.exec();
}

/****************************************************************
 * SDialog
 ****************************************************************/
SDialog::SDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle )
    : QDialog( pParent )
{
    this->stringKey = stringKey;

    setWindowTitle( stringTitle.isEmpty() ? stringKey : stringTitle );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        SSetting *pSetting = g_SSettings->getSetting( stringKey ); Q_ASSERT( pSetting );
        QWidget *pPanel = pSetting->getPanelMax( this );
        pLayoutTop->addWidget( pPanel );
    }

    {
        QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
        pLayoutTop->addWidget( pButtonBox ); 
        connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    }

    doLoadState();
}

SDialog::SDialog( QWidget *pParent, SSetting *pSetting, const QString &stringTitle )
    : QDialog( pParent )
{
    Q_ASSERT( pSetting );

    this->stringKey = pSetting->metaObject()->className();

    setWindowTitle( stringTitle.isEmpty() ? stringKey : stringTitle );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        QWidget *pPanel = pSetting->getPanelMax( this );
        pLayoutTop->addWidget( pPanel );
    }

    {
        QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
        pLayoutTop->addWidget( pButtonBox ); 
        connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    }

    doLoadState();
}

SDialog::SDialog( QWidget *pParent, const QString &stringKey, const QString &stringTitle, QWidget *pPanel )
    : QDialog( pParent )
{
    Q_ASSERT( pPanel );

    this->stringKey = stringKey;
    setWindowTitle( stringTitle.isEmpty() ? stringKey : stringTitle );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pLayoutTop->addWidget( pPanel );

    {
        QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
        pLayoutTop->addWidget( pButtonBox ); 
        connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
    }

    doLoadState();
}

SDialog::~SDialog()
{
    doSaveState();
}

void SDialog::doSaveState()
{
    QSettings settings;
    settings.setValue( stringKey + "/Dialog" + QString("/width"), width() );
    settings.setValue( stringKey + "/Dialog" + QString("/height"), height() );
}

void SDialog::doLoadState()
{
    QSettings settings;
    int nWidth = settings.value( stringKey + "/Dialog" + QString("/width"), width() ).toInt();
    int nHeight= settings.value( stringKey + "/Dialog" + QString("/height"), height() ).toInt();
    resize( nWidth, nHeight );
}

