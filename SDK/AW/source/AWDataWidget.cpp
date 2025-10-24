#include "LibInfo.h"
#include "AWDataWidget.h"
#include "AWObject.h"

AWDataWidget::AWDataWidget( AWObject *pObject, QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    this->pObject = pObject;
}

int AWDataWidget::indexOf( const QString &stringInherits )
{
    for ( int n = 0; n < count(); n++ )
    {
        if ( widget( n )->inherits( stringInherits.toLatin1() ) ) return n;
    }
    return -1;
}

QWidget *AWDataWidget::getByClass( const QString &stringInherits )
{
    int n = indexOf( stringInherits );
    if ( n >= 0 ) return widget( n );
    return nullptr;
}

void AWDataWidget::removeByClass( const QString &stringInherits )
{
    int n = indexOf( stringInherits );
    if ( n >= 0 )
    { 
        QWidget *pWidget = widget( n );
        removeTab( n );
        delete pWidget;
    }
}

void AWDataWidget::removeByName( const QString &stringName )
{
    for ( int n = 0; n < count(); n++ )
    {
        if ( tabText( n ) == stringName )
        {
            QWidget *pWidget = widget( n );
            removeTab( n );
            delete pWidget;
            return;
        }
    }
}

void AWDataWidget::removeByWidget( QWidget *pWidget )
{
    int n = QTabWidget::indexOf( pWidget );
    if ( n < 0 ) return;
    removeTab( n );
    delete pWidget;
}

