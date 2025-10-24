#ifndef H_AWTreeWidget
#define H_AWTreeWidget

#include "AWObject.h"

class AWTreeWidgetFolder;
class AWTreeWidgetItem;

/*!
 * \class AWTreeWidget 
 * \brief A tree widget with context menu & double-click handling. 
 * 
 * This extends QTreeWidget so that we can handle; 
 * - context menu requests 
 * - double click on an item 
 * - expand/collapse requests 
 *  
 * \sa AWObject 
 *  
 * \author pharvey (10/24/18)
 */
class AWTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    AWTreeWidget( QWidget *pParent );
    ~AWTreeWidget();

    virtual void setRoot( AWObject *pObject );

    AWObject *getRoot() { return pRoot; }

protected:
    AWObject *pRoot;

    virtual void startDrag( Qt::DropActions n );
    virtual void doSaveState();
    virtual void doLoadState();

protected slots:
    virtual void slotContextMenu( const QPoint &point );
    virtual void slotItemClicked( QTreeWidgetItem *pItem, int nColumn );
    virtual void slotItemDoubleClicked( QTreeWidgetItem *pItem, int nColumn );
    virtual void slotItemExpanded( QTreeWidgetItem *pItem );
    virtual void slotItemCollapsed( QTreeWidgetItem *pItem );
};

/*!
 * \brief Base class for tree widget items.
 *
 * This provides an interface used by AWTreeWidget.
 * 
 *  \sa AWTreeWidget
 */
class AWTreeWidgetItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    AWTreeWidgetItem( AWTreeWidget * pParent )
    : QTreeWidgetItem( pParent )
    {
    }
    AWTreeWidgetItem( AWTreeWidgetItem * pParent )
    : QTreeWidgetItem( pParent )
    {
    }

    virtual ~AWTreeWidgetItem() {}

    // SETTERS

    // GETTERS
    virtual AWObject *getObject() = 0;

    // DO'rs
    virtual void doMenu( const QPoint &point ) = 0;
    virtual void doExpanded() = 0;
    virtual void doCollapsed() = 0;
    virtual void doClicked() = 0;
    virtual void doDoubleClicked() = 0;
};

/*!
 * \brief A tree item to represent an AWObject. 
 *  
 * Events, such as a request for a context menu, are mostly handled by calling 
 * into the AWObject. 
 *  
 * Connects to some AWObject signals. For example; to handle the creation or deletion 
 * of a child AWObject. 
 *  
 * Will indicate selected and/or modified status of AWObject. 
 *  
 * \author pharvey (4/24/20)
 */
class AWTreeWidgetObject : public AWTreeWidgetItem
{
    Q_OBJECT
public:
    AWTreeWidgetObject( AWObject *pObject, AWTreeWidget * pParent );
    AWTreeWidgetObject( AWObject *pObject, AWTreeWidgetItem * pParent );
    virtual ~AWTreeWidgetObject();

    // SETTERS

    // GETTERS
    virtual AWObject *getObject() { return pObject; }

    // DO'rs
    virtual void doMenu( const QPoint &point );
    virtual void doExpanded();
    virtual void doCollapsed();
    virtual void doClicked();
    virtual void doDoubleClicked();

public slots:
    virtual void slotChildCreated( ADObject *p );
    virtual void slotChildDeleted( ADObject *p );
    virtual void slotSelected( bool );
    virtual void slotModified();
    virtual void slotState();

protected:
    AWObject *pObject;

    QString             getNamePrefix();
    AWTreeWidgetObject *getNewChild( AWObject *pObject );
    AWTreeWidgetFolder *getFolder( AWObject *pObject );

    void doInit( AWObject *pObject ); // do not make virtual - why?   
};

/*!
 * \brief A tree item which exists to group AWObjects into a 'folder' under their parent . 
 *  
 * This is usefull to declutter the object browser. 
 *  
 * The parent tree item will control this - so its pretty dumb. 
 *  
 * The parent (AWTreeWidgetObject) will create one of these when 
 * AWObject::getFolder() is not empty. 
 *  
 * This can *not* be a root item. 
 * The parent tree item for this *must* be a AWTreeWidgetObject. 
 * All of the child tree items *must* be AWTreeWidgetObject. 
 *  
 * \author pharvey (4/24/20)
 */
class AWTreeWidgetFolder : public AWTreeWidgetItem
{
    Q_OBJECT
public:
    AWTreeWidgetFolder( AWTreeWidgetObject *pParent, const QIcon &icon, const QString &s )
    : AWTreeWidgetItem( pParent )
    {
        setIcon( 0, icon );
        setText( 0, s );
        // we are never load on demand
        setChildIndicatorPolicy( QTreeWidgetItem::DontShowIndicatorWhenChildless );
    }

    virtual ~AWTreeWidgetFolder() {}

    // SETTERS

    // GETTERS
    virtual QString     getText() { return text( 0 ); }
    virtual AWObject *  getObject() { return nullptr; }

    // DO'rs
    virtual void doMenu( const QPoint & ) {}
    virtual void doExpanded() {}
    virtual void doCollapsed() {}
    virtual void doClicked() {}
    virtual void doDoubleClicked() {}
};

#endif

