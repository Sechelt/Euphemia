#include "LibInfo.h"
#include "FRDialogButtonBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRDialogButtonBox::FRDialogButtonBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRDialogButtonBox::~FRDialogButtonBox()
{
}

void FRDialogButtonBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QDialogButtonBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRDialogButtonBox::getIcon()
{
    return QPixmap( ":FR/DialogButtonBox" );
}

