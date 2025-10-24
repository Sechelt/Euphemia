#include "LibInfo.h"
#include "PMProcedureWidget.h"

PMProcedureWidget::PMProcedureWidget( PMProcedure *pProcedure, QWidget *pParent )
: QWidget( pParent )
{
    setObjectName( metaObject()->className() );
    this->pProcedure = pProcedure;

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    pEditor = new DATASqlWidget( this );
    new WSQL92Highlighter( pEditor->document() );
    pEditor->setText( pProcedure->getDef() );
    pLayout->addWidget( pEditor );
    connect( pEditor, SIGNAL(signalModified()), this, SLOT(slotTextChanged()));
}

void PMProcedureWidget::slotTextChanged()
{
    pProcedure->setDef( pEditor->getText() );
}


