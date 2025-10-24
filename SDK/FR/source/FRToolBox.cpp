#include "LibInfo.h"
#include "FRToolBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRToolBox::FRToolBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRToolBox::~FRToolBox()
{
}

void FRToolBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QToolBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRToolBox::getIcon()
{
    return QPixmap( ":FR/ToolBox" );
}

