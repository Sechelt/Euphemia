#include "LibInfo.h"
#include "FRHorizontalScrollBar.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRHorizontalScrollBar::FRHorizontalScrollBar( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRHorizontalScrollBar::~FRHorizontalScrollBar()
{
}

void FRHorizontalScrollBar::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QScrollBar( Qt::Horizontal );
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRHorizontalScrollBar::getIcon()
{
    return QPixmap( ":FR/HorizontalScrollBar" );
}

