#include "LibInfo.h"
#include "FRPlainTextEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRPlainTextEdit::FRPlainTextEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRPlainTextEdit::~FRPlainTextEdit()
{
}

void FRPlainTextEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QPlainTextEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRPlainTextEdit::getIcon()
{
    return QPixmap( ":FR/PlainTextEdit" );
}

