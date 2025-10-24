#include "LibInfo.h"
#include "FRTreeWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTreeWidget::FRTreeWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTreeWidget::~FRTreeWidget()
{
}

void FRTreeWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTreeWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTreeWidget::getIcon()
{
    return QPixmap( ":FR/TreeWidget" );
}


