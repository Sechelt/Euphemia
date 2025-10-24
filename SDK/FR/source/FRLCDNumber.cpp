#include "LibInfo.h"
#include "FRLCDNumber.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRLCDNumber::FRLCDNumber( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRLCDNumber::~FRLCDNumber()
{
}

void FRLCDNumber::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QLCDNumber();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRLCDNumber::getIcon()
{
    return QPixmap( ":FR/LCDNumber" );
}

