#include "LibInfo.h"
#include "FRTimeEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTimeEdit::FRTimeEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTimeEdit::~FRTimeEdit()
{
}

void FRTimeEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTimeEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTimeEdit::getIcon()
{
    return QPixmap( ":FR/TimeEdit" );
}

