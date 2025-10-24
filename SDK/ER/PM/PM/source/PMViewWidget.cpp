#include "LibInfo.h"
#include "PMViewWidget.h"

PMViewWidget::PMViewWidget( PMView *pView, QWidget *pParent )
: QWidget( pParent )
{
    setObjectName( metaObject()->className() );
    this->pView = pView;

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    pEditor = new DATASqlWidget( this );
    new WSQL92Highlighter( pEditor->document() );
    pEditor->setText( pView->getDef() );
    pLayout->addWidget( pEditor );
    connect( pEditor, SIGNAL(signalModified()), this, SLOT(slotTextChanged()));
}

void PMViewWidget::slotTextChanged()
{
    pView->setDef( pEditor->getText() );
}


