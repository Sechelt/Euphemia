#include "LibInfo.h"
#include "FRTabWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTabWidget::FRTabWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTabWidget::~FRTabWidget()
{
}

void FRTabWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTabWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTabWidget::getIcon()
{
    return QPixmap( ":FR/TabWidget" );
}

