#include "LibInfo.h"
#include "DVectorWidget.h"

DVectorWidget::DVectorWidget( DVector *pVector, QWidget *pParent )
    : WSvgButton( pVector->getVector(), pParent )
{
    this->pVector = pVector;
    connect( this, SIGNAL(signalChanged(const QByteArray &)), pVector, SLOT(slotVector(const QByteArray &)) );
    connect( pVector, SIGNAL(signalModified()), SLOT(slotModified()) );
}

void DVectorWidget::slotModified()
{
    setVector( pVector->getVector() );
}























