/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WTabDialog.h"

WTabDialog::WTabDialog( QWidget *pwidgetParent, const QString &stringTitle, const QString &stringName, QDialogButtonBox::StandardButtons buttons, bool bModal, Qt::WindowFlags f )
    : QDialog( pwidgetParent, f )
{
    setObjectName( stringName );
    setModal( bModal );
    setWindowTitle( stringTitle );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pTabWidget = new QTabWidget( this );
    pLayout->addWidget( pTabWidget );

    pButtonBox = new QDialogButtonBox( buttons /* QDialogButtonBox::Ok | QDialogButtonBox::Cancel */, this );
    pLayout->addWidget( pButtonBox );

    connect(pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    doLoadState();
}

WTabDialog::~WTabDialog()
{
    doSaveState();
}

/*
void WTabDialog::closeEvent( QCloseEvent *pEvent )
{                                                 
    doSaveState();                                
    QDialog::closeEvent( pEvent );                
}                                                 
*/

void WTabDialog::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    settings.setValue( s + "/width", width() );
    settings.setValue( s + "/height", height() );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName();
    // if ( s.isEmpty() ) s = metaObject()->className();
    // s += "/";
    // QSettings settings;
    // settings.setValue( s + "geometry", saveGeometry() );
}

void WTabDialog::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    int nWidth = settings.value( s + "/width", width() ).toInt();
    int nHeight= settings.value( s + "/height", height() ).toInt();
    resize( nWidth, nHeight );

    return;
    // the following is not good when going on and off of a dual display etc
    // QString s = objectName();
    // if ( s.isEmpty() ) s = metaObject()->className();
    // s += "/";
    // QSettings settings;
    // restoreGeometry( settings.value( s + "geometry" ).toByteArray() );
}

