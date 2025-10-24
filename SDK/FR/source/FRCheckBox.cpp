#include "LibInfo.h"
#include "FRCheckBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRCheckBox::FRCheckBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRCheckBox::~FRCheckBox()
{
}

void FRCheckBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QCheckBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRCheckBox::getIcon()
{
    return QPixmap( ":FR/CheckBox" );
}



