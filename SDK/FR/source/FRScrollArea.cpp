#include "LibInfo.h"
#include "FRScrollArea.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRScrollArea::FRScrollArea( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRScrollArea::~FRScrollArea()
{
}

void FRScrollArea::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QScrollArea();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRScrollArea::getIcon()
{
    return QPixmap( ":FR/ScrollArea" );
}

