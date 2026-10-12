/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "AppInfo.h"
#include "EStartLayerDialog.h"

//
// EStartLayerDialog
//
EStartLayerDialog::EStartLayerDialog( QWidget *pParent )
    : QDialog( pParent )
{
    setObjectName( "EStartLayerDialog" );
    setWindowTitle( tr("Start Layer...") );

    QVBoxLayout *pLayoutDialog = new QVBoxLayout( this );

    pStartLayerWidget = new EStartLayerWidget( this );
    pLayoutDialog->addWidget( pStartLayerWidget );

    QDialogButtonBox *pButtonBox = new QDialogButtonBox( QDialogButtonBox::Ok, this );
    pLayoutDialog->addWidget( pButtonBox ); 

    connect( pButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
}

bool EStartLayerDialog::isNone()        
{ 
    return pStartLayerWidget->pNone->isChecked();      
}

bool EStartLayerDialog::isPaint()    
{ 
    return pStartLayerWidget->pPaint->isChecked();  
}

bool EStartLayerDialog::isDraw()      
{ 
    return pStartLayerWidget->pDraw->isChecked();    
}

//
// EStartLayerWidget
//
EStartLayerWidget::EStartLayerWidget( QWidget *pParent )
    : QWidget( pParent )
{
    setObjectName( "EStartLayerWidget" );

    // create buttons in a group box
    QGroupBox *pGroupBox = new QGroupBox( tr("Initial Layer"), this );
    pNone   = new QRadioButton( tr("None"), pGroupBox );
    pPaint  = new QRadioButton( tr("Paint"), pGroupBox );
    pDraw   = new QRadioButton( tr("Draw"), pGroupBox );
    pNone->setChecked( true );
    // add a layout to the group box
    QVBoxLayout *pLayoutButtons = new QVBoxLayout();
    pLayoutButtons->addWidget( pNone );
    pLayoutButtons->addWidget( pPaint );
    pLayoutButtons->addWidget( pDraw );
    pLayoutButtons->addStretch( 1 );
    pGroupBox->setLayout( pLayoutButtons );
    // add group box to top-level layout
    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    pLayoutTop->addWidget( pGroupBox );
}


