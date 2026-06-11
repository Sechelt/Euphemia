/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WFindTextDialog.h"

#include "WFindTextWidget.h"

WFindTextDialog::WFindTextDialog( QWidget *pParent )
    : QDialog( pParent, Qt::Tool )
{
    setWindowTitle( tr("Find Text") );

    QVBoxLayout *pLayout = new QVBoxLayout( this );
    pFindTextWidget = new WFindTextWidget( this );
    pLayout->addWidget( pFindTextWidget );

    connect( pFindTextWidget, SIGNAL(signalNext()), SLOT(slotFindNext()) );
    connect( pFindTextWidget, SIGNAL(signalPrevious()), SLOT(slotFindPrevious()) );
    connect( pFindTextWidget, SIGNAL(signalReplace()), SLOT(slotReplace()) );
    connect( pFindTextWidget, SIGNAL(signalReplaceAll()), SLOT(slotReplaceAll()) );
    connect( pFindTextWidget, SIGNAL(signalFindTextChanged(const QString &)), SLOT(slotFindTextChanged(const QString &)) );

    doLoadState();
}

WFindTextDialog::~WFindTextDialog()
{
    doSaveState();
}

void WFindTextDialog::doClear()
{
    if ( bFinding )
    {
        bFinding = false;
    }
    pFindTextWidget->doClear();
}

void WFindTextDialog::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    settings.setValue( s + "/width", width() );
    settings.setValue( s + "/height", height() );
}

void WFindTextDialog::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();

    QSettings settings;
    int nWidth = settings.value( s + "/width", width() ).toInt();
    int nHeight= settings.value( s + "/height", height() ).toInt();
    resize( nWidth, nHeight );
}

void WFindTextDialog::slotFindNext()
{
    if ( bFinding )
    {
        emit signalFindNext();
    }
    else
    {
        emit signalFindFirst( pFindTextWidget->getFindText(),
                              pFindTextWidget->isRegularExpression(),
                              pFindTextWidget->isCaseSensitive(),
                              pFindTextWidget->isWholeWord() );
        bFinding = true;
    }
}

void WFindTextDialog::slotFindPrevious()
{
    if ( bFinding )
    {
        emit signalFindPrevious();
    }
}

void WFindTextDialog::slotReplace()
{
    if ( bFinding )
    {
        emit signalReplace( pFindTextWidget->getReplaceText() );
    }
}

void WFindTextDialog::slotReplaceAll()
{
    if ( bFinding )
    {
        emit signalReplaceAll( pFindTextWidget->getReplaceText() );
        // we should not have a find in motion at this point so...
        bFinding = false;
    }
}

void WFindTextDialog::slotFindTextChanged( const QString & )
{
    if ( bFinding )
    {
        bFinding = false; // this must be first
        emit signalFindTextChanged();
    }
}


