#include "LibInfo.h"
#include "FRToolButton.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRToolButton::FRToolButton( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRToolButton::~FRToolButton()
{
}

void FRToolButton::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QToolButton();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRToolButton::getIcon()
{
    return QPixmap( ":FR/ToolButton" );
}

