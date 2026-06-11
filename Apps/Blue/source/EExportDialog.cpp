/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "AppInfo.h"
#include "EExportDialog.h"

//
// EExportDialog
//
EExportDialog::EExportDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setObjectName( "EExportDialog" );
    setWindowTitle( tr("Export...") );

    QVBoxLayout *pLayoutDialog = new QVBoxLayout( this );

    pExportWidget = new EExportWidget( this );
    pLayoutDialog->addWidget( pExportWidget );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutDialog->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
}

bool EExportDialog::isAll()        
{ 
    return pExportWidget->pAll->isChecked();      
}

bool EExportDialog::isCurrent()    
{ 
    return pExportWidget->pCurrent->isChecked();  
}

bool EExportDialog::isImage()      
{ 
    return pExportWidget->pImage->isChecked();    
}

bool EExportDialog::isXML()        
{ 
    return pExportWidget->pXML->isChecked();      
}

//
// EExportWidget
//
EExportWidget::EExportWidget( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "EExportWidget" );

    QVBoxLayout *pLayoutTop     = new QVBoxLayout( this );
    QVBoxLayout *pLayoutGroup;

    QGroupBox *pGroupLayer = new QGroupBox( tr("Layer"), this );
    pCurrent    = new QRadioButton( tr("Current") );
    pAll        = new QRadioButton( tr("All") );
    pCurrent->setChecked( true );
    pLayoutGroup= new QVBoxLayout();
    pLayoutGroup->addWidget( pCurrent );
    pLayoutGroup->addWidget( pAll );
    pGroupLayer->setLayout( pLayoutGroup );
    pLayoutTop->addWidget( pGroupLayer );

    QGroupBox *pGroupFormat = new QGroupBox( tr("Format"), this );
    pImage      = new QRadioButton( tr("Image") );
    pXML        = new QRadioButton( tr("XML") );
    pImage->setChecked( true );
    pLayoutGroup= new QVBoxLayout();
    pLayoutGroup->addWidget( pImage );
    pLayoutGroup->addWidget( pXML );
    pGroupFormat->setLayout( pLayoutGroup );
    pLayoutTop->addWidget( pGroupFormat );
}


