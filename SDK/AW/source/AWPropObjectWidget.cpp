#include "LibInfo.h"
#include "AWPropObjectWidget.h"

AWPropObjectWidget::AWPropObjectWidget( AWObject *pObject, QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pObject = pObject;

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );
    {
        QGridLayout *pLayoutGrid = new QGridLayout;

        pLabelOID = new QLabel( QString( "%1" ).arg( pObject->getOID() ), this );
        pLineEditName = new QLineEdit( pObject->getName(), this );
        pLineEditCode = new QLineEdit( pObject->getCode(), this );

        // pTextEditComment = new QTextEdit( pObject->getComment(), this );
        pTextEditComment = new QTextEdit( this );
        QStringList l = pObject->getComment().split( '\n' );
        foreach( QString s, l )
        {
            pTextEditComment->append( s );
        }

        pLayoutGrid->addWidget( new QLabel( tr("OID:"), this ), 0, 0 );
        pLayoutGrid->addWidget( pLabelOID, 1, 0 );
        pLayoutGrid->addWidget( new QLabel( tr("Name:"), this ), 0, 1 );
        pLayoutGrid->addWidget( pLineEditName, 1, 1 );
        pLayoutGrid->addWidget( new QLabel( tr("Code:"), this ), 0, 2 );
        pLayoutGrid->addWidget( pLineEditCode, 1, 2 );

        connect( pLineEditName, SIGNAL(editingFinished()), SLOT(slotName()) );
        connect( pLineEditCode, SIGNAL(editingFinished()), SLOT(slotCode()) );
        connect( pTextEditComment, SIGNAL(textChanged()), SLOT(slotComment()) );
        connect( pObject, SIGNAL(signalModified()), SLOT(slotRefresh()) );

        pLayoutTop->addLayout( pLayoutGrid );
    }
    pLayoutTop->addWidget( new QLabel( tr("Comment"), this ) );
    pLayoutTop->addWidget( pTextEditComment );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}

void AWPropObjectWidget::slotRefresh()
{
    // no need to refresh if we caused the change
    if ( pLineEditName->hasFocus() ) return;
    if ( pLineEditCode->hasFocus() ) return;
    if ( pTextEditComment->hasFocus() ) return;

    // refresh
    pLabelOID->setText( QString::number( pObject->getOID() ) );
    pLineEditName->setText( pObject->getName() );
    pLineEditCode->setText( pObject->getCode() );
    pTextEditComment->setText( pObject->getComment() );
}


