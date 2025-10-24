#include "LibInfo.h"
#include "ORGObjectEmploymentWidget.h"

ORGObjectEmploymentWidget::ORGObjectEmploymentWidget( ORGPerson *pPerson, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pPerson = pPerson;

    QVBoxLayout *pLayout00 = new QVBoxLayout( this );

    QHBoxLayout *pLayout01 = new QHBoxLayout;


    {
        QGridLayout *pLayout02 = new QGridLayout;

        pTitle      = new QLineEdit( pPerson->getTitle(), this );
        pRole       = new QLineEdit( pPerson->getRole(), this );
        pEMail      = new QLineEdit( pPerson->getEMail(), this );
        pPhone      = new QLineEdit( pPerson->getPhone(), this );

        pLayout02->addWidget( new QLabel( tr("Title:"), this ), 0, 0 );
        pLayout02->addWidget( pTitle, 0, 1 );
        pLayout02->addWidget( new QLabel( tr("Role:"), this ), 1, 0 );
        pLayout02->addWidget( pRole, 1, 1 );
        pLayout02->addWidget( new QLabel( tr("eMail:"), this ), 2, 0 );
        pLayout02->addWidget( pEMail, 2, 1 );
        pLayout02->addWidget( new QLabel( tr("Phone:"), this ), 3, 0 );
        pLayout02->addWidget( pPhone, 3, 1 );

        pLayout01->addLayout( pLayout02 );
    }

    pLayout00->addLayout( pLayout01 );

    pLocation   = new QTextEdit( pPerson->getLocation(), this );
    pLayout00->addWidget( new QLabel( tr("Location"), this ) );
    pLayout00->addWidget( pLocation );


    connect( pTitle, SIGNAL(editingFinished()), SLOT(slotTitle()) );
    connect( pRole, SIGNAL(editingFinished()), SLOT(slotRole()) );
    connect( pLocation, SIGNAL(textChanged()), SLOT(slotLocation()) );
    connect( pEMail, SIGNAL(editingFinished()), SLOT(slotEMail()) );
    connect( pPhone, SIGNAL(editingFinished()), SLOT(slotPhone()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}




















