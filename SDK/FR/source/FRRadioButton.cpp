#include "LibInfo.h"
#include "FRRadioButton.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRRadioButton::FRRadioButton( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRRadioButton::~FRRadioButton()
{
}

void FRRadioButton::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QRadioButton();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRRadioButton::getIcon()
{
    return QPixmap( ":FR/RadioButton" );
}

