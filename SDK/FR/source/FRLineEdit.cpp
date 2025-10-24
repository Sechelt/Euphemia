#include "LibInfo.h"
#include "FRLineEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRLineEdit::FRLineEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRLineEdit::~FRLineEdit()
{
}

void FRLineEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QLineEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRLineEdit::getIcon()
{
    return QPixmap( ":FR/LineEdit" );
}

