#include "LibInfo.h"
#include "FRListWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRListWidget::FRListWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRListWidget::~FRListWidget()
{
}

void FRListWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QListWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRListWidget::getIcon()
{
    return QPixmap( ":FR/ListWidget" );
}

