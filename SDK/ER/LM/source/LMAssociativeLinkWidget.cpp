#include "LibInfo.h"
#include "LMAssociativeLinkWidget.h"

LMAssociativeLinkDetailsWidget::LMAssociativeLinkDetailsWidget( LMAssociativeLink *p, CBD::EOLTypes n, QWidget *pParent )
    : QWidget( pParent )
{
    pAssociativeLink = p;
    nEOL = n;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    QGroupBox *pFrame = new QGroupBox( this );
    QString stringFrameTitle = tr("no entity name");
    ADObject *pObject = pAssociativeLink->getConnected( n == CBD::EOLBegin ? "Begin" : "End" );
    if ( pObject && !pObject->getName().isEmpty() ) stringFrameTitle = pObject->getName();
    
    pFrame->setTitle( stringFrameTitle );

    QFormLayout *pLayout = new QFormLayout( pFrame );

    // Dominant
    pCheckBoxDominant = new QCheckBox( this );
    pCheckBoxDominant->setChecked( pAssociativeLink->isDominant( nEOL ) );
    pLayout->addRow( tr("Dominant:"), pCheckBoxDominant );
    connect( pCheckBoxDominant, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDominant(Qt::CheckState)) );

    // Identified
    pCheckBoxIdentified = new QCheckBox( this );
    pCheckBoxIdentified->setChecked( pAssociativeLink->isIdentified( nEOL ) );
    pLayout->addRow( tr("Identified:"), pCheckBoxIdentified );
    connect( pCheckBoxIdentified, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotIdentified(Qt::CheckState)) );

    // Dependent
    pCheckBoxDependent = new QCheckBox( this );
    pCheckBoxDependent->setChecked( pAssociativeLink->isDependent( nEOL ) );
    pLayout->addRow( tr("Dependent:"), pCheckBoxDependent );
    connect( pCheckBoxDependent, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDependent(Qt::CheckState)) );

    // Cardinality
    QStringList stringlistCardinality  = pAssociativeLink->getCardinality( nEOL ).split( ',' );
    pComboBoxCardinalityMin = new QComboBox( this );
    pComboBoxCardinalityMin->setEditable( true );
    pComboBoxCardinalityMin->addItem( "0" );
    pComboBoxCardinalityMin->addItem( "1" );
    pComboBoxCardinalityMin->addItem( "n" );
    if ( stringlistCardinality.count() > 0 ) pComboBoxCardinalityMin->setCurrentText( stringlistCardinality.at(0) );
    pLayout->addRow( tr("Cardinality Min:"), pComboBoxCardinalityMin );
    connect( pComboBoxCardinalityMin, SIGNAL(editTextChanged(const QString &)), SLOT(slotCardinalityMin(const QString &)) );

    pComboBoxCardinalityMax = new QComboBox( this );
    pComboBoxCardinalityMax->setEditable( true );
    pComboBoxCardinalityMax->addItem( "1" );
    pComboBoxCardinalityMax->addItem( "n" );
    if ( stringlistCardinality.count() > 1 ) pComboBoxCardinalityMax->setCurrentText( stringlistCardinality.at(1) );
    pLayout->addRow( tr("Cardinality Max:"), pComboBoxCardinalityMax );
    connect( pComboBoxCardinalityMax, SIGNAL(editTextChanged(const QString &)), SLOT(slotCardinalityMax(const QString &)) );

    pLineEditRole = new QLineEdit( pAssociativeLink->getRole( nEOL ), this );
    pLayout->addRow( tr("Role:"), pLineEditRole );
    connect( pLineEditRole, SIGNAL(textChanged(const QString &)), SLOT(slotRole(const QString &)) );

    pLayoutTop->addWidget( pFrame );

    connect( pAssociativeLink, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMAssociativeLinkDetailsWidget::slotDominant( Qt::CheckState n )
{
    pAssociativeLink->setDominant( nEOL, n );
}

void LMAssociativeLinkDetailsWidget::slotIdentified( Qt::CheckState n )
{
    pAssociativeLink->setIdentified( nEOL, n );
}

void LMAssociativeLinkDetailsWidget::slotDependent( Qt::CheckState n )
{
    pAssociativeLink->setDependent( nEOL, n );
}

void LMAssociativeLinkDetailsWidget::slotCardinalityMin( const QString &s )
{
    pAssociativeLink->setCardinality( nEOL, s + "," + pComboBoxCardinalityMax->currentText() );
}

void LMAssociativeLinkDetailsWidget::slotCardinalityMax( const QString &s )
{
    pAssociativeLink->setCardinality( nEOL, pComboBoxCardinalityMin->currentText() + "," + s );
}

void LMAssociativeLinkDetailsWidget::slotRole( const QString &s )
{
    pAssociativeLink->setRole( nEOL, s );
}

void LMAssociativeLinkDetailsWidget::slotModified()
{
    pCheckBoxDominant->setChecked( pAssociativeLink->isDominant( nEOL ) );
    pCheckBoxIdentified->setChecked( pAssociativeLink->isIdentified( nEOL ) );
    pCheckBoxDependent->setChecked( pAssociativeLink->isDependent( nEOL ) );
    QStringList stringlistCardinality  = pAssociativeLink->getCardinality( nEOL ).split( ',' );
    if ( stringlistCardinality.count() > 0 ) pComboBoxCardinalityMin->setCurrentText( stringlistCardinality.at(0) );
    if ( stringlistCardinality.count() > 1 ) pComboBoxCardinalityMax->setCurrentText( stringlistCardinality.at(1) );
    pLineEditRole->setText( pAssociativeLink->getRole( nEOL ) );
}

LMAssociativeLinkWidget::LMAssociativeLinkWidget( LMAssociativeLink *p, Qt::Orientation nOrientation, QWidget *pParent )
    : QWidget( pParent )
{
    pAssociativeLink = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        pGenerate = new QCheckBox( this );
        pGenerate->setChecked( pAssociativeLink->isGenerate() );
        connect( pGenerate, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotGenerate(Qt::CheckState)) );

        pLayout->addWidget( new QLabel( tr("Generate:"), this ) );
        pLayout->addWidget( pGenerate );
        pLayout->addStretch( 10 );

        pLayoutTop->addLayout( pLayout );
    }

    {
        QLayout *pLayout;
        if ( nOrientation == Qt::Horizontal ) pLayout = new QHBoxLayout();
        else pLayout = new QVBoxLayout();

        pBegin  = new LMAssociativeLinkDetailsWidget( pAssociativeLink, CBD::EOLBegin, this );
        pEnd    = new LMAssociativeLinkDetailsWidget( pAssociativeLink, CBD::EOLEnd, this );

        pLayout->addWidget( pBegin );
        pLayout->addWidget( pEnd );

        pLayoutTop->addLayout( pLayout );
    }

    connect( pAssociativeLink, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMAssociativeLinkWidget::slotGenerate( Qt::CheckState n )
{
    if ( n ) pAssociativeLink->setGenerate();
    else pAssociativeLink->setGenerate( false );
}

void LMAssociativeLinkWidget::slotModified()
{
    pGenerate->setChecked( pAssociativeLink->isGenerate() );
}


