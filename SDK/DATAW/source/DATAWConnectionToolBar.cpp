#include "LibInfo.h"
#include "DATAWConnectionToolBar.h"

#include <DATAConnection.h>
#include <DATAStatement.h>
#include <DATASqlWidget.h>

#include <AWPalWidget.h>
#include <AWPanWidget.h>

#include "DATAWConnection.h"
#include "DATAWSql.h"

//
//
//
DATAWConnectionComboBox::DATAWConnectionComboBox( QWidget *pParent )
    : QComboBox( pParent )
{
}

void DATAWConnectionComboBox::showPopup()
{
    // Ensure that we have the current name for the connections in the list.
    // This addresses the case where a new connection is created (and added to this list) during a load. In this case
    // we do not get notified when the name is set.
    for ( int n = 1; n < count(); n++ )
    {
        DATAWConnection *p = (DATAWConnection*)itemData( n ).toULongLong();
        setItemText( n, p->getName() );
    }
    QComboBox::showPopup();
}

//
//
//
DATAWConnectionToolBar::DATAWConnectionToolBar( QWidget *pParent )
: QToolBar( pParent )
{
    setObjectName( tr("Data Connection") );
    setWindowTitle( tr("Data Connection") );

    isActive = true;
    pCurrent = nullptr;

    pNewConnection = addAction( QIcon(":DATAW/NewConnection"), tr("new connection") );
    pNewConnection->setToolTip( tr("new connection") );
    connect( pNewConnection, SIGNAL(triggered()), this, SIGNAL(signalNewConnection()) );

    pConnections = new DATAWConnectionComboBox( this );
    pConnections->addItem( tr("no connection"), 0 );   // we set to 0 if current is deleted
    pConnections->setToolTip( tr("connection") );
    addWidget( pConnections );
    connect( pConnections, SIGNAL(currentIndexChanged(int)), SLOT(slotConnection(int)) );

    pToggleConnect = addAction( QIcon(":DATAW/Disconnected"), tr("connect/disconnect") );
    pToggleConnect->setCheckable( true );
    pToggleConnect->setToolTip( tr("connect/disconnect") );
    connect( pToggleConnect, SIGNAL(toggled(bool)), SLOT(slotToggleConnect(bool)) );
}

DATAWConnectionToolBar::~DATAWConnectionToolBar()
{
}

void DATAWConnectionToolBar::setConnection( DATAWConnection *p )
{
    // not found?
    int nIndex = pConnections->findData( (qulonglong)p );
    if ( nIndex < 0 ) return;
    // same as current?
    if ( nIndex == pConnections->currentIndex() ) return;
    // unwatch
    setCurrent(); 
    // set new current
    pConnections->setCurrentIndex( nIndex );
    // watch
    setCurrent( p );
    // let folks know
    emit signalConnection( p );
}

void DATAWConnectionToolBar::setActive( bool b )
{
    if ( b == isActive ) return;

    isActive = b;

    if ( isActive )
    { 
        pNewConnection->setEnabled( true );
        pConnections->setEnabled( true );
        pToggleConnect->setEnabled( true );
    }
    else
    { 
        pNewConnection->setEnabled( false );
        pConnections->setEnabled( false );
        pToggleConnect->setEnabled( false );
    }
}

DATAWConnection *DATAWConnectionToolBar::getConnection()
{
    return pCurrent;
}

void DATAWConnectionToolBar::doAdd( DATAWConnection *p )
{
    // must have a value - can not add another none/0 item
    Q_ASSERT( p );
    // is it in the combo box?
    if ( pConnections->findData( (qulonglong)p ) > 0 ) return;
    // add - we assume current does NOT change
    pConnections->addItem( p->getName(), (qulonglong)p );
    connect( p, SIGNAL(signalModified(ADObject*)), this, SLOT(slotUpdate(ADObject*)) );
}

void DATAWConnectionToolBar::doRemove( DATAWConnection *p )
{
    // must have a value - can not remove none/0 item
    Q_ASSERT( p ); 
    // is it in the combo box?
    int n = pConnections->findData( (qulonglong)p );
    if ( n <= 0 ) return;
    // is it the current connection?
    if ( n == pConnections->currentIndex() ) 
    {
        // unwatch
        setCurrent();
        //
        pConnections->setCurrentIndex( 0 );
        pConnections->removeItem( n );
        // let folks know
        emit signalConnection( pCurrent );
    }
    else
    {
        // its not the current one so just remove it
        pConnections->removeItem( n );
    }
    disconnect( p, SIGNAL(signalModified(ADObject*)), this, SLOT(slotUpdate(ADObject*)) );
}

void DATAWConnectionToolBar::doClear()
{
    // unwatch
    setCurrent();
    // revert to just one item none/0
    pConnections->clear();
    pConnections->addItem( tr("no connection"), 0 );   // we set to 0 if current is deleted
    // let others know
    emit signalConnection( nullptr );
}

void DATAWConnectionToolBar::slotConnection( int nIndex )
{
    Q_UNUSED( nIndex );

    // unwatch 
    setCurrent();
    // watch
    setCurrent( (DATAWConnection*)pConnections->itemData( nIndex ).toULongLong() );
    // let others know
    emit signalConnection( pCurrent );
}

void DATAWConnectionToolBar::slotToggleConnect( bool b )
{
    DATAWConnection *p = getConnection();
    if ( !p ) return;
    // make request to connect/disconnect
    if ( b )
        p->slotConnect();
    else
        p->slotDisconnect();
}

void DATAWConnectionToolBar::slotConnected()
{
    pToggleConnect->setChecked( true );
}

void DATAWConnectionToolBar::slotDisconnected()
{
    pToggleConnect->setChecked( false );
}

void DATAWConnectionToolBar::slotRemove( QObject *p )
{
    Q_ASSERT( p );
    Q_ASSERT( p->inherits("DATAWConnection") );
    int n = pConnections->findData( (qulonglong)p );
    if ( n <= 0 ) return;
    doRemove( (DATAWConnection*)p );
}

void DATAWConnectionToolBar::slotUpdate( ADObject *p )
{
    Q_ASSERT( p );

    // can get here because child of connection modified - not what we care about
    if ( !p->inherits("DATAWConnection") ) return;

    // a connection was modified so update the name to ensure we have any changes
    int n = pConnections->findData( (qulonglong)p );
    if ( n <= 0 ) return;
    pConnections->setItemText( n, p->getName() );
}

void DATAWConnectionToolBar::setCurrent( DATAWConnection *p )
{
    pCurrent = p;
    if ( pCurrent )
    {
        pToggleConnect->setChecked( pCurrent->isConnected() );
        connect( pCurrent, SIGNAL(signalConnected()), this, SLOT(slotConnected()) );
        connect( pCurrent, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()) );
    }
}

void DATAWConnectionToolBar::setCurrent()
{
    if ( !pCurrent ) return;
    disconnect( pCurrent, SIGNAL(signalConnected()), this, SLOT(slotConnected()) );
    disconnect( pCurrent, SIGNAL(signalDisconnected()), this, SLOT(slotDisconnected()) );
    pCurrent = nullptr;
    pToggleConnect->setChecked( false );
}


