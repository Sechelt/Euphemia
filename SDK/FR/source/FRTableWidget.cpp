#include "LibInfo.h"
#include "FRTableWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTableWidget::FRTableWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTableWidget::~FRTableWidget()
{
}

void FRTableWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTableWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTableWidget::getIcon()
{
    return QPixmap( ":FR/TableWidget" );
}

