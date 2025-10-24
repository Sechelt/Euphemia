#include "LibInfo.h"
#include "FRPushButton.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRPushButton::FRPushButton( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRPushButton::~FRPushButton()
{
}

void FRPushButton::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QPushButton();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRPushButton::getIcon()
{
    return QPixmap( ":FR/PushButton" );
}

