#include "LibInfo.h"
#include "FRComboBox.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRComboBox::FRComboBox( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRComboBox::~FRComboBox()
{
}

void FRComboBox::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QComboBox();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRComboBox::getIcon()
{
    return QPixmap( ":FR/ComboBox" );
}

