#include "LibInfo.h"
#include "FRDoubleSpinBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRDoubleSpinBox::FRDoubleSpinBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRDoubleSpinBox::~FRDoubleSpinBox()
{
}

void FRDoubleSpinBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QDoubleSpinBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRDoubleSpinBox::getIcon()
{
    return QPixmap( ":FR/DoubleSpinBox" );
}

