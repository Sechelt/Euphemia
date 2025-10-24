#include "LibInfo.h"
#include "FRSpinBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRSpinBox::FRSpinBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRSpinBox::~FRSpinBox()
{
}

void FRSpinBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QSpinBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRSpinBox::getIcon()
{
    return QPixmap( ":FR/SpinBox" );
}

