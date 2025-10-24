#include "LibInfo.h"
#include "FRStackedWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRStackedWidget::FRStackedWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRStackedWidget::~FRStackedWidget()
{
}

void FRStackedWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QStackedWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRStackedWidget::getIcon()
{
    return QPixmap( ":FR/StackedWidget" );
}

