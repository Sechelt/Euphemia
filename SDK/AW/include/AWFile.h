#ifndef H_AWFile
#define H_AWFile

#include <ADDocument.h>

#include "AWObject.h"

/*!
 * \class AWFile
 * \brief An ADObject which is a document (saves/loads to/from a file and has an editor).
 *  
 * Use this class when your model is saved in a file AND you want to support an editor. See AWContainer
 * for folder-like model.
 * 
 * The difference between this and AWContainer is mostly in how/when the Open/Close/Save logic 
 * works. This class can work two ways depending upon getOpenWithEditor(). 
 * 
 * 1. By default; the file is not loaded automatically. The application will call slotOpen()
 * and slotClose() when needed. Presumably the application would also provide a means of 
 * calling slotOpenEditor(), slotCloseEditor() and slotObjectDialog() sometime between slotOpen()
 * and slotClose().
 * 
 * 2. Call setOpenWithEditor( true ) when you create the model if you want the file to be loaded
 * in slotOpenEditor() and saved in slotCloseEditor(). This means that the model data should be
 * considered invalid when no editor is open. 
 * 
 * It is possible that these two modes can be mixed. For example; to load the file when either
 * the edit is opened OR when its listviewitem is expanded. This would require that the application
 * use this class with special care and make intelligent use of setOpenWithEditor().
 *  
 * Unloaded files may be silently loaded/unloaded to carry out a search or validation. 
 *  
 * \sa AWObject     - base class 
 * \sa AWContainer  - saves/loads self also saves/loads references to its children 
 * \sa AWWorkspace  - a special AWContainer... is the 'root' object
 * \sa AWFile       - saves/loads self and all of its children 
 * \sa AWFolder     - no persistence but good for organizing objects in a tree view (pass through)
 *  
 * \author pharvey (11/3/18)
 */
class AWFile : public AWObject, public ADDocument
{
    Q_OBJECT
public:
    AWFile( ADObject *pModelParent = nullptr, const QString &stringName = QString() );
    virtual ~AWFile();

    // SETTERS

    // GETTERS
    virtual QPixmap         getIcon();
    virtual QString         getFriendlyClassName() { return tr( "File" ); }
    virtual AWDataWidget *  getObjectWidget( QWidget *pWidgetParent = nullptr );
    virtual QString         getFileName();
    virtual QString         getFileSpec( const QString &stringName = QString() );
    virtual QString         getFileExtension();
    virtual bool            isNew() { return getFileName().isEmpty(); }

    // DO'rs
    virtual bool doOpen( const QString &stringFileName );

signals:
    void signalChangedFileName();
    void signalChangedFileName( const QString &stringFileName );

public slots:
    virtual bool slotOpen();                                         
    virtual AWObject::SaveResults slotSave( bool bRequest = true );  
    virtual AWObject::SaveResults slotSaveAs( bool bRequest = true );
    virtual AWObject::SaveResults slotClose( bool bRequest = true ); 

protected:
    QString stringFileName; // file name. empty = new and not saved yet

    virtual void setFileName( const QString &stringFileName );

    virtual bool doRead( const QString &stringFileName );
    virtual bool doWrite( const QString &stringFileName );
    virtual void doClear();
};

#endif


