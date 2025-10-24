#include "LibInfo.h"
#include "DStencilsWidget.h"

#include "DStencils.h"
#include "DStencil.h"
#include "DStencilItem.h"

//
//
//
DStencilItemWidget::DStencilItemWidget( QWidget *pWidget, DStencilItem *p )
: WIconWidget( p->getName(), p->getIcon().scaled( QSize( 48, 48 ) ), pWidget )
{
    pStencilItem = p;
    connect( pStencilItem, SIGNAL(signalModified()), SLOT(slotModified()) );
}

DStencilItemWidget::~DStencilItemWidget()
{
}

void DStencilItemWidget::slotModified()
{
    pLabelIcon->setPixmap( pStencilItem->getIcon().scaled( QSize( 48, 48 ) ) );
    pLabelText->setText( pStencilItem->getName() );
}

void DStencilItemWidget::mousePressEvent( QMouseEvent *pMouseEvent )
{
    if ( pMouseEvent->button() == Qt::LeftButton )
    {
        // initiate drag...
        QDrag *     drag    = new QDrag( this );
        QMimeData * pMime   = new QMimeData();

        pMime->setText( QString( pStencilItem->metaObject()->className() ) + "," + QString::number( (qulonglong)pStencilItem ) );        

        drag->setPixmap( pStencilItem->getIcon() );
        drag->setMimeData( pMime );
        drag->exec( Qt::CopyAction, Qt::CopyAction );

        // delete drag;
        // delete pMime;

        return;
    }

    QWidget::mousePressEvent( pMouseEvent );
}


//
//
//
DStencilWidget::DStencilWidget( QWidget *pWidget, DStencil *p )
: QWidget( pWidget )
{
    Q_ASSERT( p );

    // setStyleSheet( "background-color: lightblue" );
    pLayout = new WIconLayout( this );

    pStencil = nullptr;
    doLoad( p );
}

DStencilWidget::~DStencilWidget()
{
    // Qt clean up does what we need
}

void DStencilWidget::slotChildCreated( ADObject *p )
{
    if ( !p->inherits( "DStencilItem" ) ) return;
    DStencilItemWidget *pStencilItemWidget = new DStencilItemWidget( this, (DStencilItem*)p );
    listStencilItemWidgets.append( pStencilItemWidget );
    pLayout->addWidget( pStencilItemWidget );
}

void DStencilWidget::slotChildDeleted( ADObject *p )
{
    for ( int n = 0; n < listStencilItemWidgets.count(); n++ )
    {
        if ( listStencilItemWidgets.at( n )->getStencilItem() == p )
        {
            delete listStencilItemWidgets.takeAt( n );
            return;
        }
    }
}

void DStencilWidget::doLoad( DStencil *p )
{
    if ( pStencil ) doClear();
    if ( !p ) return;
    pStencil = p;

    QList<ADObject*> listStencilItems = pStencil->getObjects( QString(), QString(), "DStencilItem" );
    ADObject *pObject;
    foreach( pObject, listStencilItems )
    {
        DStencilItemWidget *pStencilItemWidget = new DStencilItemWidget( this, (DStencilItem*)pObject );
        listStencilItemWidgets.append( pStencilItemWidget );
        pLayout->addWidget( pStencilItemWidget );
    }

    // we want to know when a DStencilItem is created/delete
    connect( pStencil, SIGNAL(signalCreated(ADObject*)), this, SLOT(slotChildCreated(ADObject*)) );
    connect( pStencil, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotChildDeleted(ADObject*)) );
}

void DStencilWidget::doClear()
{
    if ( !pStencil ) return;
    qDeleteAll( listStencilItemWidgets );
    pStencil = nullptr;
}

//
//
//
DStencilsWidget::DStencilsWidget( QWidget *pWidget, DStencils *p )
: WStackedWidget( pWidget )
{
    pStencils = nullptr;
    if ( p ) doLoad( p );
}

DStencilsWidget::~DStencilsWidget()
{
    // Qt clean up does what we need
}

void DStencilsWidget::slotCreated( ADObject *p )
{
    if ( !p->inherits( "DStencils" ) ) return;
    doLoad( (DStencils*)p );
}

void DStencilsWidget::slotDeleted( ADObject *p )
{
    if ( p != pStencils ) return;
    doClear();
}

void DStencilsWidget::slotChildCreated( ADObject *p )
{
    if ( !p->inherits( "DStencil" ) ) return;
    DStencil*pStencil = (DStencil*)p;
    addWidget( pStencil->getName(), new DStencilWidget( this, pStencil ) );
}

void DStencilsWidget::slotChildDeleted( ADObject *p )
{
    for ( int n = 0; n < count(); n++ )
    {
        DStencilWidget *pWidget = (DStencilWidget*)widget( n );
        if ( pWidget->getStencil() == p )
        { 
            removeWidget( pWidget );
            delete pWidget;
            return;
        }
    }
}

/*!
 * \brief Update stencil name when DStencil is modified.
 *  
 * We maintain a combobox with the stencil names. 
 * Update name when stencil is modified.
 *  
 * \author pharvey (5/7/20)
 * 
 * \param p 
 */
void DStencilsWidget::slotModified( ADObject *p )
{
    Q_ASSERT( p );
    if ( !p->inherits( "DStencil" ) ) return;

    for ( int n = 0; n < count(); n++ )
    {
        DStencilWidget *pStencilWidget = (DStencilWidget*)widget( n );
        if ( pStencilWidget->getStencil() == p )
        { 
            setName( n, p->getName() );
            return;
        }
    }
}

void DStencilsWidget::doLoad( DStencils *p )
{
    if ( pStencils ) doClear();
    if ( !p ) return;
    pStencils = p;

    QList<ADObject*> listStencils = pStencils->getObjects( QString(), QString(), "DStencil" );
    ADObject *pObject;
    foreach( pObject, listStencils )
    {
        DStencil *pStencil = (DStencil*)pObject;
        addWidget( pStencil->getName(), new DStencilWidget( this, pStencil ) );
    }

    // we want to know when a DStencil is created/delete
    connect( pStencils, SIGNAL(signalCreated(ADObject*)), this, SLOT(slotChildCreated(ADObject*)) );
    connect( pStencils, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotChildDeleted(ADObject*)) );
    connect( pStencils, SIGNAL(signalModified(ADObject*)), this, SLOT(slotModified(ADObject*)) );
}

void DStencilsWidget::doClear()
{
    if ( !pStencils ) return;
    while ( count() )
    {
        QWidget *p = widget( 0 );
        Q_ASSERT( p );
        removeWidget( p );
        delete p;
    }
    pStencils = nullptr;
}




