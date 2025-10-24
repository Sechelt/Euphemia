#include "LibInfo.h"
#include "AWDataTable.h"
        
/*!
    Derived classes will want to;
    - add desired table columns 
    - disable any unwanted toolbar buttons
    - implement slotNew(), doRowGet() and doRowSet()
*/    
AWDataTable::AWDataTable( AWObject *pmodelParent, const QString &stringInherits, QWidget *pwidgetParent, bool bUses )
    : QMainWindow( pwidgetParent )
{

    // !!!! this was created before object factory !!!! 
    // \todo make this use the parents object factory, be more general purpose, to the point where we would only want to enable/disable certain toolbar buttons

    this->pmodelParent      = pmodelParent;         
    this->stringClass       = stringInherits;
    this->bUses             = bUses;
    bClassInherits          = true;

    doInit();
}

AWDataTable::AWDataTable( AWObject *pmodelParent, QWidget *pwidgetParent, const QString &stringClass, bool bUses )
    : QMainWindow( pwidgetParent )
{
    this->pmodelParent      = pmodelParent;         
    this->stringClass       = stringClass;
    this->bUses             = bUses;
    bClassInherits          = false;

    doInit();
}

AWDataTable::~AWDataTable()
{
    doStabalize();
    doSaveState();
}

void AWDataTable::setNew( bool b )
{
    if ( b )
        pActionNew->setVisible( true );
    else
        pActionNew->setVisible( false );
}

void AWDataTable::setEdit( bool b )
{
    if ( b )
        pActionEdit->setVisible( true );
    else
        pActionEdit->setVisible( false );
}

void AWDataTable::setDelete( bool b )
{
    if ( b )
        pActionDelete->setVisible( true );
    else
        pActionDelete->setVisible( false );
}

void AWDataTable::setUp( bool b )
{
    if ( b )
        pActionUp->setVisible( true );
    else
        pActionUp->setVisible( false );
}

void AWDataTable::setDown( bool b )
{
    if ( b )
        pActionDown->setVisible( true );
    else
        pActionDown->setVisible( false );
}

void AWDataTable::doStabalize()
{
    ptable->setCurrentCell( -1, -1 );
}

/*!
    Derived classes will certianly want to replace this.
*/    
void AWDataTable::slotNew()
{
}

void AWDataTable::slotEdit()
{
    int nRow = getRowNumNotStrict();

    if ( nRow < 0 ) return;

    AWObject *pmodel = getObject( nRow );
    if ( !pmodel ) return;

    pmodel->slotObjectDialog( this );
    
    doRowSet( nRow, pmodel );
}

void AWDataTable::slotDelete()
{
    int nRow = getRowNumStrict();

    if ( nRow < 0 )
        return;

    // make edit widget go away
    ptable->setCurrentCell( -1, -1 );

    AWObject *pmodel = getObject( nRow );
    if ( !pmodel )
        return;

    if ( bUses )
    {
        pmodelParent->doDisconnect( pmodel );
    }
    else
    {
        AWObject *pParent = (AWObject*)pmodel->parent();
        Q_ASSERT( pParent );
        pParent->slotDelete( pmodel );
    }

    doTableSet();
}

void AWDataTable::slotUp()
{
    if ( bUses ) return;

    int nRow = getRowNumNotStrict();
    if ( nRow < 1 ) return;

    // make edit widget go away
    ptable->setCurrentCell( -1, -1 );

    // get model on row
    AWObject *pmodel = getObject( nRow );
    if ( !pmodel ) return;

    // shift in list
    if ( bClassInherits )
        pmodel->doToPrev( QString(), QString(), stringClass );
    else
        pmodel->doToPrev( QString(), stringClass, QString() );

    // refresh display
    doTableSet();

    ptable->setCurrentCell( nRow - 1, 1 );

    pmodelParent->setModified();
}

void AWDataTable::slotDown()
{
    if ( bUses ) return;

    int nRow = getRowNumNotStrict();
    if ( nRow < 0 ) return;
    if ( nRow >= (ptable->rowCount()-1) ) return;

    // make edit widget go away
    ptable->setCurrentCell( -1, -1 );

    // get PMColumn
    AWObject *pmodel = getObject( nRow );
    if ( !pmodel ) return;

    // shift in list
    if ( bClassInherits )
        pmodel->doToNext( QString(), QString(), stringClass );
    else
        pmodel->doToNext( QString(), stringClass, QString() );

    // refresh display
    doTableSet();
    
    ptable->setCurrentCell( nRow + 1, 1 );

    pmodelParent->setModified();
}

void AWDataTable::slotValueChanged( int nRow, int nCol )
{
    Q_UNUSED( nCol );

    // get model
    AWObject *pObject = getObject( nRow );
    if ( !pObject ) return;

    // update model
    doRowGet( nRow, pObject );
}


void AWDataTable::doInit()
{
    // TABLE
    ptable = new QTableWidget( 1, 1, this );
    ptable->hideColumn( 0 );
    ptable->setHorizontalHeaderItem( 0, new QTableWidgetItem( tr("ID") ) );
    ptable->setSelectionBehavior( QAbstractItemView::SelectRows );
    ptable->setSelectionMode( QAbstractItemView::SingleSelection );    
    ptable->setRowCount( 0 );
    setCentralWidget( ptable );

    // TOOLBAR
    pActionNew = new QAction( QIcon( ":W/RowNew16x16" ), tr("New"), this );
    pActionEdit = new QAction( QIcon( ":W/RowEdit16x16" ), tr("Edit"), this );
    pActionUp = new QAction( QIcon( ":W/RowUp16x16" ), tr("MoveUp"), this );
    pActionDown = new QAction( QIcon( ":W/RowDown16x16" ), tr("MoveDown"), this );
    pActionDelete = new QAction( QIcon( ":W/RowDelete16x16" ), tr("Delete"), this );

    connect( pActionNew, SIGNAL(triggered()), this, SLOT(slotNew()) );
    connect( pActionEdit, SIGNAL(triggered()), this, SLOT(slotEdit()) );
    connect( pActionUp, SIGNAL(triggered()), this, SLOT(slotUp()) );
    connect( pActionDown, SIGNAL(triggered()), this, SLOT(slotDown()) );
    connect( pActionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()) ); 

    ptoolbar = addToolBar( tr("") );
    ptoolbar->addAction( pActionNew );
    ptoolbar->addAction( pActionEdit );
    ptoolbar->addAction( pActionUp );
    ptoolbar->addAction( pActionDown );
    ptoolbar->addSeparator();
    ptoolbar->addAction( pActionDelete );

    // hide all actions by default
    pActionNew->setVisible( false );
    pActionEdit->setVisible( false );
    pActionDelete->setVisible( false );
    pActionUp->setVisible( false );
    pActionDown->setVisible( false );

    // derived clas constructor should do the following...
    // doTableSet();
    // connect( ptable, SIGNAL(cellChanged(int,int)), SLOT(slotValueChanged(int,int)) );
}

/*!
    doTableSet
*/    
void AWDataTable::doTableSet()
{
    // Set most columns "stretchable". This means;
    // - columns are same width
    // - columns will be sized to be visible
    // - columns can not be manually resized
    // Leave the first couple of columns as stretchable.
    ptable->setRowCount( 0 );
    for(int i=2;i<ptable->columnCount();i++)
    {
        // ptable->setColumnStretchable(i, true);
    }

    if ( bUses )
    {
        QList<ADObjectReferenceKey> listKeys = pmodelParent->getUsesKeys();
        ADObjectReferenceKey Key;
        foreach( Key, listKeys )
        {
            if ( bClassInherits )
            { 
                if ( Key.pObject->inherits( stringClass.toLatin1().constData() ) ) doRowAppend( (AWObject*)Key.pObject );
            }
            else
            {
                if ( Key.pObject->metaObject()->className() == stringClass ) doRowAppend( (AWObject*)Key.pObject );
            }
        }
    }
    else
    {
        QList<ADObject *> listObjects;
        if ( bClassInherits )
            listObjects = pmodelParent->getObjects( QString(), QString(), stringClass );
        else
            listObjects = pmodelParent->getObjects( QString(), stringClass, QString() );

        ADObject *p;
        foreach( p, listObjects )
        {
            doRowAppend( (AWObject*)p );
        }
    }
}

void AWDataTable::doRowAppend( AWObject *pObject )
{
    // append a new row...
    ptable->setRowCount( ptable->rowCount() + 1 );
    int nRow = ptable->rowCount() - 1;
    // init empty columns...
    for ( int nCol = 0; nCol < ptable->columnCount(); nCol++ )
    {
        QTableWidgetItem *pItem = new QTableWidgetItem();
        // pItem->setFlags( Qt::ItemIsSelectable ); // turn off editable to force user to invoke dialog to edit - for now
        ptable->setItem( nRow, nCol, pItem );
    }
    // set cell values
    doRowSet( nRow, pObject );
}

/*!
    Derived classes will certianly want to replace this method. But
    do not forget the 1st column is reserved and should be set by
    simply calling AWDataTable::doRowSet().
*/    
void AWDataTable::doRowSet( int nRow, AWObject *pmodel )
{
    ptable->item( nRow, 0 )->setData( Qt::DisplayRole,  QString::number( (qulonglong)pmodel ) );
}

/*!
    Derived classes will certianly want to replace this method. But
    do not forget the 1st colun (column 0) is reserved.
*/    
void AWDataTable::doRowGet( int, AWObject * )
{
}

/*!
    getModel
    
    Get the AWObject which corresponds to nRow in QTable. This
    assumes that the 1st column in QTable is a pointer to a AWObject.
    
    Will try to verify the existence of the AWObject before returning
    it. Will return nullptr if something failed.
*/
AWObject *AWDataTable::getObject( int nRow )
{
    QTableWidgetItem *pItem = ptable->item( nRow, 0 );
    if ( !pItem ) return nullptr;

    qulonglong nID = pItem->data( Qt::DisplayRole ).toULongLong();
    if ( nID == 0 ) return nullptr;

    if ( bUses )
    {
        // is pointer in use?
        // if ( pmodelParent->listUses.indexOf( (AWObject*)nID ) < 0 ) return nullptr;
        // ok - its being used but lets check class
        ADObject *pObject = (AWObject*)nID;
        if ( bClassInherits )
        { 
            if ( pObject->inherits( stringClass.toLatin1().constData() ) ) return (AWObject*)pObject;
        }
        else
        {
            if ( pObject->metaObject()->className() == stringClass ) return (AWObject*)pObject;
        }
    }
    else
    {
        // is pointer in use?
        QList<ADObject*> listObjects = pmodelParent->getObjects();
        if ( listObjects.indexOf( (ADObject*)nID ) < 0 ) return nullptr;
        // ok - its being used but lets check class
        ADObject *pObject = (ADObject*)nID;
        if ( bClassInherits )
        { 
           if ( pObject->inherits( stringClass.toLatin1().constData() ) ) return (AWObject*)pObject;
        }
        else
        {
           if ( pObject->metaObject()->className() == stringClass ) return (AWObject*)pObject;
        }
    }

    return nullptr;
}

int AWDataTable::getRowNumStrict()
{
    return ptable->currentRow();
}

int AWDataTable::getRowNumNotStrict()
{
    return ptable->currentRow();
}

void AWDataTable::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < ptable->columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", ptable->columnWidth( n ) );
    }
    settings.endArray();
}

void AWDataTable::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        ptable->setColumnWidth( n, settings.value( "w", QString::number( ptable->columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

