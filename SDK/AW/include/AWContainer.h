#ifndef H_AWContainer
#define H_AWContainer

#include <ADDocument.h>

#include "AWObject.h"

/*!
 * \class AWContainer 
 * \brief Extends AWObject to be a container. 
 *  
 * This class handles AWContainer and AWFile children in a special way. Each of these 
 * are designed to save/load to a file. 
 *  
 * Save 
 *  
 * When this class encounters these during a save it will request that the child object 
 * save itself and then it will simply save a reference (file name) to the child. 
 *  
 * Load 
 *  
 * This class will create a AWContainer/AWFile object when encountered during a load 
 * and it will set the file name. However; the child is not asked to load itself 
 * until this is needed. For example; request to edit or for a search. 
 *  
 * Notable behaviour of a AWContainer...
 * 
 * - does not have an editor but does have a properties dialog
 * - file representing container contains a list of references to
 *   its child objects where each have;
 *             - class type of child
 *             - file location and name of child
 * - save only involves saving self - not children                
 * - close involves trying to close its children before trying to 
 *   close itself - this implies an attempt to save any changes first
 * - becomes modified under the following circumstances;
 *             - its properties are changed (i.e. Name/Desc)
 *             - children are added/removed
 *             - child file path or name changes
 * 
 * A derived class must implement doElementRead() to handle each tag read
 * from the XML file ( see doRead() ). The derived class does not have to 
 * implement anything for saving the file ( see doWrite() ).
 *  
 * \sa AWObject     - base class 
 * \sa AWContainer  - saves/loads self also saves/loads references to its children 
 * \sa AWWorkspace  - a special AWContainer... is the 'root' object
 * \sa AWFile       - saves/loads self and all of its children 
 * \sa AWFolder     - no persistence but good for organizing objects in a tree view (pass through)
 *  
 * \author pharvey (11/2/18)
 */
class AWContainer : public AWObject, public ADDocument
{
    Q_OBJECT
public:
    AWContainer( AWObject *pobjectParent = nullptr, const QString &stringName = QString() );
    virtual ~AWContainer();

    // SETTERS
    virtual void setFileName( const QString &stringFileName );

    // GETTERS
    virtual QPixmap         getIcon( AWObject::enumIconRoles nRole );
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "Container" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QString         getFileName();
    virtual QString         getFileSpec();
    virtual QString         getFileExtension();
    virtual QString         getRootDir() { return ( stringActiveFileName.isEmpty() ? QString() : QFileInfo( stringActiveFileName ).absolutePath() ); }
    virtual ADObject*       getObject( const QString &s, ADObject *p = nullptr );
    virtual bool            isNew()        { return getFileName().isEmpty(); }
    virtual bool            isLoaded()     { return bLoaded;   }

    // DO'rs
    virtual QDomElement doSave( QDomDocument *pdomDoc, QDomElement *pdomElemParent );
    virtual bool doLoad( QDomElement *pdomElem );
    virtual bool doLoadClass( QDomElement *pdomElemClass );
    virtual void doFindLoaded( ADObjectPathItem::enumValueTypes nValueType, ADObjectPathList listObjectPathItems, const ADFindCriteria &Criteria );

signals:
    void signalChangedFileName();
    void signalChangedFileName( const QString &stringFileName );

    // DO'rs
public slots:
    virtual void slotModified() { setModified( true ); }
    virtual bool slotDelete( ADObject * );

    virtual bool slotOpen( const QString &stringFileName );          
    virtual AWObject::SaveResults slotSave( bool bRequest = true );  
    virtual AWObject::SaveResults slotSaveAs( bool bRequest = true );
    virtual AWObject::SaveResults slotClose( bool bRequest = true ); 

protected:
    bool        bLoaded;
    QString     stringFileName;         // file name - is also alt for name when not loaded 
    QString     stringActiveFileName;   // trying out a file name during a save/load

    // SETTERS

    // GETTERS

    // DO'RS
    virtual AWObject::SaveResults doSaveChildren( bool bRequest = true );
    virtual AWObject::SaveResults doCloseChildren( bool bRequest = true );

    virtual bool doRead( const QString &stringFileName );
    virtual bool doWrite( const QString &stringFileName );
    virtual void doClear();

protected slots:
    virtual void eventModified( ADObject *pSource );
};

/*!
 * \class AWPrefContainerWidget 
 * \brief View/edit preferences for a AWContainer.
 * 
 * \author pharvey (4/3/22)
 */
class AWPrefContainerWidget : public QWidget
{
    Q_OBJECT
public:
    AWPrefContainerWidget( QWidget *pwidgetParent, const QString &stringPath = QString() );
    ~AWPrefContainerWidget();

protected slots:        
    void slotCustom( Qt::CheckState n );                           
    void slotName( const QString & );   
    void slotCode( const QString & );   
    void slotComment();                 
    void slotAuthor( const QString & ); 
    void slotVersion( const QString & );
                                                
protected:
    QString     stringPath;

    QCheckBox * pCustom;
    QLineEdit * plineeditName;
    QLineEdit * pCode;
    QTextEdit * ptexteditComment;
    QLineEdit * plineeditAuthor;
    QLineEdit * plineeditVersion;

    void doCustom( Qt::CheckState nState );
};

#endif


