#include "LibInfo.h"
#include "ORGObjectProjectWidget.h"

ORGObjectProjectWidget::ORGObjectProjectWidget( ORGPerson *pPerson, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT BEGIN: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif

    this->pPerson = pPerson;

    QLabel *pLabel;

    QVBoxLayout *pLayout00 = new QVBoxLayout( this );

    QHBoxLayout *pLayout01 = new QHBoxLayout;
    pLabel = new QLabel( tr("Project Role:"), this );
    pProjectRole = new QLineEdit( pPerson->getProjectRole(), this );
    pLayout01->addWidget( pLabel );
    pLayout01->addWidget( pProjectRole );
    pLayout00->addLayout( pLayout01 );

    pLabel = new QLabel( tr("Events:"), this );
    pEvents = new QTableWidget( this );
    pLayout00->addWidget( pLabel );
    pLayout00->addWidget( pEvents );

    connect( pProjectRole, SIGNAL(editingFinished()), SLOT(slotProjectRole()) );

#ifdef DEBUG_DESTRUCTORS
    qDebug( "[PAH][%s][%d] CONSTRUCT END: %s %p", __FILE__, __LINE__, metaObject()->className(), this );
#endif
}


















