#include "LibInfo.h"
#include "WDataNavWidget.h"

WDataNavWidget::WDataNavWidget( QWidget *pParent )
    : QWidget( pParent )
{
    QHBoxLayout *pLayout = new QHBoxLayout( this );

    QLabel *pLabel = new QLabel( this );
    pLabel->setText( tr("Record:") );
    pLayout->addWidget( pLabel );

    pFirst      = new QToolButton( this );
    pFirst->setIcon( QPixmap( ":W/RecordFirst" ).scaled( 24, 24 ) );
    pFirst->setFixedSize( 24, 24 );
    pLayout->addWidget( pFirst );
    connect( pFirst, SIGNAL(clicked()), this, SIGNAL(signalFirst()) );

    pPrevious   = new QToolButton( this );    
    pPrevious->setIcon( QPixmap( ":W/RecordPrevious" ).scaled( 24, 24 ) );
    pPrevious->setFixedSize( 24, 24 );
    pLayout->addWidget( pPrevious );
    connect( pPrevious, SIGNAL(clicked()), this, SIGNAL(signalPrevious()) );

    pRecord   = new QLineEdit( this );    
    pRecord->setReadOnly( true );
    pRecord->setText( "0 of 0" );
    pLayout->addWidget( pRecord );

    pNext       = new QToolButton( this );
    pNext->setIcon( QPixmap( ":W/RecordNext" ).scaled( 24, 24 ) );
    pNext->setFixedSize( 24, 24 );
    pLayout->addWidget( pNext );
    connect( pNext, SIGNAL(clicked()), this, SIGNAL(signalNext()) );

    pLast       = new QToolButton( this );
    pLast->setIcon( QPixmap( ":W/RecordLast" ).scaled( 24, 24 ) );
    pLast->setFixedSize( 24, 24 );
    pLayout->addWidget( pLast );
    connect( pLast, SIGNAL(clicked()), this, SIGNAL(signalLast()) );

    pNew        = new QToolButton( this );
    pNew->setIcon( QPixmap( ":W/RecordNew" ).scaled( 24, 24 ) );
    pNew->setFixedSize( 24, 24 );
    pLayout->addWidget( pNew );
    connect( pNew, SIGNAL(clicked()), this, SIGNAL(signalNew()) );

    pDelete        = new QToolButton( this );
    pDelete->setIcon( QPixmap( ":W/RecordDelete" ).scaled( 24, 24 ) );
    pDelete->setFixedSize( 24, 24 );
    pLayout->addWidget( pDelete );
    connect( pDelete, SIGNAL(clicked()), this, SIGNAL(signalDelete()) );

/*
    pLabel = new QLabel( this );
    pLabel->setText( tr("Selection:") );
    pLayout->addWidget( pLabel );

    pSelection   = new QLineEdit( this );    
    pSelection->setReadOnly( true );
    pSelection->setText( "0,0 to 0,0" );
    pLayout->addWidget( pSelection );
*/
    pLayout->addStretch( 10 );
}

WDataNavWidget::~WDataNavWidget()
{
}

void WDataNavWidget::setRecord( int n )
{
    nRecord = n;
    setRecord();
}

void WDataNavWidget::setRecords( int n )
{
    nRecords = n;
    setRecord();
}

/*
void WDataNavWidget::setSelection( int nRow1, int nCol1, int nRow2, int nCol2 )
{
    pSelection->setText( QString( tr("%1,%2 to %3,%4") ).arg( nRow1 ).arg( nCol1 ).arg( nRow2 ).arg( nCol2 ) );
}
*/

void WDataNavWidget::setRecord()
{
    pRecord->setText( QString( tr("%1 of %2") ).arg( nRecord ).arg( nRecords ) );

    if ( nRecord < 1 )
        pDelete->setEnabled( false );
    else
        pDelete->setEnabled( true );

    if ( nRecord <= 1 ) 
    {
        pFirst->setEnabled( false );
        pPrevious->setEnabled( false );
    }
    else
    {
        pFirst->setEnabled( true );
        pPrevious->setEnabled( true );
    }

    if ( nRecord >= nRecords ) 
    {
        pLast->setEnabled( false );
        pNext->setEnabled( false );
    }
    else
    {
        pLast->setEnabled( true );
        pNext->setEnabled( true );
    }
}

