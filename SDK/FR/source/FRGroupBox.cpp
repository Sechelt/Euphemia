#include "LibInfo.h"
#include "FRGroupBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRGroupBox::FRGroupBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRGroupBox::~FRGroupBox()
{
}

void FRGroupBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QGroupBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRGroupBox::getIcon()
{
    return QPixmap( ":FR/GroupBox" );
}

