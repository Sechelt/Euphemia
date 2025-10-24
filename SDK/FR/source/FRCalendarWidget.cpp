#include "LibInfo.h"
#include "FRCalendarWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRCalendarWidget::FRCalendarWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRCalendarWidget::~FRCalendarWidget()
{
}

void FRCalendarWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 
    // Q_ASSERT( !p->inherits( "FRGraphicsProxyObject" ) );

    FRObject::setProxy( p );
    QWidget *pWidget = new QCalendarWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRCalendarWidget::getIcon()
{
    return QPixmap( ":FR/CalendarWidget" );
}


