#include "LibInfo.h"
#include "AWTreeWidget.h"

#include <ADObjectSelectionManager.h>

#include "AWFile.h"
#include "AWQuery.h"

/*!
 * \brief A tree widget for browsing AppData.
 * 
 * \author pharvey (11/30/19)
 * 
 * \param pParent 
 */
AWTreeWidget::AWTreeWidget( QWidget *pParent )
    : QTreeWidget( pParent )
{
    pRoot = nullptr;
    QStringList listHeaders;
    listHeaders << tr("Name") << tr("Value") << tr("Comment");
    setRootIsDecorated( true );
    setColumnCount( 3 );
    setHeaderLabels( listHeaders );
    setAllColumnsShowFocus( true );
    setSelectionMode( QAbstractItemView::SingleSelection );

    // double click invokes object editor or object properties
    // user must click indicator to expand/collapse
    setExpandsOnDoubleClick( false ); 

    // setDragDropMode( QAbstractItemView::DragOnly );
    setDragEnabled( true );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(slotContextMenu(const QPoint &)) );
    connect( this, SIGNAL(itemClicked(QTreeWidgetItem*,int)), SLOT(slotItemClicked(QTreeWidgetItem*,int)) );
    connect( this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)) );
    connect( this, SIGNAL(itemExpanded(QTreeWidgetItem*)), SLOT(slotItemExpanded(QTreeWidgetItem*)) );
    connect( this, SIGNAL(itemCollapsed(QTreeWidgetItem*)), SLOT(slotItemCollapsed(QTreeWidgetItem*)) );

    doLoadState();
}

AWTreeWidget::~AWTreeWidget()
{
    doSaveState();
}

void AWTreeWidget::setRoot( AWObject *pObject )
{
    if ( pRoot )
    {
        clear();
        pRoot = nullptr; 
    }

    if ( pObject )
    {
        pRoot = pObject;
        new AWTreeWidgetObject( pObject, this ); // navigate through the object hierarchy adding AWTreeWidgetItem for each
    }
}

void AWTreeWidget::startDrag( Qt::DropActions n )
{
    // we can only drag one item so take first
    QTreeWidgetItem *p = currentItem();
    if ( !p ) return;
    AWTreeWidgetItem *pItem = (AWTreeWidgetItem*)p;

    QMimeData *pMime = new QMimeData();
    AWObject *pObject = pItem->getObject();
    pMime->setText( QString( pObject->metaObject()->className() ) + "," + QString::number( (qulonglong)pObject ) );        

    QDrag *drag = new QDrag( this );
    drag->setPixmap( pObject->getIcon() );
    drag->setMimeData( pMime );
    drag->exec( n, Qt::CopyAction );

    // this prevents (most of) the selection confusion when a diagram drag/dropped
    setCurrentItem( p );
}

void AWTreeWidget::doSaveState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    settings.beginWriteArray( s );
    for ( int n = 0; n < columnCount(); n++ )
    {
        settings.setArrayIndex( n );
        settings.setValue( "w", columnWidth( n ) );
    }
    settings.endArray();
}

void AWTreeWidget::doLoadState()
{
    QString s = metaObject()->className();
    if ( !objectName().isEmpty() ) s = s + "/" + objectName();
    s += "/column";

    QSettings settings;
    int nSize = settings.beginReadArray( s );
    for ( int n = 0; n < nSize; n++ ) 
    {
        settings.setArrayIndex( n );
        setColumnWidth( n, settings.value( "w", QString::number( columnWidth( n ) ) ).toInt() );
    }
    settings.endArray();
}

// 
void AWTreeWidget::slotContextMenu( const QPoint &point )
{
    QTreeWidgetItem *plistviewitem = itemAt( point );
    if ( !plistviewitem )
        return;
 
    AWTreeWidgetItem *pcontroller = (AWTreeWidgetItem*)plistviewitem;
    pcontroller->doMenu( point );
}

void AWTreeWidget::slotItemClicked( QTreeWidgetItem *pItem, int nColumn )
{
    Q_UNUSED(nColumn);
    AWTreeWidgetItem *p = (AWTreeWidgetItem *)pItem;
    p->doClicked();
}

void AWTreeWidget::slotItemDoubleClicked( QTreeWidgetItem *pItem, int nColumn )
{
    Q_UNUSED(nColumn);
    AWTreeWidgetItem *p = (AWTreeWidgetItem *)pItem;
    p->doDoubleClicked();
}

void AWTreeWidget::slotItemExpanded( QTreeWidgetItem *pItem )
{
    AWTreeWidgetItem *p = (AWTreeWidgetItem *)pItem;
    p->doExpanded();
}

void AWTreeWidget::slotItemCollapsed( QTreeWidgetItem *pItem )
{
    AWTreeWidgetItem *p = (AWTreeWidgetItem *)pItem;
    p->doCollapsed();
}

/*! 
 * \class AWTreeWidgetItem 
 * \brief An AWTreeWidget item.
 * 
 * \author pharvey (11/30/19)
 * 
 * \param pObject 
 * \param pParent 
 */
AWTreeWidgetObject::AWTreeWidgetObject( AWObject *pObject, AWTreeWidget *pParent )
    : AWTreeWidgetItem( pParent )
{
    doInit( pObject );
}

AWTreeWidgetObject::AWTreeWidgetObject( AWObject *pObject, AWTreeWidgetItem *pParent )
    : AWTreeWidgetItem( pParent )
{
    doInit( pObject );
}

AWTreeWidgetObject::~AWTreeWidgetObject()
{
}

void AWTreeWidgetObject::doMenu( const QPoint &point )
{
    QMenu *pmenu = pObject->getContextMenu( treeWidget() );
    if ( !pmenu )
        return;
        
    pmenu->exec( treeWidget()->mapToGlobal( point ) );

    delete pmenu;
}

void AWTreeWidgetObject::doExpanded()
{
    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );

    if ( pObject->canLoadChildren() ) 
    {
        pObject->doLoadChildren();
    }
}

void AWTreeWidgetObject::doCollapsed()
{
    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
}

void AWTreeWidgetObject::doClicked()
{
    // select and/or set focus
    if ( pObject->isSelectable() )
    {
        ADObjectSelectionManager *p = pObject->getSelectionManager();
        if ( p )
        { 
            p->doSelectNone();
            p->setSelected( pObject );
        }
    }
    pObject->doGoTo();
}

void AWTreeWidgetObject::doDoubleClicked()
{
    if ( pObject->canCreateEditor() )
    {
        pObject->slotOpenEditor();
        return;
    }
    // all others - object dialog
    pObject->slotObjectDialog();
}

void AWTreeWidgetObject::slotChildCreated( ADObject *pObject )
{
    AWTreeWidgetObject *p = getNewChild( (AWObject*)pObject );
    setExpanded( false ); // much faster load from file when this is false
    setSelected( false );
    treeWidget()->setCurrentItem( p );
}

void AWTreeWidgetObject::slotChildDeleted( ADObject *p )
{
    AWObject *pObject = (AWObject*)p;

    if ( pObject->getFolder().isEmpty() )
    { 
        for ( int n = 0; n < childCount(); n++ )
        {
            AWTreeWidgetObject *pItem = (AWTreeWidgetObject *)child( n );
            if ( pObject == pItem->getObject() )
            { 
                removeChild( pItem );
                delete pItem;
                break;
            }
        }
    }
    else
    {
        AWTreeWidgetFolder *pFolder = getFolder( pObject );
        for ( int n = 0; n < pFolder->childCount(); n++ )
        {
            AWTreeWidgetObject *pItem = (AWTreeWidgetObject *)pFolder->child( n );
            if ( pObject == pItem->getObject() )
            { 
                pFolder->removeChild( pItem );
                delete pItem;
                break;
            }
        }

        if ( pFolder->childCount() < 1 )
        { 
            removeChild( pFolder );
            delete pFolder;
        }
    }
}

void AWTreeWidgetObject::slotSelected( bool b )
{
    Q_UNUSED(b);
    setText( 0, getNamePrefix() + pObject->getName() );
    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
    setText( 1, pObject->getCode() );
    setText( 2, pObject->getComment() );
}

void AWTreeWidgetObject::slotModified()
{
    if ( pObject->inherits( "AWFile" ) )
    {
        AWFile *pFile = (AWFile*)pObject;
        QFileInfo FileInfo( pFile->getFileName() ); 
        setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
        setText( 0, getNamePrefix() + FileInfo.baseName() );
        setText( 1, pFile->getCode() );
        setText( 2, pFile->getFileName() );
        return;
    }

    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
    setText( 0, getNamePrefix() + pObject->getName() );
    setText( 1, pObject->getCode() );
    setText( 2, pObject->getComment() );
}

void AWTreeWidgetObject::slotState()
{
    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
}

QString AWTreeWidgetObject::getNamePrefix()
{
    QString s;

    if ( pObject->isModified() ) s += "*";
    if ( pObject->isSelected() ) s += "+";
    if ( !s.isEmpty() ) s += " ";

    return s;
}

void AWTreeWidgetObject::doInit( AWObject *pObject )
{
    this->pObject = pObject;

    setFlags( flags() | Qt::ItemIsDragEnabled );

    // indicator is needed to do load ondemand in some cases
    if ( pObject->canLoadChildren() ) 
        setChildIndicatorPolicy( QTreeWidgetItem::ShowIndicator );
    else
        setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicatorWhenChildless );

    setIcon( 0, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ) );
    setText( 0, getNamePrefix() + pObject->getName() );
    setText( 1, pObject->getCode() );
    setText( 2, pObject->getComment() );

    slotModified();

    connect( pObject, SIGNAL(signalSelected(bool)), this, SLOT(slotSelected(bool)) );
    connect( pObject, SIGNAL(signalModified()), this, SLOT(slotModified()) );
    connect( pObject, SIGNAL(signalState()), this, SLOT(slotState()) );
    connect( pObject, SIGNAL(signalDeleted(ADObject*)), this, SLOT(slotChildDeleted(ADObject*)) );
    connect( pObject, SIGNAL(signalCreated(ADObject*)), this, SLOT(slotChildCreated(ADObject*)) );

    connect( pObject, SIGNAL(signalChildAdded(ADObject*)), this, SLOT(slotChildCreated(ADObject*)) );
    connect( pObject, SIGNAL(signalChildRemoved(ADObject*)), this, SLOT(slotChildDeleted(ADObject*)) );

    // now do the children
    QList<ADObject*> listChildren = pObject->getObjects();
    ADObject *pChild;
    foreach( pChild, listChildren )
    {
        Q_ASSERT(pChild->inherits( "AWObject" ));
        getNewChild( (AWObject*)pChild ); 
    }
}

AWTreeWidgetObject *AWTreeWidgetObject::getNewChild( AWObject *pObject )
{
    if ( pObject->getFolder().isEmpty() )
        return new AWTreeWidgetObject( pObject, this );
    else
        return new AWTreeWidgetObject( pObject, getFolder( pObject ) );
}

AWTreeWidgetFolder *AWTreeWidgetObject::getFolder( AWObject *pObject )
{
    // find it
    for ( int n = 0; n < childCount(); n++ )
    {
        AWTreeWidgetItem *p = (AWTreeWidgetItem*)child( n );

        if ( p->inherits( "AWTreeWidgetFolder" ) )
        { 
            AWTreeWidgetFolder *pFolder = (AWTreeWidgetFolder*)p;
            if ( pFolder->getText() == pObject->getFolder() ) return pFolder;
        }
    }
    // not found so create
    return new AWTreeWidgetFolder( this, pObject->getIcon( isExpanded() ? AWObject::nIconRoleExpanded : AWObject::nIconRoleCollapsed ), pObject->getFolder() );
}


