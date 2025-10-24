#include "LibInfo.h"
#include "FRFrame.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRFrame::FRFrame( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRFrame::~FRFrame()
{
}

void FRFrame::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QFrame();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRFrame::getIcon()
{
    return QPixmap( ":FR/Frame" );
}

