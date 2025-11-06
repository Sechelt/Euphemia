#include "LibInfo.h"
#include "DATAWConnection.h"

#include <DATAStatement.h>

#include <AWDataDialog.h>

// data widgets
#include <AWObjectNameWidget.h>

#include <DATAProfile.h>
#include <DATAProfileWidget.h>

#include "DATAWConnectWizard.h"
#include "DATAWConnectDialog.h"
#include "DATAWQueryCatalogs.h"
#include "DATAWQueryCatalog.h"
#include "DATAWSql.h"

// not sure we need to manage state but could be used to change icon in UI
#define DATAW_STATE_DISCONNECTED 0
#define DATAW_STATE_CONNECTED 1

DATAWConnection::DATAWConnection( AWObject *pParent, const QString &stringName )
    : AWObject( pParent, stringName )
{
    //
    pSystem         = new DATASystem();
    connect( pSystem, SIGNAL(signalMessage(DATAMessage *)), SLOT(slotMessage(DATAMessage *)) );
    connect( pSystem, SIGNAL(signalDiagnostic(DATADiagnostic *)), SLOT(slotDiagnostic(DATADiagnostic *)) );
    pSystem->doAlloc();

    pEnvironment    = new DATAEnvironment( pSystem );
    pEnvironment->doAlloc();
    pEnvironment->setAttrODBCVersion( DATAEnvironment::OVOdbc3 );

    pConnection     = new DATAConnection( pEnvironment );
    pConnection->doAlloc();
    connect( pConnection, SIGNAL(signalConnected()), SLOT(slotConnected()) );
    connect( pConnection, SIGNAL(signalDisconnected()), SLOT(slotDisconnected()) );

    // this needs to be based upon the objects supported by the data source
    mapMeta.clear();
    mapMeta.insert( "DATAWQueryCatalogs", ADMeta( QIcon(), "DATAWQueryCatalogs", "Catalogs" ) );
    mapMeta.insert( "DATAWQueryCatalog", ADMeta( QIcon(), "DATAWQueryCatalog", "Catalog" ) );
    mapMeta.insert( "DATAWSql", ADMeta( QIcon(":W/SQL16x16"), "DATAWSql", "SQL Editor" ) );
}

DATAWConnection::~DATAWConnection()
{
    delete pConnection;
    delete pEnvironment;
    delete pSystem;
}

void DATAWConnection::setKeyValues( QMap<QString,QString> m )
{
    if ( m == mapKeyValues ) return;
    mapKeyValues = m;
    setModified();
}

QPixmap DATAWConnection::getIcon()
{
    if ( isConnected() ) return QPixmap( ":W/Connected16x16" );

    return QPixmap( ":W/Disconnected16x16" );
}

ADObject *DATAWConnection::getObject( const QString &s, ADObject * )
{
    // \note in this case we need to be dynamic because different data sources support different objects

    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryCatalogs" )    
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DATAWQueryCatalogs" ).count() )
        {
            doMessageBox( "INFO", tr("New Query Catalogs"), tr("Already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DATAWQueryCatalogs( this, pConnection );           
    }
    else if ( stringClass == "DATAWQueryCatalog" )    
    {
        // caller should handle a nullptr return
        if ( getObjects( QString(), QString(), "DATAWQueryCatalog" ).count() )
        {
            doMessageBox( "INFO", tr("New Query Catalog"), tr("Already exists and the maximum is one.") );
            return nullptr; 
        }
        p = new DATAWQueryCatalog( this, pConnection );           
        // IF SQL_CATALOG_TERM == 'database' THEN SQL_ATTR_CURRENT_CATALOG
        // p->setName( (stringCatalog.isEmpty()?"CATALOG":stringCatalog) );
    }
    else if ( stringClass == "DATAWSql" )        
        p = new DATAWSql( this );           

    if ( !p )
    {
        doMessageBox( "WARNING", "Create Object...", "Unknown class requested " + s );
        return nullptr;
    }

    // announce the birth of a new child
    emit signalCreated( p );    
    emit signalCreated( this, p );

    return p;
}

AWDataWidget *DATAWConnection::getObjectWidget( QWidget *pWidgetParent )
{
    // caller must delete and this is done nicely if used in a AWDataDialog with proper parent
    AWDataWidget *p = new AWDataWidget( this, pWidgetParent );
    p->setWindowIcon( getIcon() );
    p->setWindowTitle( tr("Connection...") );
    
    p->addTab( new AWObjectNameWidget( this, p ), tr("Name") );
    p->addTab( new DATAWConnectWidget( this, p ), tr("Connection") );
    p->addTab( new DATAProfileWidget( pConnection->getProfile(), p ), tr("Data Source Profile") );

    p->setCurrentIndex( 0 );

    return p;
}

QMenu *DATAWConnection::getContextMenu( QWidget *pParent )
{
    QMenu *     pMenu   = AWObject::getContextMenu( pParent );
    QAction *   pAction;

    // add our stuff at the top of the menu = reverse order
    pAction = pMenu->insertSeparator( pMenu->actions().first() );

    if ( isConnected() && getObjects( QString(), QString(), "DATAWQueryCatalog" ).isEmpty() )
    {
        pAction = new QAction( QIcon( ":DATAW/CatalogClosed" ), tr( "Add Catalog Browser" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotNewCatalogBrowser()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    if ( isConnected() && getObjects( QString(), QString(), "DATAWQueryCatalogs" ).isEmpty() )
    {
        pAction = new QAction( QIcon( ":DATAW/CatalogClosed" ), tr( "Add Catalogs Browser" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotNewCatalogsBrowser()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }

    pAction = new QAction( QIcon( ":W/SQL16x16" ), tr( "New SQL Editor" ), this );
    connect( pAction, SIGNAL(triggered()), SLOT(slotNewDATAWSql()) );
    pMenu->insertAction( pMenu->actions().first(), pAction );

    if ( isConnected() )
    {
        pAction = new QAction( QIcon( ":W/Disconnected16x16" ), tr( "Disconnect" ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotDisconnect()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );
    }
    else
    {
        pAction = new QAction( QIcon( ":W/Connected16x16" ), tr( "Connect Wizard..." ), this );
        connect( pAction, SIGNAL(triggered()), SLOT(slotConnectWizard()) );
        pMenu->insertAction( pMenu->actions().first(), pAction );

        if ( mapKeyValues.count() )
        {
            pAction = new QAction( QIcon( ":W/Connected16x16" ), tr( "Connect..." ), this );
            connect( pAction, SIGNAL(triggered()), SLOT(slotConnect()) );
            pMenu->insertAction( pMenu->actions().first(), pAction );
        }
    }

    return pMenu;
}

QDomElement DATAWConnection::doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent )
{
    // save common and children...
    QDomElement domElemThis = AWObject::doSave( pdomDoc, pdomElemParent );

    domElemThis.appendChild( doSaveConnectString( pdomDoc ) );

    return domElemThis;
}

bool DATAWConnection::doLoad( QDomElement *pdomElemThis )
{
    // load common and children... 
    AWObject::doLoad( pdomElemThis );

    // load additional nodes...
    QDomElement         domElem;
    QDomNode            domNode;
    domNode = pdomElemThis->firstChild();
    while( !domNode.isNull() ) 
    {
        domElem = domNode.toElement();
        if( domElem.isNull() ) 
        {
            domNode = domNode.nextSibling();
            continue;
        }

        if ( domElem.tagName() == "ConnectString" ) doLoadConnectString( &domElem );

        domNode = domNode.nextSibling();
    }

    return true;
}

void DATAWConnection::slotConnectWizard()
{
    if ( isConnected() ) return;

    DATAWConnectWizard dialog( getConnection(), getEditorWidgetParent() );
    if ( dialog.exec() == QDialog::Accepted )
    {
        mapKeyValues = dialog.getKeyValues();
        setModified();
    }
}

void DATAWConnection::slotConnect()
{
    if ( isConnected() ) return;

    // first time?  lets build a connect string from scratch
    if ( mapKeyValues.isEmpty() ) return slotConnectWizard();

    // we only prompt if we have a value for substitution
    bool bPrompt = false;
    {
        QMapIterator<QString,QString> i(mapKeyValues);
        while ( i.hasNext() ) 
        {
            i.next();
            if ( i.value() == "?" || i.value() == "??" )
            {
                bPrompt = true;
                break;
            }
        }
    }

    if ( bPrompt )
    {
        // we need to get some info from the User so...
        DATAWConnectDialog dialog( getConnection(), mapKeyValues, getEditorWidgetParent() );
        dialog.exec();
        return;
    }

    // hopefully all needed info is in mapKeyValues so...
    if ( mapKeyValues.count() == 3 && mapKeyValues.contains( "DSN" ) && mapKeyValues.contains( "UserName" ) && mapKeyValues.contains( "Authentication" ) )
        getConnection()->doConnect( mapKeyValues.value( "DSN" ), mapKeyValues.value( "UserName" ), mapKeyValues.value( "Authentication" ) );
    else
        getConnection()->doDriverConnect( nullptr, mapKeyValues );
    if ( !isConnected() ) 
    {
        doMessageBox( "ERROR", tr("Connect"), tr("Failed to connect.\nLets try connecting using the wizard.") );
        return slotConnectWizard();
    }
}

void DATAWConnection::slotDisconnect()
{
    if ( !isConnected() ) return;
    getConnection()->doDisconnect();
}

void DATAWConnection::slotDelete()
{
    if ( !getObjects( QString(), QString(), "DATAWSql" ).isEmpty() )
    {
        QMessageBox msgBox;
        msgBox.setText( tr("Contains SQL.") );
        msgBox.setInformativeText( tr("All SQL within will be lost!\nDo you really want to delete this connection?") );
        msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton( QMessageBox::No );
        if ( msgBox.exec() != QMessageBox::Yes ) return;
    }
    AWObject::slotDelete();
}

QDomElement DATAWConnection::doSaveConnectString( QDomDocument *pdomDoc )
{
    QDomElement domElem = pdomDoc->createElement( "ConnectString" );

    QMapIterator<QString, QString> i(mapKeyValues);
    while ( i.hasNext() ) 
    {
        i.next();
        domElem.setAttribute( i.key(), i.value() );
    }

    return domElem;
}

void DATAWConnection::doLoadConnectString( QDomElement *pdomElem )
{
    mapKeyValues.clear();

    auto var = pdomElem->attributes();
    for ( int i=0; i < var.count(); ++i)
    {
        mapKeyValues[var.item(i).nodeName()] = var.item(i).nodeValue();
    }
}

void DATAWConnection::doDumpKeyValues( const QMap<QString,QString> &mapKeyValues )
{
    QMapIterator<QString,QString> i(mapKeyValues);
    while ( i.hasNext() ) 
    {
        i.next();
        printf( "[%s][%s][%d] key[%s] value[%s]\n", __FILE__, __FUNCTION__, __LINE__, i.key().toUtf8().constData(), i.value().toUtf8().constData() );
    }
}

void DATAWConnection::slotConnected()
{
    setState( DATAW_STATE_CONNECTED );
    emit signalConnected();
}

void DATAWConnection::slotDisconnected()
{
    setState( DATAW_STATE_DISCONNECTED );
    emit signalDisconnected();
}

void DATAWConnection::slotNewDATAWSql()
{
    DATAWSql *p = (DATAWSql*)getObject( "DATAWSql" );
    Q_ASSERT(p);
    p->slotOpenEditor();
    p->slotObjectDialog( getEditorWidgetParent() );
}

void DATAWConnection::slotNewCatalogBrowser()
{
    if ( !getObjects( QString(), QString(), "DATAWQueryCatalog" ).isEmpty() ) return;
    DATAWQueryCatalog *p = (DATAWQueryCatalog*)getObject( "DATAWQueryCatalog" );
    Q_ASSERT( p );

// is a catalog a database?
//    DATAInfo *pSQL_CATALOG_TERM = (getConnection()->getProfile()->mapInfo)["SQL_CATALOG_TERM"]; Q_ASSERT( pSQL_CATALOG_TERM );
//    if ( pSQL_CATALOG_TERM->vValue.toString() == "database" )

    p->setName( pConnection->getAttrCurrentCatalog() );
    setModified( false );
}

void DATAWConnection::slotNewCatalogsBrowser()
{
    if ( !getObjects( QString(), QString(), "DATAWQueryCatalogs" ).isEmpty() ) return;
    getObject( "DATAWQueryCatalogs" );
    setModified( false );
}

void DATAWConnection::slotMessage( DATAMessage *p )
{
    // package message and send it out via ADObject
    ADDataEventGeneral Event( p );
    emit signalOutput( &Event );
}

void DATAWConnection::slotDiagnostic( DATADiagnostic *p )
{
    // package message and send it out via ADObject
    ADDataEventDiagnostic Event( p );
    emit signalOutput( &Event );
}

//
// DATAWConnectWidget
//
DATAWConnectWidget::DATAWConnectWidget( DATAWConnection *pConnection, QWidget *pWidgetParent )
    : QWidget( pWidgetParent )
{
    QMap<QString,QString> mapKeyValues = pConnection->getKeyValues();

    QVBoxLayout *pLayoutTop = new QVBoxLayout( this );

    // build a form
    {
        QFormLayout *pLayout = new QFormLayout;
        QMapIterator<QString,QString> i(mapKeyValues);
        while ( i.hasNext() ) 
        {
            i.next();
            QLineEdit *p = new QLineEdit( this );
            p->setText( i.value() );
            p->setReadOnly( true );
            pLayout->addRow( new QLabel( i.key(), this ), p );
        }
        pLayoutTop->addLayout( pLayout, 10 );
    }
}

