#include "LibInfo.h"
#include "FRHorizontalSlider.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRHorizontalSlider::FRHorizontalSlider( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRHorizontalSlider::~FRHorizontalSlider()
{
}

void FRHorizontalSlider::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QSlider( Qt::Horizontal );
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRHorizontalSlider::getIcon()
{
    return QPixmap( ":FR/HorizontalSlider" );
}

