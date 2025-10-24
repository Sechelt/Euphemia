#include "LibInfo.h"
#include "FRDial.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRDial::FRDial( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRDial::~FRDial()
{
}

void FRDial::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QDial();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRDial::getIcon()
{
    return QPixmap( ":FR/Dial" );
}

