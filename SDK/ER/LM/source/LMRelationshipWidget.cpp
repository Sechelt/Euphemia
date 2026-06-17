#include "LibInfo.h"
#include "LMRelationshipWidget.h"

LMRelationshipDetailsWidget::LMRelationshipDetailsWidget( LMRelationship *p, CBD::EOLTypes n, QWidget *pParent )
    : QWidget( pParent )
{
    pRelationship = p;
    nEOL = n;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    QGroupBox *pFrame = new QGroupBox( this );
    QString stringFrameTitle = tr("no entity name");
    ADObject *pObject = pRelationship->getConnected( n == CBD::EOLBegin ? "Begin" : "End" );
    if ( pObject && !pObject->getName().isEmpty() ) stringFrameTitle = pObject->getName();
    
    pFrame->setTitle( stringFrameTitle );

    QFormLayout *pLayout = new QFormLayout( pFrame );

    // Dominant
    pCheckBoxDominant = new QCheckBox( this );
    pCheckBoxDominant->setChecked( pRelationship->isDominant( nEOL ) );
    pLayout->addRow( tr("Dominant:"), pCheckBoxDominant );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxDominant, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDominant(Qt::CheckState)) );
#else
    connect( pCheckBoxDominant, SIGNAL(stateChanged(int)), SLOT(slotDominant(int)) );
#endif
    // Identified
    pCheckBoxIdentified = new QCheckBox( this );
    pCheckBoxIdentified->setChecked( pRelationship->isIdentified( nEOL ) );
    pLayout->addRow( tr("Identified:"), pCheckBoxIdentified );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxIdentified, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotIdentified(Qt::CheckState)) );
#else
    connect( pCheckBoxIdentified, SIGNAL(stateChanged(int)), SLOT(slotIdentified(int)) );
#endif
    // Dependent
    pCheckBoxDependent = new QCheckBox( this );
    pCheckBoxDependent->setChecked( pRelationship->isDependent( nEOL ) );
    pLayout->addRow( tr("Dependent:"), pCheckBoxDependent );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
    connect( pCheckBoxDependent, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotDependent(Qt::CheckState)) );
#else
    connect( pCheckBoxDependent, SIGNAL(stateChanged(int)), SLOT(slotDependent(int)) );
#endif
    // Cardinality
    QStringList stringlistCardinality  = pRelationship->getCardinality( nEOL ).split( ',' );
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

    pLineEditRole = new QLineEdit( pRelationship->getRole( nEOL ), this );
    pLayout->addRow( tr("Role:"), pLineEditRole );
    connect( pLineEditRole, SIGNAL(textChanged(const QString &)), SLOT(slotRole(const QString &)) );

    pLayoutTop->addWidget( pFrame );

    connect( pRelationship, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMRelationshipDetailsWidget::slotDominant( int n )
{
    pRelationship->setDominant( nEOL, n );
}

void LMRelationshipDetailsWidget::slotIdentified( int n )
{
    pRelationship->setIdentified( nEOL, n );
}

void LMRelationshipDetailsWidget::slotDependent( int n )
{
    pRelationship->setDependent( nEOL, n );
}

void LMRelationshipDetailsWidget::slotCardinalityMin( const QString &s )
{
    pRelationship->setCardinality( nEOL, s + "," + pComboBoxCardinalityMax->currentText() );
}

void LMRelationshipDetailsWidget::slotCardinalityMax( const QString &s )
{
    pRelationship->setCardinality( nEOL, pComboBoxCardinalityMin->currentText() + "," + s );
}

void LMRelationshipDetailsWidget::slotRole( const QString &s )
{
    pRelationship->setRole( nEOL, s );
}

void LMRelationshipDetailsWidget::slotModified()
{
    pCheckBoxDominant->setChecked( pRelationship->isDominant( nEOL ) );
    pCheckBoxIdentified->setChecked( pRelationship->isIdentified( nEOL ) );
    pCheckBoxDependent->setChecked( pRelationship->isDependent( nEOL ) );
    QStringList stringlistCardinality  = pRelationship->getCardinality( nEOL ).split( ',' );
    if ( stringlistCardinality.count() > 0 ) pComboBoxCardinalityMin->setCurrentText( stringlistCardinality.at(0) );
    if ( stringlistCardinality.count() > 1 ) pComboBoxCardinalityMax->setCurrentText( stringlistCardinality.at(1) );
    pLineEditRole->setText( pRelationship->getRole( nEOL ) );
}

LMRelationshipWidget::LMRelationshipWidget( LMRelationship *p, Qt::Orientation nOrientation, QWidget *pParent )
    : QWidget( pParent )
{
    pRelationship = p;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    {
        QHBoxLayout *pLayout = new QHBoxLayout();

        pGenerate = new QCheckBox( this );
        pGenerate->setChecked( pRelationship->isGenerate() );
#if (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0))
        connect( pGenerate, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotGenerate(Qt::CheckState)) );
#else
        connect( pGenerate, SIGNAL(stateChanged(int)), SLOT(slotGenerate(int)) );
#endif
        pLayout->addWidget( new QLabel( tr("Generate:"), this ) );
        pLayout->addWidget( pGenerate );
        pLayout->addStretch( 10 );

        pLayoutTop->addLayout( pLayout );
    }

    {
        QLayout *pLayout;
        if ( nOrientation == Qt::Horizontal ) pLayout = new QHBoxLayout();
        else pLayout = new QVBoxLayout();

        pBegin  = new LMRelationshipDetailsWidget( pRelationship, CBD::EOLBegin, this );
        pEnd    = new LMRelationshipDetailsWidget( pRelationship, CBD::EOLEnd, this );

        pLayout->addWidget( pBegin );
        pLayout->addWidget( pEnd );

        pLayoutTop->addLayout( pLayout );
    }

    connect( pRelationship, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void LMRelationshipWidget::slotGenerate( int n )
{
    if ( n ) pRelationship->setGenerate();
    else pRelationship->setGenerate( false );
}

void LMRelationshipWidget::slotModified()
{
    pGenerate->setChecked( pRelationship->isGenerate() );
}


