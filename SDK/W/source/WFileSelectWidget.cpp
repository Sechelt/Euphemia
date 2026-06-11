/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Euphemia codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WFileSelectWidget.h"
#include "W.h"

WFileSelectWidget::WFileSelectWidget( const QString &stringFileName, const QString &stringFileSpec, bool bNew, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    this->stringFileSpec = stringFileSpec;

    QHBoxLayout *pLayout = new QHBoxLayout( this );
    pFileName = new QLineEdit( stringFileName, this );
    pLayout->addWidget( pFileName );

    if ( bNew )
    {
        pNew = new QPushButton( QIcon( ":DATAW/New" ),  QString(), this );
        pNew->setToolTip( tr("new file") );
        pLayout->addWidget( pNew );
        connect( pNew, SIGNAL(clicked()), SLOT(slotNew()) );
    }

    pOpen = new QPushButton( QIcon( ":DATAW/Open" ),  QString(), this );
    pOpen->setToolTip( tr("open file") );
    pLayout->addWidget( pOpen );
    connect( pOpen, SIGNAL(clicked()), SLOT(slotOpen()) );
}

void WFileSelectWidget::slotNew()
{
    QString stringFileName = QFileDialog::getExistingDirectory( this, tr("Select directory...") );                                                                                                                                      
    if ( stringFileName.isEmpty() ) return;

    pFileName->setText( stringFileName );
}

void WFileSelectWidget::slotOpen()
{
    QString stringFileName = QFileDialog::getOpenFileName( this, tr("Select file..."), QString(), stringFileSpec );                                                                                                                                      
    if ( stringFileName.isEmpty() ) return;

    pFileName->setText( stringFileName );
}


