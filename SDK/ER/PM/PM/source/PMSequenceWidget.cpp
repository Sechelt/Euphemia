#include "LibInfo.h"
#include "PMSequenceWidget.h"

PMSequenceWidget::PMSequenceWidget( PMSequence *pSequence, QWidget *pParent )
: QWidget( pParent )
{
    setObjectName( metaObject()->className() );
    this->pSequence = pSequence;

    QFormLayout *pLayout  = new QFormLayout( this );

    QLineEdit *pInc = new QLineEdit( pSequence->getInc(), this );
    connect( pInc, SIGNAL(textEdited(const QString &)), SLOT(slotIncChanged(const QString &)) );
    pLayout->addRow( tr( "Increment" ), pInc );

    QLineEdit *pMin = new QLineEdit( pSequence->getMin(), this );
    connect( pMin, SIGNAL(textEdited(const QString &)), SLOT(slotMinChanged(const QString &)) );
    pLayout->addRow( tr( "Minimum" ), pMin );

    QLineEdit *pMax = new QLineEdit( pSequence->getMax(), this );
    connect( pMax, SIGNAL(textEdited(const QString &)), SLOT(slotMaxChanged(const QString &)) );
    pLayout->addRow( tr( "Maximum" ), pMax );

    QLineEdit *pStart = new QLineEdit( pSequence->getStart(), this );
    connect( pStart, SIGNAL(textEdited(const QString &)), SLOT(slotStartChanged(const QString &)) );
    pLayout->addRow( tr( "Start" ), pStart );

    QLineEdit *pCache = new QLineEdit( pSequence->getCache(), this );
    connect( pCache, SIGNAL(textEdited(const QString &)), SLOT(slotCacheChanged(const QString &)) );
    pLayout->addRow( tr( "Cache" ), pCache );

    QLineEdit *pCycle = new QLineEdit( pSequence->getCycle(), this );
    connect( pCycle, SIGNAL(textEdited(const QString &)), SLOT(slotCycleChanged(const QString &)) );
    pLayout->addRow( tr( "Cycle" ), pCycle );
}

void PMSequenceWidget::slotIncChanged( const QString &s )
{
    pSequence->setInc( s );
}

void PMSequenceWidget::slotMinChanged( const QString &s )
{
    pSequence->setMin( s );
}

void PMSequenceWidget::slotMaxChanged( const QString &s )
{
    pSequence->setMax( s );
}

void PMSequenceWidget::slotStartChanged( const QString &s )
{
    pSequence->setStart( s );
}

void PMSequenceWidget::slotCacheChanged( const QString &s )
{
    pSequence->setCache( s );
}

void PMSequenceWidget::slotCycleChanged( const QString &s )
{
    pSequence->setCycle( s );
}


