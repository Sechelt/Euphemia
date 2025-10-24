#include "LibInfo.h"
#include "AWObjectNameWidget.h"

AWObjectNameWidget::AWObjectNameWidget( AWObject *pObject, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
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
        {
            pTextEditComment = new QTextEdit( this );
            QStringList l = pObject->getComment().split( '\n' );
            foreach( QString s, l )
            {
                pTextEditComment->append( s );
            }
        }

        pLayoutGrid->addWidget( new QLabel( tr("OID:"), this ), 0, 0 );
        pLayoutGrid->addWidget( pLabelOID, 1, 0 );
        pLayoutGrid->addWidget( new QLabel( tr("Name:"), this ), 0, 1 );
        pLayoutGrid->addWidget( pLineEditName, 1, 1 );
        pLayoutGrid->addWidget( new QLabel( tr("Code:"), this ), 0, 2 );
        pLayoutGrid->addWidget( pLineEditCode, 1, 2 );

        pLayoutTop->addLayout( pLayoutGrid );
    }
    pLayoutTop->addWidget( new QLabel( tr("Comment"), this ) );
    pLayoutTop->addWidget( pTextEditComment );


    connect( pLineEditName, SIGNAL(editingFinished()), SLOT(slotName()) );
    connect( pLineEditCode, SIGNAL(editingFinished()), SLOT(slotCode()) );
    connect( pTextEditComment, SIGNAL(textChanged()), SLOT(slotComment()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}






















