#ifndef H_PMReference
#define H_PMReference

#include "PMLine.h"

#include "PMObject.h"
#include "PMDDLManager.h"

class PMModel;
class PMTableBase;
class PMKey;
class PMReferenceColumn;
class PMColumn;

/*!
 * \brief A reference from one table to another. 
 *  
 * The line Begins at the 'Child' table and Ends at the 'Parent' table (with an arrow). 
 * The Parent table provides a Key (primary or alternate). The Key Columns are matched 
 * to Columns in the Child table. The matched Columns in the Child table become 
 * 'foreign keys'. 
 *  
 * Implementation 
 *  
 * The implementation of this can be tricky to understand as an effort is made to be 'smart' 
 * about maintaining the reference - even while dependent parts of the diagram are changed. 
 * So its worth explaining here. 
 *  
 * ParentTable  - The table with the Primary or Alternate key (\sa PMKey). 
 *                The End EOL is connected to this and its indicated by an arrow.
 *  
 * ChildTable   - The table with the columns to be used as foreign key. 
 *                The Begin EOL is connected to this. 
 *  
 * Creating Object Associations 
 *  
 *      1. PMColumn <-- PMKey 
 *  
 *      A table can have 0-1 Primary Key and can have 0-n Alternate Key. These are created/deleted
 *      in \sa PMKeysWidget. The PMKey is simply a child object of the table.
 *  
 *      A PMKey can 'Use' 0-n PMColumn's in the same table. This done by creating a ref from the
 *      key to the column. This is done via \sa PMColumnsUsedWidget.
 *  
 *      2. ParentTable <-- ChildTable
 *  
 *      This is done by dropping the Begin EOL on the Child Table and dropping the End EOL on
 *      the Parent Table. This 'glues' the \sa DSource and \sa DSink and also creates a ref
 *      from ChildTable to ParentTable.
 *  
 *      \sa DLine::doConnect
 *      \sa DLine::doDisconnect
 *  
 *      3. PMKey <-- PMReference
 *  
 *      The PMReference can be associated with a specific key in the ParentTable it is connected
 *      to. This is done in \sa PMReferenceWidget using \sa PMReference::setKey.
 *  
 *      A \sa PMReferenceColumn is created for each column in the key. PMReferenceColumn is a
 *      child object of PMReference. All PMReferenceColumn are deleted when the key is set to null.
 *  
 *      4. ParentTable::PMColumn <-- PMReferenceColumn
 *  
 *      A ref is created from PMReferenceColumn to a corresponding key column in the ParentTable.
 *      The direction of the ref differentiates it from the ref to the column in the ChildTable.
 *      The existence of PMReferenceColumn object is dependent upon this column existing and
 *      remaining in the key. These are created via \sa PMReference::setKey.
 *  
 *      \note This is effectively a ref that is dependent upon another ref. This is an important
 *            consideration in maintain the PMReference.
 *  
 *      5. PMReferenceColumn <-- ChildTable::PMColumn
 *  
 *      A PMColumn can be assigned as a 'foreign key' column via \sa PMReferenceColumnWidget. This
 *      creates a ref from the column to the ReferenceColumn. The direction of the ref differentiates
 *      it from the ref to the key column in the ParentTable.
 *  
 *      PMReferenceColumn can exist without a ref to a column in the ChildTable.
 *  
 * Maintaining Object Associations 
 *  
 *      There are a number of interventions made to maintain PMReference. This is the main reason
 *      for this detailed expanation.
 *  
 *      1. ParentTable deleted
 *      2. ChildTable deleted
 *      3. PMKey deleted.
 *      4. PMKey column added/removed to/from key.
 *      5. PMKey column removed from table.
 *      6. Foreign key column deleted.
 *      7. Begin/End EOL moved.
 *      8. Entire line moved.
 *  
 *      Here is an explanation for each;
 *  
 *      1. ParentTable deleted
 *  
 *      This is handled in \sa PMTableBase::slotDelete. This deletes the \sa PMKey which triggers
 *      a disassociation between PMReference and the PMKey entirely - including foreign key refs.
 *  
 *      2. ChildTable deleted
 *  
 *      Here we rely on default behaviour of \sa ADObject to remove any refs. (ie for table and
 *      columns).
 *  
 *      3. PMKey deleted.
 *  
 *      We catch ParentTable::ADObject::signalDeleted(ADObject*) to detect this.
 *      \sa PMReference::setKey
 *  
 *      4. PMKey column added/removed to/from key.
 *  
 *      We catch the keys \sa signalConnected and \sa signalDisconnected. This triggers the creation
 *      or deletion of a \sa PMReferenceColumn. In the case of a deletion we rely on default
 *      behaviour in \sa ADObject to simply remove the ref to ParentTable and ChildTable columns.
 *      In the case of creation we create a ref to the ParentTable column.
 *      The signal is connected/disconnected in \sa PMReference::setKey. 
 *  
 *      5. PMKey column removed from table.
 *  
 *      We catch ParentTable::ADObject::signalDeleted(ADObject*) to detect this.
 *      \sa PMReference::setKey
 *  
 *      6. Foreign key column deleted.
 *  
 *      This is handled by \sa ADObject destructor. Its worth noting that this, default, behaviour
 *      does NOT emit \sa ADObject::signalDisconnected. However; this is not a problem in this case.
 *      PMReferenceColumn works directly with this ref - so if the ref is gone we no longer ref
 *      the column (without further complications).
 *  
 *      7. Begin/End EOL moved.
 *  
 *      This case is caught by \sa PMReference::mouseReleaseHandle.
 *  
 *      \note We preserve the PMReference associations if the EOL is moved from/to the same table.
 *            In all other cases the associations are, silently, removed entirely.
 *  
 *      8. Entire line moved.
 *  
 *      This is handled in \sa PMReference::setPosDelta. At present; setPosDelta is only used to
 *      move an entire object - so this works. It calls \sa PMReference::setKey with a null to
 *      clear all PMReference associations.
 *  
 * \note \sa PMReference::doPostLoad connects the needed signal/slots for this after a load. 
 *  
 * \author pharvey (5/22/20)
 * 
 * \param pParent 
 */
class PMReference : public PMLine, public PMDDLManager
{
    Q_OBJECT
public:
    PMReference( PMModel *pParent );
    virtual ~PMReference();

    // SETTERS
    virtual void setPosDelta( const QPointF &pointDelta );
    virtual void setMatch( const QString &s );
    virtual void setOnDelete( const QString &s );
    virtual void setOnUpdate( const QString &s );
    virtual void setVisible( bool b = true );
    virtual void setRole( CBD::EOLTypes n, const QString &s );
    virtual void setKey( PMKey *pKey );

    // GETTERS
    virtual QPixmap             getIcon();
    virtual QString             getFriendlyClassName() { return tr( "Reference" ); }
    virtual ADObject *          getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual AWDataWidget *      getObjectWidget( QWidget *pWidgetParent = nullptr );
    static ADValidationDatabase getValidationConfiguration( ADValidationDatabase &mapClasses ); 
    virtual AWPropWidget *      getPropWidget( QWidget *pWidgetParent = nullptr );
    virtual QString             getMatch()      { return stringMatch;       }
    virtual QString             getOnDelete()   { return stringOnDelete;    }
    virtual QString             getOnUpdate()   { return stringOnUpdate;    }
    virtual PMKey *             getKey();
    virtual PMReferenceColumn * getReferenceColumn( PMColumn *pKeyColumn, bool bCreate = false );
    virtual QString             getRole( CBD::EOLTypes n );
    virtual QString             getParentTable();
    virtual QString             getChildTable();
    virtual PMTableBase *       getParentTableObject();
    virtual PMTableBase *       getChildTableObject();
    virtual QString             getCreateAuto();
    virtual QString             getDropAuto();

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElemThis );
    virtual void doPostLoad();
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doLoadDefaults();

    virtual void mouseReleaseHandle( DHandle *pHandle, const QPointF &pointScene );

public slots:
    virtual void slotDescription( const QString &s )    { setDescription( s );  }
    virtual void slotAnnotation( const QString &s )     { setAnnotation( s );   }
    virtual void slotKeyColumnConnected( ADObject *pUser, ADObject *pUsed, const QString &stringName );
    virtual void slotKeyColumnDisconnected( ADObject *pUser, ADObject *pUsed, const QString &stringName );
    virtual void slotTableObjectDeleted( ADObject * );

protected:
    QString stringOnDelete;
    QString stringOnUpdate;
    QString stringMatch;
    QString stringBeginRole;
    QString stringEndRole;

    // floating text for line ends show Role + Cardinality
    QGraphicsTextItem * pFloatingTextBegin;
    QGraphicsTextItem * pFloatingTextEnd;
    QPointF             pointDeltaFloatingTextBegin;
    QPointF             pointDeltaFloatingTextEnd;

    virtual QPointF getFloatingTextBeginPos();  // scene coord
    virtual QPointF getFloatingTextEndPos();    // scene coord

    virtual void doHandleMove( DHandle *pHandle, const QPointF &pointScenePos );
    virtual void doHandlesCreate();
    virtual void doHandlesSync();

    virtual void doSourcesCreate();
    virtual void doSourcesDelete();
    virtual void doSourcesSync();
};


#endif


