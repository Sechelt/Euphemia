#include "LibInfo.h"
#include "FRTextBrowser.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRTextBrowser::FRTextBrowser( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRTextBrowser::~FRTextBrowser()
{
}

void FRTextBrowser::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QTextBrowser();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRTextBrowser::getIcon()
{
    return QPixmap( ":FR/TextBrowser" );
}

