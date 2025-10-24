#include "LibInfo.h"
#include "PMDDLWidget.h"

PMDDLWidget::PMDDLWidget( PMDDLManager *pDDLManager, const QString &stringCreate, const QString &stringDrop, QWidget *pParent )
: QTabWidget( pParent )
{
    setObjectName( metaObject()->className() );
    this->pDDLManager = pDDLManager;
    stringCreateAuto    = stringCreate;
    stringDropAuto      = stringDrop;

    // CREATE
    {
        QWidget *pWidget = new QWidget( this );

        QVBoxLayout *pLayout = new QVBoxLayout( pWidget );

        QHBoxLayout *pLayout01 = new QHBoxLayout();
        pManualCreate = new QCheckBox( tr("Manual"), this );
        pManualCreate->setChecked( pDDLManager->isCreateManual() );
        pLayout01->addWidget( pManualCreate );
        connect( pManualCreate, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotManualCreate(Qt::CheckState)) );

        pIncludeCreate = new QCheckBox( tr("Include"), this );
        pIncludeCreate->setChecked( pDDLManager->isIncludeCreate() );
        pLayout01->addWidget( pIncludeCreate );
        connect( pIncludeCreate, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotIncludeCreate(Qt::CheckState)) );
        pLayout01->addStretch( 10 );

        pLayout->addLayout( pLayout01 );

        pTextEditCreate = new DATASqlWidget( this );
        new WSQL92Highlighter( pTextEditCreate->document() );
        if ( pDDLManager->isCreateManual() ) pTextEditCreate->setText( pDDLManager->getCreateManual() );
        else pTextEditCreate->setText( stringCreateAuto );
        pLayout->addWidget( pTextEditCreate );
        connect( pTextEditCreate, SIGNAL(textChanged()), this, SLOT(slotTextChangedCreate()));

        addTab( pWidget, tr( "Create" ) );
    }
    // DROP
    {
        QWidget *pWidget = new QWidget( this );

        QVBoxLayout *pLayout = new QVBoxLayout( pWidget );

        QHBoxLayout *pLayout01 = new QHBoxLayout();
        pManualDrop = new QCheckBox( tr("Manual"), this );
        pManualDrop->setChecked( pDDLManager->isDropManual() );
        pLayout01->addWidget( pManualDrop );
        connect( pManualDrop, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotManualDrop(Qt::CheckState)) );

        pIncludeDrop = new QCheckBox( tr("Include"), this );
        pIncludeDrop->setChecked( pDDLManager->isIncludeDrop() );
        pLayout01->addWidget( pIncludeDrop );
        connect( pIncludeDrop, SIGNAL(checkStateChanged(Qt::CheckState)), SLOT(slotIncludeDrop(Qt::CheckState)) );
        pLayout01->addStretch( 10 );

        pLayout->addLayout( pLayout01 );

        pTextEditDrop = new DATASqlWidget( this );
        new WSQL92Highlighter( pTextEditDrop->document() );
        if ( pDDLManager->isDropManual() ) pTextEditDrop->setText( pDDLManager->getDropManual() );
        else pTextEditDrop->setText( stringDropAuto );
        pLayout->addWidget( pTextEditDrop );
        connect( pTextEditDrop, SIGNAL(textChanged()), this, SLOT(slotTextChangedDrop()));

        addTab( pWidget, tr( "Drop" ) );
    }
}

void PMDDLWidget::slotManualCreate( Qt::CheckState b )
{
    if ( b ) pTextEditCreate->setText( pDDLManager->getCreateManual() );
    else pTextEditCreate->setText( stringCreateAuto );
    pDDLManager->setCreateManual( b );
}

void PMDDLWidget::slotManualDrop( Qt::CheckState b )
{
    if ( b ) pTextEditDrop->setText( pDDLManager->getDropManual() );
    else pTextEditDrop->setText( stringDropAuto );
    pDDLManager->setDropManual( b );
}

void PMDDLWidget::slotIncludeCreate( Qt::CheckState b )
{
    pDDLManager->setIncludeCreate( b );
}

void PMDDLWidget::slotIncludeDrop( Qt::CheckState b )
{
    pDDLManager->setIncludeDrop( b );
}

void PMDDLWidget::slotTextChangedCreate()
{
    if ( !pManualCreate->isChecked() ) return;
    QString stringManual = pTextEditCreate->getText();
    pDDLManager->setCreateManual( stringManual );
}

void PMDDLWidget::slotTextChangedDrop()
{
    if ( !pManualDrop->isChecked() ) return;
    QString stringManual = pTextEditDrop->getText();
    pDDLManager->setDropManual( stringManual );
}


