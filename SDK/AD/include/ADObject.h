#ifndef H_ADObject
#define H_ADObject

#include <CBDPersist.h>

#include <QDomDocument>

class DATADiagnostic;
class DATAMessage;
class ADObject;
class ADObjectSelectionManager;

/*!
 * \brief Root object. 
 *  
 * The ADObject's are organized into a hierarchy with a single ADObject at the root level. 
 * This global should be set when the hierarchy is created even if there is just one object. 
 * It should otherwise be set to nullptr. 
 * This will usually happen in the app code.
 *  
 * \author pharvey (4/16/23)
 */
extern ADObject *g_Root;

#include "ADTransaction.h"
#include "ADObjectReference.h"
#include "ADValidation.h"
#include "ADObjectPath.h"
#include "ADExport.h"

class ADFindCriteria
{
public:
    ADFindCriteria()
    {
        bName           = true;
        bCode           = true;
        bComment        = true;
        bDynamicData    = true;
        bOther          = true;
        bCaseSenstive   = false;
        bCompleteWord   = false;
        bCompleteField  = false;
    }
    ADFindCriteria( const QString &s )
    {
        bName           = true;
        bCode           = true;
        bComment        = true;
        bDynamicData    = true;
        bOther          = true;
        bCaseSenstive   = false;
        bCompleteWord   = false;
        bCompleteField  = false;
        stringValue     = s;
    }

    bool bName;
    bool bCode;
    bool bComment;
    bool bDynamicData;
    bool bOther;
    bool bCaseSenstive;
    bool bCompleteWord;
    bool bCompleteField;

    QString stringValue;
};

class ADEventOutput : public QObject
{
    Q_OBJECT                                     
public:
    // this is in case we want to direct focus to source object most commonly by;
    // - single click on output item to set focus on object
    // - double click on output item to go to object
    ADObjectPathList listObjectPath;
};

class ADEventRequest : public ADEventOutput
{
    Q_OBJECT                                     
public:
    enum RequestTypes
    {
        RequestFocus,
        RequestClear
    };
    ADEventRequest( const QString s, int n ) 
    {
        stringCategory  = s;  
        nRequest        = n; 
    }

    QString stringCategory; // "Validation", "Search", etc
    int     nRequest;
};

class ADEventValidation : public ADEventOutput
{
    Q_OBJECT                                     
public:
    ADEventValidation( ADObjectPathList listObjectPathItems, ADValidation::Severity nSeverity, const QString &stringMessage )
    {
        this->listObjectPath = listObjectPathItems;
        this->nSeverity = nSeverity;
        this->stringMessage = stringMessage;
    }
    ADValidation::Severity nSeverity;
    QString stringMessage;
};

class ADEventGeneral : public ADEventOutput
{
    Q_OBJECT                                     
public:
    ADEventGeneral( const QString &stringSeverity, const QString &stringMessage )
    {
        this->stringSeverity = stringSeverity;
        this->stringMessage  = stringMessage;
    }
    QString stringSeverity;
    QString stringMessage;
};

class ADEventSearch : public ADEventOutput
{
    Q_OBJECT                                     
public:
    ADEventSearch( ADObjectPathList listObjectPathItems, const QPixmap &Pixmap, const QString &stringMatch )
    {
        this->listObjectPath = listObjectPathItems;
        this->Pixmap = Pixmap;
        this->stringMatch = stringMatch;
    }
    QPixmap Pixmap;      // icon of source
    QString stringMatch; // matching text
};

class ADDataEventDiagnostic : public ADEventOutput 
{
    Q_OBJECT                                     
public:
    ADDataEventDiagnostic( DATADiagnostic *p ) { pDiagnostic = p; }
                                       
    DATADiagnostic *pDiagnostic;        
};                                    
                                      
class ADDataEventGeneral : public ADEventOutput 
{                                     
    Q_OBJECT                                     
public:                               
    ADDataEventGeneral( DATAMessage *p ) { pMessage = p; }

    DATAMessage *pMessage;
};
                                      
class ADDataEventResult : public ADEventOutput
{                                     
    Q_OBJECT                                     
public:                               
    int nRowsReturned;                
    int nRowsAffected;                
    int nSeconds;                     
};                                    

/*!
 * \brief Contains information about what this object can create via \sa getObject(). 
 *  
 * 0-n of these are created and stored in ADObject::mapMeta. This is done in the 
 * constructor of ADObject based objects. 
 * Classes derived from ADObject can append more or delete as they see fit.
 * 
 * \author pharvey (4/5/23)
 */
class ADMeta
{
public:
    ADMeta() {}
    ADMeta( const QIcon &t1, const QString &t2, const QString &t3, const QStringList t4 = QStringList() )
    {
        icon = t1;
        stringClass = t2;
        stringFriendlyName = t3;
        listFeatures = t4;
    }

    QString     stringClass;            /*!< the class name - is also the unique key                        */
    QIcon       icon;                   /*!< icon that can be used in the UI                                */
    QString     stringFriendlyName;     /*!< friendly name that can be used in the UI                       */
    QStringList listFeatures;           /*!< feature list - can be used to filter before presenting in UI   */
};

/*! 
  \class ADObject 
  \brief Extends QObject to include; OID, custom searching, persistence, and some common properties.
    
    This is used very much like QObject in that it forms the base for just
    about everything but we extend QObject to add more AD functionality.
    
    ADObject is used to support simple, applications and more
    complex, MDI applications. Like QObject, ADObject classes do not always
    require a GUI at all - depends upon what the derived classes 
    introduce.
*/
class ADObject : public QObject
{
    Q_OBJECT
public:
    enum Fields
    {
        FieldName,
        FieldCode
    };
    ADObject( ADObject *pobjectParent = nullptr, const QString &stringName = QString() );
    virtual ~ADObject();

    // SETTERS
    virtual void setOID( int nOID ) { this->nOID = nOID; }
    virtual void setNextOID( int nOID ) { this->nNextOID = nOID; }
    virtual void setNextOIDTemp( int nOID ) { this->nNextOIDTemp = nOID; }
    virtual void setOIDSource( ADObject *pobjectOIDSource ) { this->pobjectOIDSource = pobjectOIDSource; } // only call when no children yet
    virtual void setName( const QString &sringName );
    virtual void setCode( const QString &sringCode );
    virtual void setComment( const QString &srgetValidationSeverityingComment );
    virtual void setSelected( bool b = true );                 // select/unselect this
    virtual void setModified( bool b = true );
    virtual void setState( int n = 0 );
    virtual void setValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation, ADValidation::Severity nSeverity );
    virtual void setScript( const QString &stringName, const QString &stringScript );

    // GETTERS
    virtual int                         getOID() { return nOID; }
    virtual ADObject *                  getOIDSource() { return pobjectOIDSource; }
    virtual int                         getNextOID( bool bForPersist = true );
    virtual QString                     getName() { return stringName; }
    virtual QString                     getCode() { return stringCode; }
    virtual QString                     getComment() { return stringComment; }
    virtual ADObject *                  getParentPaste( const QString &stringClass ) { Q_UNUSED(stringClass); return this; } // best guess - derived classes need to do better
    virtual ADObject *                  getParent( const char *pszClassName /* inherits className */ );
    virtual ADObjectSelectionManager *  getSelectionManager();
    virtual ADObjectPathList            getPath( ADObject *pobjectRoot );
    virtual ADObjectPathList            getPath();
    virtual QString                     getPathString( ADObject *pobjectRoot );
    virtual ADObject *                  getDuplicate( ADObject *pObject, ADObject::Fields nField );
    virtual QStringList                 getEvents();
    virtual QStringList                 getScripts();
    virtual QString                     getScript( const QString &stringName );
    virtual ADObject *                  getObject( int nOID );
    virtual ADObject *                  getObject( QStack<int> &stackPath );
    virtual ADObject *                  getObject( const QStringList &l );
    virtual ADObject *                  getObject( ADObjectPathList listObjectPathItems, uint nDepth = 0 );
    virtual ADObject *                  getObject( ADObject *pobject, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString(), bool bRecurse = true );
    virtual QList<ADObject*>            getObjects( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass, int nIndex = -1 );
    virtual QStringList                 getObjectsStringList( const QString &stringClassName, const QString &stringInheritsClass );
    virtual QList<ADObject*>            getObjects() { return findChildren<ADObject*>( QString(), Qt::FindDirectChildrenOnly ); } // \todo use QObject::findChildren() (as found here) more often
    virtual QList<ADObject*>            getUses( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass, int nIndex = -1 );
    static  QString                     getPathDiff( const QString &stringPathAbsStart, const QString &stringPathAbsEnd );
    static  ADValidationDatabase        getValidationConfiguration( ADValidationDatabase &mapClasses ); // get validation config
    virtual ADValidation::Severity      getValidationSeverity( const QString &stringClass, ADValidation::Strictness nStrictness, const QString &stringValidation ); // lookup severity
    virtual ADMeta                      getMeta( const QString &stringClass ) { return mapMeta.value( stringClass ); }
    virtual QStringList                 getClasses( bool bFriendly = false );
    virtual QString                     getClass( const QString &stringName );
    virtual ADObject *                  getObject( const QString &stringClass, ADObject *pObjectParent = nullptr );
    virtual ADObject *                  getObjectAnyParent( const QString &stringClass );
    virtual bool                        hasChildren() { return children().count(); }
    virtual bool                        isSelected() { return bSelected; }
    virtual bool                        isModified() { return bModified; }
    virtual int                         getState() { return nState; }

    // DO'rs
    virtual void doChildRemove( ADObject *p );      // reparenting
    virtual void doChildAdd( ADObject *p );         // reparenting

    virtual void doScriptRemove( const QString &stringName );

    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual bool doLoadReference( int *pnRef, QDomElement *pdomElemReference );
    virtual bool doLoadClass( QDomElement *pdomElemClass );

    virtual bool doLoad( ADObject *pobject );
    virtual bool doSave( ADObject *pobject );
    virtual void doPreSave();
    virtual void doPostLoad();
    virtual void doObjectDelete( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBack();
    virtual void doToBack( QList<ADObject*> listChildren );
    virtual void doToFront();
    virtual void doToFront( QList<ADObject*> listChildren );
    virtual void doToPrev( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToNext( const QString &stringName, const QString &stringClassName, const QString &stringInheritsClass );
    virtual void doToBefore( ADObject *pObject );
    virtual void doToAfter( ADObject *pObject );
    virtual void doToIndex( int nIndex );
    virtual void doValidate( ADValidation::Strictness nStrictness, ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems );
    virtual void doFind( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria );
    virtual void doDump( int nLevel = 0 );
    virtual void doMessageBox( const QString &stringSeverity, const QString &stringCaption, const QString &stringMessage );
    virtual void doMessage( const QString &stringSeverity, const QString &stringMessage );

    // references
    // we are primarily a 'hierarchial' data store but here we blend in the concept of 'networked'
    // ie we are an entity attribute (LMAttribute) and we 'use' a predefined data type (LMDataItem) as our data type
    // ie we are a connector of some sort (DLine) and we connect two shapes (DRectangle)
    virtual bool doConnect( ADObject *pObject, const QString &stringName = QString() );                             // create a reference to pObject 
    virtual bool doDisconnect( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual bool isConnected( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual ADObject *getConnected( const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual int getUsedByCount( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual int getUsesCount( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual QList<ADObjectReferenceKey> getUsedByKeys( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual QList<ADObjectReferenceKey> getUsesKeys( ADObject *pObject = nullptr, const QString &stringName = QString(), const QString &stringClassName = QString(), const QString &stringInheritsClass = QString() );
    virtual void doUsedByRemove( ADObject *pObject, const QString &stringName = QString() );    // supporting or called from destructors
    virtual void doUsesRemove( ADObject *pObject, const QString &stringName = QString() );      // supporting or called from destructors

    virtual bool isSelectable() { return bSelectable; }

signals:
    void signalChildAdded( ADObject *p );                 // reparenting not creating
    void signalChildRemoved( ADObject *p );               // reparenting not deleting

    void signalCreated( ADObject *pParent, ADObject *pChild );  // echo'd up the object hierarchy whenever a ADObject is created
    void signalCreated( ADObject *pChild );                     // same as above but NOT echo'd up the object hierarchy
    void signalModified( ADObject *p );                         // signals parent that we were modified - parent may echo up - or not (ie in the case of \sa AWFile)
    void signalModified();                                      // signals that we have - changed used by \sa AWTreeWidget and others
    void signalState();                                         // signals that we have - changed used by \sa AWTreeWidget and others
    void signalSelected( ADObject *, bool );                    // echo'd up the object hierarchy whenever a ADObject is selected or unselected
    void signalSelected( bool );                                // same as above but NOT echo'd up the object hierarchy
    void signalDeleted( ADObject *pParent, ADObject *pChild );  // echo'd up the object hierarchy whenever a pChild has been deleted (as in DO NOT dereference pChild)
    void signalDeleted( ADObject *pChild );                     // similar to above but NOT echo'd up and sent just *before* it is to be deleted
    void signalOutput( ADEventOutput *pEvent );                 // messages for an output window (some messages should be caught at root for proper nav back to source)
    void signalDeleteRequest( ADObject * );                     // Connect this to slotDelete in parent. Emitted to request we be deleted. Top level should be connected to application.

    void signalConnected( ADObject *pUses, ADObject *pUsedBy, const QString &stringName );      // reference created
    void signalDisconnected( ADObject *pUses, ADObject *pUsedBy, const QString &stringName );   // reference removed

    //
    void signalName( const QString &s );
    void signalCode( const QString &s );
    void signalComment( const QString &s );

public slots:
    virtual void slotFind( const ADFindCriteria &Criteria );
    virtual void slotValidate( ADValidation::Strictness n = ADValidation::High );
    virtual void slotDelete();                          // Simply emits signalDeleteRequest( this ). To be triggered by context/popup menu
    virtual bool slotDelete( ADObject *pChild );        // Triggered by child to request that it (the child) be deleted.
    virtual void slotGoTo( ADObjectPathList listObjectPathItems );
    virtual void slotGoToView( ADObjectPathList listObjectPathItems );
    virtual void slotExportHTML();
    virtual void slotExportRTF();

protected:
    // To support doConnect()/doDisconnect(). Do NOT call directly. 
    virtual void doUsedByAdd( ADObject *pObject, const QString &stringName = QString() );
    virtual void doUsesAdd( ADObject *pObject, const QString &stringName = QString() );
    QMap<ADObjectReferenceKey,ADObjectReference> mapUsedBy;     // references that others created to us
    QMap<ADObjectReferenceKey,ADObjectReference> mapUses;       // references we created

    // Object factory 
    // \sa getClasses()
    // \sa getObject( QString )
    QMap<QString,ADMeta> mapMeta;       // Classes we can instantiate. This is init in constructor.                

    // events
    // - each event has a unique name which may (or may not) have a macro or script attached to it
    // - a macro starts with a '=' ... it is a script otherwise
    // - a macro can call a script/module in mapScripts ... this allows a script to be shared
    QMap<QString,QString> mapEvents;    // default: add/remove in constructors
    QMap<QString,QString> mapScripts;   // default: no scripts
    
    // Object Identifier (OID)
    //
    // nOID can be used to uniquely identify an object - even when ADObject is persisted.
    //
    // 1. The OID starts off with value of zero (not set). OID is automatically assigned a temp OID
    //    using parents getNextOID() when parent given.
    //
    //    The OID may subsequently be replaced (i.e. if loaded from a file) by calling setOID(). 
    //
    // 2. An OID is unique within its scope. The scope is determined by the location of the ADObject
    //    designated as the OID source. By default every ADObject is an OID source for its
    //    children. This ensures unique OID's among children. The scope can be changed by calling
    //    setOIDSource().
    //
    // 3. A nOID value of zero indicates that nOID has not been set. A value of less than zero indicates 
    //    that its a temp OID a value greater than zero indicates that its an OID which may be persisted.
    //
    // 4. The nOID may be accurate if persisted (i.e. saved to a file and reloaded) but only if
    //    the app ensures that the method used to generate the nOID does not reuse the value.
    //    This can be done by using getNextOID( true ) to generate the nOID and also saving/restoring 
    //    the OID source's nNextOID. Also; in this case you do not want to call the parents getNextOID() 
    //    when loading the object from a file. Instead use something like;
    //
    //         pobjectNew->setOID( nFromFileOID );
    //
    // 5. At this time there is no attempt made to handle the possibility of exceeding the max 
    //    value of nOID or nNextOID. We rely on the max value being large enough.
    //
    // 6. ADObject once used the pointer, converted to a string, for each object but this was limiting. nOID
    //    is a better solution :)
    int             nOID;                   // our unique identifier - 0 means not set
    int             nNextOID;               // next OID for child - OID values of > 0 can be persisted (nNextOID should be saved as well)
    int             nNextOIDTemp;           // next OID for child - OID values of < 0 must not be persisted
    ADObject *      pobjectOIDSource;        // we control OID's for our children if NULL

    //              
    QString         stringName;             // Should be same as QObject::name but we keep our own copy because name() will return 'unnamed' when empty... we want empty strings!
    QString         stringCode;             //
    QString         stringComment;
    bool            bSelectable;
    bool            bSelected;
    bool            bModified;              // data has changed (need to consider indicating in UI and saving)
    int             nState;                 // state has changed (need to consider indicating in UI)

    // SETTERS

    // GETTERS
    virtual QString getExportDescription( ADExportHTML *pexport );                     // should only be called by doExport()

    // DOERS
    virtual void doExport( ADExportHTML *pexport, bool bReportProgress = false );      // should only be called by slotExportHTML()
    virtual void doExport( ADExportRTF *pexport, bool bReportProgress = false  );      // should only be called by slotExportRTF()

    //
    virtual void doGoTo( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doGoToView( ADObjectPathList listObjectPathItems, uint nDepth );
    virtual void doClear();

    // these get packaged in a ADEventOutput and sent out via a common signal signalOutput
    virtual void eventOutputGeneral( const QString &stringSeverity, const QString &stringMessage );
    virtual void eventOutputValidate( ADObjectPathList listObjectPathItems, ADValidation::Severity nSeverity, const QString &stringMessage );
    virtual void eventOutputFind( ADObjectPathList listObjectPathItems, const QPixmap &pixmap,  const QString &stringMatch );
    virtual void eventOutputRequest( const QString &s, int n );

    virtual void eventModified(); 

protected slots:
    virtual void eventModified( ADObject *pSource ); 
};

#endif


