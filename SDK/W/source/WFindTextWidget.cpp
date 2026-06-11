/*
 * Copyright 2020-2026, Peter Harvey <pharvey@codebydesign.com>
 * All rights reserved.
 *
 * This file is part of the Blue codebase.
 * [placeholder]
 *
 */

#include "LibInfo.h"
#include "WFindTextWidget.h"

//
// WFindTextWidget
//
WFindTextWidget::WFindTextWidget( QWidget *pParent )
    : QWidget( pParent )
{
    pFindText       = new QLineEdit( this );
    pReplaceText    = new QLineEdit( this );
    pNext           = new QToolButton( this ); 
    pPrevious       = new QToolButton( this ); 
    pReplace        = new QToolButton( this ); 
    pReplaceAll     = new QToolButton( this ); 

    // options
    {
        pExp            = new QToolButton( this );
        pCaseSensitive  = new QToolButton( this );
        pWholeWord      = new QToolButton( this );

        {
            QIcon icon;
            icon.addPixmap( QPixmap( ":W/RegularExpression" ), QIcon::Normal, QIcon::Off );
            icon.addPixmap( QPixmap( ":W/RegularExpressionOn" ), QIcon::Normal, QIcon::On );
            pExp->setIcon( icon );          
        }
        {
            QIcon icon;
            icon.addPixmap( QPixmap( ":W/MatchCase" ), QIcon::Normal, QIcon::Off );
            icon.addPixmap( QPixmap( ":W/MatchCaseOn" ), QIcon::Normal, QIcon::On );
            pCaseSensitive->setIcon( icon );          
        }
        {
            QIcon icon;
            icon.addPixmap( QPixmap( ":W/WholeWord" ), QIcon::Normal, QIcon::Off );
            icon.addPixmap( QPixmap( ":W/WholeWordOn" ), QIcon::Normal, QIcon::On );
            pWholeWord->setIcon( icon );          
        }

        pExp->setToolTip( tr("Regular Expression") );          
        pCaseSensitive->setToolTip( tr("Case Sensitive") );  
        pWholeWord->setToolTip( tr("Whole Word") );    

        pExp->setCheckable( true );
        pCaseSensitive->setCheckable( true );
        pWholeWord->setCheckable( true );

        pCaseSensitive->setChecked( true );
        pWholeWord->setChecked( true );
    }

    pNext->setIcon( QIcon( ":W/Next" ) );          
    pPrevious->setIcon( QIcon( ":W/Previous" ) );  
    pReplace->setIcon( QIcon( ":W/Replace" ) );    
    pReplaceAll->setIcon( QIcon( ":W/ReplaceAll" ) );

    pNext->setToolTip( tr("Next") );          
    pPrevious->setToolTip( tr("Previous") );  
    pReplace->setToolTip( tr("Replace") );    
    pReplaceAll->setToolTip( tr("Replace All") );

    QHBoxLayout *pLayoutH = new QHBoxLayout( this );
    QVBoxLayout *pLayoutV = new QVBoxLayout();
    QGridLayout *pLayout = new QGridLayout();

    // row 0
    pLayout->addWidget( new QLabel( tr("Find:"), this ), 0, 0 );
    pLayout->addWidget( pFindText, 0, 1 );
    pLayout->addWidget( pPrevious, 0, 2 );
    pLayout->addWidget( pNext, 0, 3 );
    pLayout->addWidget( pExp, 0, 4 );
    pLayout->addWidget( pCaseSensitive, 0, 5 );
    pLayout->addWidget( pWholeWord, 0, 6 );
    // row 1
    pLayout->addWidget( new QLabel( tr("Replace:"), this ), 1, 0 );
    pLayout->addWidget( pReplaceText, 1, 1 );
    pLayout->addWidget( pReplace, 1, 2 );
    pLayout->addWidget( pReplaceAll, 1, 3 );

    pLayoutV->addLayout( pLayout, 10 );
    pLayoutH->addLayout( pLayoutV, 10 );

    connect( pFindText, SIGNAL(textChanged(const QString &)), SIGNAL(signalFindTextChanged(const QString &)) );
    connect( pNext, SIGNAL(clicked()), SIGNAL(signalNext()) );
    connect( pPrevious, SIGNAL(clicked()), SIGNAL(signalPrevious()) );
    connect( pReplace, SIGNAL(clicked()), SIGNAL(signalReplace()) );
    connect( pReplaceAll, SIGNAL(clicked()), SIGNAL(signalReplaceAll()) );
}

void WFindTextWidget::doClear()
{
    pFindText->clear();
    pReplaceText->clear();
}

