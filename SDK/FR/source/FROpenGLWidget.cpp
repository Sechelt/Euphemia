#include "LibInfo.h"
#include "FROpenGLWidget.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FROpenGLWidget::FROpenGLWidget( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FROpenGLWidget::~FROpenGLWidget()
{
}

void FROpenGLWidget::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QOpenGLWidget();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FROpenGLWidget::getIcon()
{
    return QPixmap( ":FR/OpenGLWidget" );
}

