#ifndef H_ADTransaction
#define H_ADTransaction

#include <CBD.h>

class ADObject;

/*!
 * \class ADTransaction 
 * \brief Global used to facilitate a copy-in-progress and a paste-in-progress etc. 
 *  
 * Copied model data can be pasted into the original model or another model - of correct class. 
 * The root is typically an object which reperesents a file.
 *  
 * The Copy is done by using the file doSave methods to get XML of selected objects and then 
 * putting it into the system clipboard instead of a file. 
 *  
 * Example; 
 *  
 *  g_Transaction->initCopy( pRoot, ADTransaction::CopyReferenceOnly ); 
 *  doSave( &domDoc, &domElemRoot );
 *  g_Transaction->fini();
 *  pClipboard->setText( domDoc.toString() );
 *  
 * The Paste is done by getting XML from the system clipboard then calling the file doLoad 
 * methods to create objects. 
 *  
 * Example; 
 *  
 *  domDoc.setContent( pClipboard->text() );
 *  domElemRoot = domDoc.documentElement();
 *  g_Transaction->initPaste( pRoot );
 *  doLoad( &domElemRoot );
 *  g_Transaction->fini();
 *  
 * A single instance of ADTransaction is, silently, created for the application and can be 
 * accessed via \sa g_Transaction. 
 *  
 * \author pharvey (12/10/19)
 */
class ADTransaction
{
public:
    // State of object hierarchy.
    enum TransactionStates
    {
        TransactionIdle,  // No transaction but the usual; navigating, creating, modifying, deleteing going on. 
        TransactionCopy,  // Copying objects to XML. Uses doSave.
        TransactionPaste, // Creating objects from XML. Uses doLoad.
        TransactionDelete,// Deleting objects
        TransactionSave,  // Saving objects into XML. Supports saving to a file and Copy.
        TransactionLoad   // Creating objects from XML. Supports loading from a file and Paste.
    };

    // Rule 1: Only selected objects (and their children) of given parent (pRoot) will get copied. References to unselected siblings are lost.
    // Rule 2: References to objects in *other* parents are handled according to CopyModes.
    // \note   Aside from CopyReferenceOnly - references are followed through objects and may result in many objects becoming involved.
    enum CopyModes  // order of these are important
    {
        CopyReferencedObjects = 0,  // Create new, copied, dependent objects when pasted. Can be pasted into a different parent object of same class.
        CopyReferenceOnly,          // Connect pasted objects to same dependents. Can only be pasted into same parent object.
        CopyReferenceNew            // Create new, empty, dependent objects when pasted. Can be pasted into a different parent object of same class.
    };

    // Rule 1: Only selected objects (and their children) of given parent (pRoot) will get deleted. References to unselected siblings are lost but objects left.
    // Rule 2: References to objects in *other* parents are handled according to DeleteModes.
    enum DeleteModes // order of these are important
    {
        DeleteReferencedUnused = 0, // Try to delete referenced object when it no longer has references. May be restricted so its a request.
        DeleteReferencedNever       // Just remove reference - leave referenced object.
    };

    ADTransaction();
    ~ADTransaction();

    // This is our single, global, instance. \sa g_Transaction
    static ADTransaction *instance();

    // All object-paths (usually a list of OID's) are relative to the root.
    // It does not matter where the dependencies lead us - we can always find them with the object-path.
    // The root must encompass all dependencies.
    ADObject *pRoot;

    // COPY
    // 
    // This is used to prevent copying an object more than once. 
    // This can happen when an object is referenced more than once by other objects being copied and
    // even when it is being reference twice by the same object.
    // 
    // Code should...
    // 
    // if ( g_Transaction->hashObjectsCopied.contains( pObject ) ) { do not copy }
    //
    // if ( pObject copied ) { g_Transaction->hashObjectsCopied.insert( pObject ) }
    // 
    QSet<ADObject*> hashObjectsCopied;

    // PASTE
    //
    // Some object OID's will be changed during a Paste. 
    //
    // For example;
    //  - reparenting top-level - resulting in the new parent providing a different OID
    //  - creating new objects to satisfy dependencies
    //
    // This poses a problem when trying to resolve references during doPoastLoad. 
    // For this reason we keep track of the oldOID's and provide a XRef to the newOID's.
    // 
    // Objects being pasted may exist anywhere in the object hierarchy and their relationships
    // to each other can be anything (due to references). So rely on the paste context to know where
    // to create them, based upon the class, and we rely on complete paths to objects for xref. 
    // 
//    QMap<ADObject*,QMap<QString,QString>> mapXRef;

    // <source-path,destination-path>
    QMap<QString,QString> mapXRef;

    bool initCopy( ADObject *p, CopyModes nCopy = CopyReferenceOnly  );
    bool initPaste( ADObject *p );
    bool initDelete( ADObject *p, DeleteModes nDelete = DeleteReferencedUnused );
    bool initDelete();
    bool initSave( ADObject *p );
    bool initLoad( ADObject *p );

    void fini(); // back to idle

    bool isIdle() { return (nState == TransactionIdle); }
    bool canCopy() { return (nState == TransactionCopy); }
    bool canPaste() { return (nState == TransactionPaste); }
    bool isDelete() { return (nState == TransactionDelete); }
    bool isSave() { return (nState == TransactionSave); }
    bool isLoad() { return (nState == TransactionLoad); }

    TransactionStates getState() { return nState; }
    CopyModes getCopyMode() { return nCopyMode; }
    DeleteModes getDeleteMode() { return nDeleteMode; }
    QProgressDialog *pProgress = nullptr;

    void doDumpXRef();

protected:
    TransactionStates nState;
    CopyModes nCopyMode;
    DeleteModes nDeleteMode;
};

// Simplify access to our single, global, instance.For example;
//  
//          g_Transaction->canCopy() 
//
#define g_Transaction ADTransaction::instance()

#endif

