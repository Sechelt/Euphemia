#include "LibInfo.h"
#include "DATAWQueryTable.h"

#include <DATAConnection.h>
#include <DATAStatement.h>

#include "DATAWQueryColumn.h"
#include "DATAWQueryPrimaryKeys.h"
#include "DATAWQueryForeignKeys.h"
#include "DATAWQuerySpecialColumns.h"
#include "DATAWQueryIndexs.h"

#include "DATAWDataWidget.h"
#include "DATAWTableWidget.h"

DATAWQueryTable::DATAWQueryTable( ADObject *pParent, DATAConnection *pConnection, const QString &stringCatalog, const QString &stringSchema, const QString &stringType )
    : DATAWQuery( pParent )
{
    bCanCreateEditor = true;

    this->pConnection   = pConnection;
    this->stringCatalog = stringCatalog;
    this->stringSchema  = stringSchema;
    this->stringType    = stringType;

    mapMeta.insert( "DATAWQueryColumn", ADMeta( QIcon(), "DATAWQueryColumn", "Column" ) );
    mapMeta.insert( "DATAWQueryPrimaryKeys", ADMeta( QIcon(), "DATAWQueryPrimaryKeys", "PrimaryKeys" ) );
    mapMeta.insert( "DATAWQueryForeignKeys", ADMeta( QIcon(), "DATAWQueryForeignKeys", "ForeignKeys" ) );
    mapMeta.insert( "DATAWQuerySpecialColumns", ADMeta( QIcon(), "DATAWQuerySpecialColumns", "SpecialColumns" ) );
    mapMeta.insert( "DATAWQueryIndexs", ADMeta( QIcon(), "DATAWQueryIndexs", "Indexs" ) );
}

DATAWQueryTable::~DATAWQueryTable()
{
}

QPixmap DATAWQueryTable::getIcon()
{
    return QPixmap( ":DATAW/Table" );
}

QMenu *DATAWQueryTable::getContextMenu( QWidget *pParent )
{
    QMenu *pMenu = DATAWQuery::getContextMenu( pParent );

    // DATAWConnection *p = (DATAWConnection*)getParent( "DATAWConnection" );
    // if ( p ) pMenu->addAction( getIcon(), tr( "Data" ), this, SLOT(slotData() ) );

    if ( isEditor() )
    {
        pMenu->addAction( QPixmap(), tr("Close Editor"), this, SLOT(slotCloseEditor()) );
    }
    else
    {
        pMenu->addAction( QPixmap(), tr( "Open Editor" ), this, SLOT( slotOpenEditor() ) );
    }
    pMenu->addAction( QPixmap( ":AW/Properties" ), tr("Properties..."), this, SLOT(slotObjectDialog()) );



    return pMenu;
}

ADObject *DATAWQueryTable::getObject( const QString &s, ADObject * )
{
    AWObject *p = nullptr;

    // translate any alias
    QString stringClass = getClass( s );

    if ( stringClass == "DATAWQueryColumn" )        
        p = new DATAWQueryColumn( this );           
    else if ( stringClass == "DATAWQueryPrimaryKeys" )        
        p = new DATAWQueryPrimaryKeys( this, pConnection, stringCatalog, stringSchema, getName() );           
    else if ( stringClass == "DATAWQueryForeignKeys" )        
        p = new DATAWQueryForeignKeys( this, pConnection, stringCatalog, stringSchema, getName() );           
    else if ( stringClass == "DATAWQuerySpecialColumns" )        
        p = new DATAWQuerySpecialColumns( this, pConnection, stringCatalog, stringSchema, getName() );           
    else if ( stringClass == "DATAWQueryIndexs" )        
        p = new DATAWQueryIndexs( this, pConnection, stringCatalog, stringSchema, getName() );           

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

void DATAWQueryTable::doGoTo( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    ADObject::doGoTo( listObjectPathItems, nDepth );
}

void DATAWQueryTable::doGoToView( ADObjectPathList listObjectPathItems, uint nDepth )
{
    if ( !isEditor() )
    { 
        if ( !slotOpenEditor() ) return;
    }
    getEditorWidget()->doGoTo();

    if ( int(nDepth) >= listObjectPathItems.count() )
    {
        slotObjectDialog();
        return;
    }

    //
    ADObject::doGoToView( listObjectPathItems, nDepth );
}

void DATAWQueryTable::doGoTo()
{
    // recurse our way to root
    if ( parent() )
    { 
        AWObject *p = (AWObject*)parent();
        p->doGoTo();
    }

    // do what we can on the way back down
    if ( isEditor() ) getEditorWidget()->doGoTo();
}

void DATAWQueryTable::doQuery()
{
    if ( bQueried || bLoadingChildren ) return;
    if ( !pConnection->isConnected() ) return;
    bLoadingChildren = true;

    // query for columns
    {
        DATAStatement *pStatement = pConnection->getColumns( stringCatalog, stringSchema.isEmpty() ? QString() : stringSchema, getName() );
        if ( !pStatement ) 
        {
            bLoadingChildren = false;
            return;
        }

        SQLRETURN n = pStatement->doFetch();
        while ( SQL_SUCCEEDED( n ) )
        {
            QString stringColumn    = pStatement->getData( 4 ).toString();
            QString stringDataType  = pStatement->getData( 6 ).toString();

            ADObject *p = getObject( "DATAWQueryColumn" );
            p->setName( stringColumn );
            p->setComment( stringDataType );
            p->setModified( false );

            // carry on
            n = pStatement->doFetch();
        }

        delete pStatement;
    }

    // other
    if ( stringType == "TABLE" || stringType == "SYSTEM TABLE" )                        
    {                                                                                   
        getObject( "DATAWQueryPrimaryKeys" );
        getObject( "DATAWQueryForeignKeys" );
        getObject( "DATAWQueryIndexs" );
        getObject( "DATAWQuerySpecialColumns" );
    }

    bQueried = true;
    bLoadingChildren = false;
}

bool DATAWQueryTable::slotOpenEditor()
{
    if ( pEditorWidget ) return true;

    // create view
    DATAWTableWidget *pEditor;
    pEditorWidget = pEditor = new DATAWTableWidget( this, getEditorWidgetParent(), pConnection, stringCatalog, stringSchema, getName() );

    //
    emit signalEditorOpened( pEditorWidget );
    emit signalEditorOpened( this, pEditorWidget );

    return true;
}


