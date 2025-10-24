/*
 * Copyright 2022-2024, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * This source code is not meant for public distribution.
 *
 */

#include "LibInfo.h"
#include "SSetting.h"

#include "SSettings.h"

SSetting::SSetting( QObject *pParent )
    : QObject( pParent )
{
}

QWidget *SSetting::getSwatch( QWidget *pParent )
{
    Q_UNUSED( pParent );
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
        SDialog dialog( pParent, metaObject()->className());
        dialog.exec();
    }
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

    SSetting *pSetting = g_SSettings->getSetting( stringKey );
    if ( pSetting )
    {
        pPanel = pSetting->getPanel( this );
        QWidget *pSwatch    = pSetting->getSwatch( this );

        // no panel? try tool bar widget
        if ( !pPanel ) pPanel = pSetting->getToolBar( this );

        QVBoxLayout *pLayout = new QVBoxLayout();

        // panel
        if ( pPanel ) pLayout->addWidget( pPanel );
        // swatch
        if ( pSwatch )
        {
            pLayout->addStretch( 10 );
            pLayout->addWidget( pSwatch );
        }

        pLayoutTop->addLayout( pLayout );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Setting not found [" << stringKey << "].";
    }

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutTop->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );

    doLoadState();
}

SDialog::SDialog( QWidget *pParent, SSetting *pSetting, const QString &stringTitle )
    : QDialog( pParent )
{
    this->stringKey = stringKey;

    setWindowTitle( stringTitle.isEmpty() ? stringKey : stringTitle );

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    if ( pSetting )
    {
        pPanel = pSetting->getPanel( this );
        QWidget *pSwatch    = pSetting->getSwatch( this );

        // no panel? try tool bar widget
        if ( !pPanel ) pPanel = pSetting->getToolBar( this );

        QVBoxLayout *pLayout = new QVBoxLayout();

        // panel
        if ( pPanel ) pLayout->addWidget( pPanel );
        // swatch
        if ( pSwatch )
        {
            pLayout->addStretch( 10 );
            pLayout->addWidget( pSwatch );
        }

        pLayoutTop->addLayout( pLayout );
    }
    else
    {
        qInfo() << "[" << __FILE__ << "][" << __FUNCTION__ << "][" << __LINE__ << "] Setting not found [" << stringKey << "].";
    }

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutTop->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );

    doLoadState();
}

SDialog::~SDialog()
{
    doSaveState();
}

void SDialog::doSaveState()
{
    if ( !pPanel ) return;
    QSettings settings;
    settings.setValue( pPanel->metaObject()->className() + QString("/width"), width() );
    settings.setValue( pPanel->metaObject()->className() + QString("/height"), height() );
}

void SDialog::doLoadState()
{
    if ( !pPanel ) return;
    QSettings settings;
    int nWidth = settings.value( pPanel->metaObject()->className() + QString("/width"), width() ).toInt();
    int nHeight= settings.value( pPanel->metaObject()->className() + QString("/height"), height() ).toInt();
    resize( nWidth, nHeight );
}

