#include "LibInfo.h"
#include "FRDateTimeEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRDateTimeEdit::FRDateTimeEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRDateTimeEdit::~FRDateTimeEdit()
{
}

void FRDateTimeEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QDateTimeEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRDateTimeEdit::getIcon()
{
    return QPixmap( ":FR/DateTimeEdit" );
}

