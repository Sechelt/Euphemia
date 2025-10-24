#include "LibInfo.h"
#include "FRVerticalScrollBar.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRVerticalScrollBar::FRVerticalScrollBar( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRVerticalScrollBar::~FRVerticalScrollBar()
{
}

void FRVerticalScrollBar::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QScrollBar( Qt::Vertical );
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRVerticalScrollBar::getIcon()
{
    return QPixmap( ":FR/VerticalScrollBar" );
}

