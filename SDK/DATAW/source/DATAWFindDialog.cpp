#include "LibInfo.h"

#include <QtWidgets>

#include "DATAWFindDialog.h"
#include "DATASqlWidget.h"

DATAWFindDialog::DATAWFindDialog( QWidget *parent, DATASqlWidget *pSQLWidget, bool bReplace )
    : QDialog( parent )
{
    if ( bReplace ) setWindowTitle( tr( "Replace..." ) );
    else setWindowTitle( tr( "Find..." ) );

    this->pSQLWidget = pSQLWidget;

    pReplace = nullptr;
    pButtonReplace = nullptr;

    bool bHasSelectedText = pSQLWidget->hasSelection(); 

    QGridLayout *mainLayout = new QGridLayout( this );
    mainLayout->setSizeConstraint( QLayout::SetFixedSize );

    //
    QFormLayout *topLeftLayout = new QFormLayout;
    topLeftLayout->addRow( new QLabel( tr( "Find:" ), this ), pFind = new QLineEdit( this ) );
    if ( bReplace ) topLeftLayout->addRow( new QLabel( tr( "Replace:" ), this ), pReplace = new QLineEdit( this ) );

    connect( pFind, SIGNAL(textChanged(const QString &)), SLOT(slotStateChanged(const QString &)) );

    //
    {
        QVBoxLayout *leftLayout = new QVBoxLayout;
        pCaseSensitive = new QCheckBox( tr( "Match &case" ) );
        pCaseSensitive->setChecked( true );
        pFromStart = new QCheckBox( tr( "Search from &start" ) );
        if ( !bHasSelectedText ) pFromStart->setChecked( true );
        pWholeWords = new QCheckBox( tr( "&Whole words" ) );
        pWholeWords->setChecked( true );
        pExpression = new QCheckBox( tr( "Expression" ) );
        pSelection = new QCheckBox( tr( "Search se&lection" ) );
        if ( bHasSelectedText ) pSelection->setChecked( true );
        leftLayout->addLayout( topLeftLayout );
        leftLayout->addWidget( pCaseSensitive );
        leftLayout->addWidget( pFromStart );
        leftLayout->addWidget( pWholeWords );
        leftLayout->addWidget( pExpression );
        leftLayout->addWidget( pSelection );

        mainLayout->addLayout( leftLayout, 0, 0 );

        connect( pCaseSensitive, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStateChanged(Qt::CheckState)) );
        connect( pFromStart, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStateChanged(Qt::CheckState)) );
        connect( pWholeWords, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStateChanged(Qt::CheckState)) );
        connect( pExpression, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStateChanged(Qt::CheckState)) );
        connect( pSelection, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotStateChanged(Qt::CheckState)) );
    }

    // vertical dialog buttons
    {
        pButtonBox = new QDialogButtonBox( Qt::Vertical );
        pButtonFind = new QPushButton( tr( "&Find" ) );
        pButtonFind->setDefault( true );
        if ( bReplace )  pButtonReplace = new QPushButton( tr( "&Replace" ) );
        if ( bReplace )  pButtonReplaceAll = new QPushButton( tr( "Replace &All" ) );
        pButtonBox->addButton( pButtonFind, QDialogButtonBox::ActionRole );
        if ( bReplace ) pButtonBox->addButton( pButtonReplace, QDialogButtonBox::ActionRole );
        if ( bReplace ) pButtonBox->addButton( pButtonReplaceAll, QDialogButtonBox::ActionRole );

        mainLayout->addWidget( pButtonBox, 0, 1 );

        connect( pButtonFind, SIGNAL(clicked()), SLOT(slotFind()) );
        if ( bReplace ) connect( pButtonReplace, SIGNAL(clicked()), SLOT(slotReplace()) );
        if ( bReplace ) pButtonReplace->setEnabled( false );
        if ( bReplace ) connect( pButtonReplaceAll, SIGNAL(clicked()), SLOT(slotReplaceAll()) );
        if ( bReplace ) pButtonReplaceAll->setEnabled( false );
    }

    mainLayout->setRowStretch( 2, 1 );
}

void DATAWFindDialog::slotFind()
{
    bool bMatch = false;

    // pCaseSensitive->isChecked();
    // pWholeWords->isChecked();

    if ( pExpression->isChecked() )
        bMatch = pSQLWidget->doFind( QRegularExpression( pFind->text() ) );
    else
        bMatch = pSQLWidget->doFind( pFind->text() );

    if ( pReplace )
    { 
        if ( bMatch )
        {
            pButtonReplace->setEnabled( true );
            pButtonReplaceAll->setEnabled( true );
        }
        else
        {
            pButtonReplace->setEnabled( false );
            pButtonReplaceAll->setEnabled( false );
        }
    }
}

void DATAWFindDialog::slotReplace()
{
    pSQLWidget->doReplace( pReplace->text() );
    slotFind();
}

void DATAWFindDialog::slotReplaceAll()
{
    // Try to prevent looping. This can happen when auto wrapping and Find and Replace are same.
    // The following helps but does not handle case where Find is a reg exp so a count is implemented.
    // \todo Handle better by knowing when we have reached start point.
    if ( pFind->text() == pReplace->text() ) return;

    int nCount = 0;
    int nMax = 10000;

    while ( pReplace->isEnabled() && nCount < nMax )
    {
        nCount++;
        pSQLWidget->doReplace( pReplace->text() );
        slotFind();
    }
}

void DATAWFindDialog::slotStateChanged( const QString & )
{
    if ( pReplace )
    {
        pButtonReplace->setEnabled( false );
        pButtonReplaceAll->setEnabled( false );
    }
}

void DATAWFindDialog::slotStateChanged( Qt::CheckState )
{
    if ( pReplace )
    {
        pButtonReplace->setEnabled( false );
        pButtonReplaceAll->setEnabled( false );
    }
}

