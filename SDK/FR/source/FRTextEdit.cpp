#include "LibInfo.h"
#include "FRTextEdit.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTextEdit::FRTextEdit( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTextEdit::~FRTextEdit()
{
}

void FRTextEdit::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTextEdit();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTextEdit::getIcon()
{
    return QPixmap( ":FR/TextEdit" );
}

