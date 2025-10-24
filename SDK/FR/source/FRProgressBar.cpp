#include "LibInfo.h"
#include "FRProgressBar.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRProgressBar::FRProgressBar( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRProgressBar::~FRProgressBar()
{
}

void FRProgressBar::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QProgressBar();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRProgressBar::getIcon()
{
    return QPixmap( ":FR/ProgressBar" );
}

