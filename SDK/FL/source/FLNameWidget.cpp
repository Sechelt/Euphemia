#include "LibInfo.h"
#include "FLNameWidget.h"

FLNameWidget::FLNameWidget( FLRectangle *pRectangle, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pRectangle = pRectangle;

    QVBoxLayout *pLayout00 = new QVBoxLayout( this );

    {
        QGridLayout *pLayout01 = new QGridLayout;

        pOID = new QLabel( QString( "%1" ).arg( pRectangle->getOID() ), this );
        pName = new QLineEdit( pRectangle->getName(), this );
        pCode = new QLineEdit( pRectangle->getCode(), this );

        pLayout01->addWidget( new QLabel( tr("OID:"), this ), 0, 0 );
        pLayout01->addWidget( pOID, 0, 1 );
        pLayout01->addWidget( new QLabel( tr("Name:"), this ), 0, 2 );
        pLayout01->addWidget( pName, 0, 3 );
        pLayout01->addWidget( new QLabel( tr("Code:"), this ), 0, 4 );
        pLayout01->addWidget( pCode, 0, 5 );

        pLayout00->addLayout( pLayout01 );
    }

    pComment = new QTextEdit( pRectangle->getComment(), this );
    pLayout00->addWidget( new QLabel( tr("Comment"), this ) );
    pLayout00->addWidget( pComment );

    pMacro = new QTextEdit( pRectangle->getMacro(), this );
    pLayout00->addWidget( new QLabel( tr("Macro"), this ) );
    pLayout00->addWidget( pMacro );

    connect( pName, SIGNAL(editingFinished()), SLOT(slotName()) );
    connect( pCode, SIGNAL(editingFinished()), SLOT(slotCode()) );
    connect( pComment, SIGNAL(textChanged()), SLOT(slotComment()) );
    connect( pMacro, SIGNAL(textChanged()), SLOT(slotMacro()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}




















