#include "LibInfo.h"
#include "FRLabel.h"

#include <FRGraphicsProxyObject.h>

#include "FRForm.h"

FRLabel::FRLabel( FRForm *pForm, const QString &stringName )
    : FRObject( pForm, stringName )
{
}

FRLabel::~FRLabel()
{
}

void FRLabel::setName( const QString &s )                   
{
    DRectangle::setName( s );  

    FRGraphicsProxyObject *pProxy = (FRGraphicsProxyObject*)getProxy();  
    Q_ASSERT( pProxy );

    QLabel *pLabel = (QLabel*)pProxy->widget();
    Q_ASSERT( pLabel );

    pLabel->setText( s );
}                                                              
                                                               
void FRLabel::setProxy( DGraphicsProxyItem *p )
{
    Q_ASSERT( p ); 

    FRObject::setProxy( p );
    QWidget *pWidget = new QLabel();
    pWidget->setEnabled( false );
    ((FRGraphicsProxyObject*)p)->setWidget( pWidget );
    rect = p->getItem()->boundingRect();
    doSinksSync();
}

QPixmap FRLabel::getIcon()
{
    return QPixmap( ":FR/Label" );
}

AWPropWidget *FRLabel::getPropWidget( QWidget *pWidgetParent )
{
    AWPropWidget *pPropWidget = DRectangle::getPropWidget( pWidgetParent );

    pPropWidget->removeWidget( tr("Font") );                                                                                 
    pPropWidget->removeWidget( tr("Pen") );                                                                                 
    pPropWidget->removeWidget( tr("Brush") );                                                                                 
    pPropWidget->removeWidget( tr("Text") );                                                                                 

    return pPropWidget;

}

