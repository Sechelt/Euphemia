#include "LibInfo.h"
#include "FRDateEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRDateEdit::FRDateEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRDateEdit::~FRDateEdit()
{
}

void FRDateEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QDateEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRDateEdit::getIcon()
{
    return QPixmap( ":FR/DateEdit" );
}

