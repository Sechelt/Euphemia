#include "LibInfo.h"
#include "AWFactoryWidget.h"

#include "AWObjectsWidget.h"

AWFactoryWidget::AWFactoryWidget( AWObject *pObject, QWidget *pWidgetParent )
    : QTabWidget( pWidgetParent )
{
    // create tab for each class using friendly name
    // the tab lists existing objects of that class and allows;
    //      - new ones to be created
    //      - deletion 
    //      - editing
    QStringList listClassesFriendly = pObject->getClasses( true );
    QString     stringClassFriendly;

    foreach( stringClassFriendly, listClassesFriendly )
    {
        AWObjectsWidget *pWidget = new AWObjectsWidget( pObject, this, pObject->getClass( stringClassFriendly ) ); // we want actual class name here
        pWidget->setNew( bAllowCreate );
        pWidget->setEdit( bAllowEdit );
        pWidget->setDelete( bAllowDelete );

        addTab( pWidget, stringClassFriendly ); // friendly class name here makes sense
    }
}

AWFactoryWidget::~AWFactoryWidget()
{
}

void AWFactoryWidget::setAllowCreate( bool b )
{
    if ( b == bAllowCreate ) return;
    bAllowCreate = b;
    for ( int n = 0; n < count(); n++ )
    {
        AWObjectsWidget *p = (AWObjectsWidget*)widget( n );
        p->setNew( bAllowCreate );
    }
}

void AWFactoryWidget::setAllowDelete( bool b )
{
    if ( b == bAllowDelete ) return;
    bAllowDelete = b;
    for ( int n = 0; n < count(); n++ )
    {
        AWObjectsWidget *p = (AWObjectsWidget*)widget( n );
        p->setDelete( bAllowDelete );
    }
}

void AWFactoryWidget::setAllowEdit( bool b )
{
    if ( b == bAllowEdit ) return;
    bAllowEdit = b;
    for ( int n = 0; n < count(); n++ )
    {
        AWObjectsWidget *p = (AWObjectsWidget*)widget( n );
        p->setEdit( bAllowEdit );
    }
}

