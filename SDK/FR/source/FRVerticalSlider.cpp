#include "LibInfo.h"
#include "FRVerticalSlider.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRVerticalSlider::FRVerticalSlider( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRVerticalSlider::~FRVerticalSlider()
{
}

void FRVerticalSlider::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QSlider( Qt::Vertical );
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRVerticalSlider::getIcon()
{
    return QPixmap( ":FR/VerticalSlider" );
}

