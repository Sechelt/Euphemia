#include "LibInfo.h"
#include "FRLabel.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRLabel::FRLabel( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRLabel::~FRLabel()
{
}

void FRLabel::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QLabel();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRLabel::getIcon()
{
    return QPixmap( ":FR/Label" );
}

